set(Boost_NO_BOOST_CMAKE true)
set(Boost_NO_SYSTEM_PATHS true)
set(Boost_ADDITIONAL_VERSIONS "1.50.0")
find_package ( Boost )

include_directories( ${crave_SOURCE_DIR}/src ${metaSMT_INCLUDE_DIR} ${Boost_INCLUDE_DIRS} ${GLOG_INCLUDE_DIR})

set(EXAMPLES coin_dist crave_functionality ex1_seed_dist ex2_constr_ref
               ex3_inh_soft_next ex4_constr_mng ex5_vec_constr ex6_inline_gen
               ex7_randv_enum ex8_make_expression sudoku sudoku_free sudoku_free_perf)

if (CRAVE_ENABLE_EXPERIMENTAL)
  LIST(APPEND EXAMPLES ex9_experimental barn_drawer)
endif()

foreach (ex ${EXAMPLES})
  if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${ex}")
    add_executable(${ex} ${ex}/main.cpp)
    target_link_libraries(${ex} crave ${PTHREAD} ${GLOG_LIBRARY})
  endif()
endforeach(ex)

if(SystemC_FOUND)
  if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/ALU")
    include_directories(crave/src metaSMT/src)
    add_subdirectory(ALU)
  endif()
  if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/random_stability")
    include_directories(crave/src metaSMT/src)
    add_subdirectory(random_stability)
  endif()
  if (CRAVE_ENABLE_EXPERIMENTAL)
    if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/y86_err_injection")
  	  add_subdirectory(y86_err_injection)
    endif()
  endif()  
endif(SystemC_FOUND)

