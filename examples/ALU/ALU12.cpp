#include <crave/SystemC.hpp>
#include <crave/ConstrainedRandom.hpp>
#include <systemc.h>
#include <boost/timer.hpp>

using crave::rand_obj;
using crave::randv;
using sc_dt::sc_bv;
using sc_dt::sc_uint;

using crave::dist;
using crave::distribution;
using crave::reference;

struct ALU12 : public rand_obj {
  randv< sc_bv<2> >  op ;
  randv< sc_uint<12> > a, b ;

  ALU12(rand_obj* parent = 0)
  : rand_obj(parent), op(this), a(this), b(this)
  {
    randv<short> tmp(NULL);
    constraint ( dist(tmp(), distribution<short>::simple_range(0, 3)) );
    constraint ( tmp() == op() );    
    constraint ( (op() != 0x0) || ( 4095 >= a() + b() ) );
    constraint ( (op() != 0x1) || ((4095 >= a() - b()) && (b() <= a()) ) );
    constraint ( (op() != 0x2) || ( 4095 >= a() * b() ) );
    constraint ( (op() != 0x3) || ( b() != 0        ) );
  }

  friend std::ostream & operator<< (std::ostream & o, ALU12 const & alu) 
  {
    o << alu.op 
      << ' ' << alu.a
      << ' ' << alu.b
      ;
    return o;
  }
};

int sc_main (int argc, char** argv)
{
  crave::init("crave.cfg");
  boost::timer timer;
  ALU12 c;
  c.next();
  std::cout << "first: " << timer.elapsed() << "\n";
  for (int i=0; i<1000; ++i) {
    c.next();
    std::cout << c << std::endl;
  }
  std::cout << "complete: " << timer.elapsed() << "\n";
  return 0;
}
