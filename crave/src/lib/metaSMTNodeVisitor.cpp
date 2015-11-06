// Copyright 2014 The CRAVE developers. All rights reserved.//

#include "../crave/ir/visitor/metaSMTNodeVisitor.hpp"
#include "../crave/backend/FactoryMetaSMT.hpp"
#include "metaSMTNodeVisitorImpl.hpp"

#include <metaSMT/BitBlast.hpp>
#include <metaSMT/DirectSolver_Context.hpp>
#include <metaSMT/UnpackFuture_Context.hpp>

#include <glog/logging.h>
#include <string>

#define DEFINE_SOLVER(inCrave, inMetaSMT)                                                        \
  namespace crave {                                                                              \
  template <>                                                                                    \
  struct FactorySolver<inCrave> {                                                                \
    typedef metaSMT::DirectSolver_Context<metaSMT::UnpackFuture_Context<inMetaSMT> > SolverType; \
    static bool isDefined() { return true; }                                                     \
    static metaSMTVisitor* getNewInstance() { return new metaSMTVisitorImpl<SolverType>(); }     \
  };                                                                                             \
  }  // namespace crave

#ifdef metaSMT_USE_Boolector
#include <metaSMT/backend/Boolector.hpp>
DEFINE_SOLVER(crave::BOOLECTOR, metaSMT::solver::Boolector);
#endif

#ifdef metaSMT_USE_CVC4
#include <metaSMT/backend/CVC4.hpp>
DEFINE_SOLVER(crave::CVC4, metaSMT::solver::CVC4);
#endif

#ifdef metaSMT_USE_SWORD
#include <metaSMT/backend/SWORD_Backend.hpp>
DEFINE_SOLVER(crave::SWORD, metaSMT::solver::SWORD_Backend);
#endif

#ifdef metaSMT_USE_Z3
#include <metaSMT/backend/Z3_Backend.hpp>
DEFINE_SOLVER(crave::Z3, metaSMT::solver::Z3_Backend);
#endif

#ifdef metaSMT_USE_CUDD
#include <metaSMT/backend/CUDD_Distributed.hpp>
#include "../crave/RandomSeedManager.hpp"
namespace crave {
extern RandomSeedManager rng;
}
namespace metaSMT {
namespace solver {
class CUDD_Distributed_ : public CUDD_Distributed {
 public:
  CUDD_Distributed_() { gen.seed((*crave::rng.get())()); }
};
}
}
DEFINE_SOLVER(crave::CUDD, metaSMT::BitBlast<metaSMT::solver::CUDD_Distributed_>);
#endif

#undef DEFINE_SOLVER

namespace crave {

SolverTypes FactoryMetaSMT::solver_type_ = UNDEFINED_SOLVER;  // default solver

void FactoryMetaSMT::setSolverType(std::string const& type) {
  if (type == "Boolector")
    solver_type_ = BOOLECTOR;
  else if (type == "CVC4")
    solver_type_ = CVC4;
  else if (type == "Z3")
    solver_type_ = Z3;
  else if (type == "SWORD")
    solver_type_ = SWORD;
  else if (type == "Cudd")
    solver_type_ = CUDD;
}

#define TRY_GET_SOLVER(solver)                                                        \
  if (!FactorySolver<solver>::isDefined()) {                                          \
    solver_type_ = UNDEFINED_SOLVER;                                                  \
    LOG(INFO) << #solver << " has not been defined, another solver will be selected"; \
    return getNewInstance();                                                          \
  } else {                                                                            \
    return FactorySolver<solver>::getNewInstance();                                   \
  }

#define TRY_GET_SOLVER_WHEN_UNDEFINED(solver)                   \
  if (FactorySolver<solver>::isDefined()) {                     \
    solver_type_ = solver;                                      \
    LOG(INFO) << #solver << " has been automatically selected"; \
    return getNewInstance();                                    \
  }

metaSMTVisitor* FactoryMetaSMT::getNewInstance(SolverTypes type) {
  switch (type) {
    case BOOLECTOR:
      TRY_GET_SOLVER(BOOLECTOR);
    case CVC4:
      TRY_GET_SOLVER(CVC4);
    case Z3:
      TRY_GET_SOLVER(Z3);
    case SWORD:
      TRY_GET_SOLVER(SWORD);
    case CUDD:
      TRY_GET_SOLVER(CUDD);
    default:  // UNDEFINED_SOLVER
      TRY_GET_SOLVER_WHEN_UNDEFINED(BOOLECTOR);
      TRY_GET_SOLVER_WHEN_UNDEFINED(Z3);
      TRY_GET_SOLVER_WHEN_UNDEFINED(CVC4);
      TRY_GET_SOLVER_WHEN_UNDEFINED(SWORD);
      TRY_GET_SOLVER_WHEN_UNDEFINED(CUDD);
      assert(false && "No solver has been defined.");
  }
}

}  // namespace crave
