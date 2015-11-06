#include <crave/ConstrainedRandom.hpp>

using crave::dist;
using crave::rand_obj;
using crave::randv;
using crave::distribution;
using crave::range;
using crave::weighted_range;
using crave::weighted_value;
using crave::if_then;

class Coin : public rand_obj {
protected:
  uint _distVal;
  randv<uint> _coinVal;
  randv<bool> _coinBVal;

public:
  Coin(uint d, rand_obj* parent=0) : rand_obj(parent), 
                                     _coinVal(this), _coinBVal(this) {
    _distVal = d;
    constraint(dist(_coinVal(), 
      distribution<uint>::create
        (weighted_value<uint>(0, 100 - _distVal))
        (weighted_value<uint>(1, _distVal))
    ));	
    constraint(dist(_coinBVal(), distribution<bool>::create(_distVal / 100.)));
  };

  randv<uint>& operator()() {
    return _coinVal;
  }

  bool getCoinBVal() {
    return _coinBVal;
  }

  void setDistVal(uint d) {
    _distVal = d;
  }

  uint getDistVal() {
    return _distVal;
  }

  friend ostream& operator<<(ostream& os, Coin& it) { 
    os << "Coin value: " << it._coinVal << " - Dist: " << it._distVal;
    return os; 
  }

};

class testDist : public rand_obj {
protected:
  randv<uint> var1;
  Coin myCoin;
public:
  testDist(rand_obj* parent=0) : rand_obj(parent), var1(this), myCoin(50, this) {
    constraint("c1", if_then(reference(myCoin()) == 1, var1() == 4));
    constraint(dist(var1(), 
      distribution<uint>::create
        (weighted_range<uint>(0, 39, 14))
        (weighted_value<uint>(40, 65))
        (weighted_range<uint>(41, 100, 21))
    ));	
  };
  uint getVar() {
    return (uint)(var1);
  };
  bool getCoin() {
    return myCoin();
  };
  
  void test() {
    uint cnt_c = 0;
    uint cnt_v = 0;
    uint cnt_vc = 0;
    uint total = 1000;
    for (int i = 0; i < total; i++) {
      assert(next());
      std::cout << getCoin() << " " << getVar() << std::endl;
      if (getCoin()) cnt_c++;
      if (getVar() == 40) cnt_v++;
      if (!getCoin() && getVar() == 40) cnt_vc++;
    }
    std::cout << "P(Coin) = " << 1. * cnt_c / total << std::endl;  
    std::cout << "P(Var == 40) = " << 1. * cnt_v / total << std::endl;  
    std::cout << "P(Var == 40 given !Coin) = " << 1. * cnt_vc / (total - cnt_c) << std::endl;
  }

};

class testDist1 : public testDist {
public:
  testDist1(rand_obj* parent=0) : testDist(parent) {
    constraint.disableConstraint("c1");
    constraint(if_then(myCoin()() == 1, var1() == 4)); 
  };
};


int main (int argc , char *argv[]) {
  crave::init("crave.cfg");
  testDist1 td;
  td.test();	
  return 0;
}

