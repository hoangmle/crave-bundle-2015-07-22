#include <boost/test/unit_test.hpp>

#include <crave/ConstrainedRandom.hpp>

#include <boost/format.hpp>

#include <set>
#include <iostream>

// using namespace std;
using namespace crave;

using boost::format;

BOOST_FIXTURE_TEST_SUITE(Distribution_t, Context_Fixture)

BOOST_AUTO_TEST_CASE(randv_dist_t1) {
  randv<int> v(NULL);
  Generator gen;
  gen(dist(v(), distribution<int>::create(range<int>(0, 5))(range<int>(50, 65))(range<int>(100, 125))));

  std::map<int, int> s;
  int total = 100000;
  for (int i = 0; i < total; i++) {
    BOOST_REQUIRE(gen.next());
    BOOST_REQUIRE((0 <= v && v <= 5) || (50 <= v && v <= 65) || (100 <= v && v <= 125));
    ++s[v];
  }
  int min = s[0], max = s[0];
  for (int i = 1; i <= 200; i++)
    if (s.find(i) != s.end()) {
      if (s[i] < min) min = s[i];
      if (s[i] > max) max = s[i];
    }
  double avg = total / (6. + 16. + 26.);
  BOOST_REQUIRE_LT(100. * (avg - min) / avg, 5);
  BOOST_REQUIRE_LT(100. * (max - avg) / avg, 5);
}

BOOST_AUTO_TEST_CASE(randv_dist_t2) {
  randv<int> v(NULL);
  Generator gen;
  BOOST_CHECK_THROW(
      gen(dist(v(), distribution<int>::create(range<int>(0, 10))(range<int>(50, 75))(range<int>(30, 51)))),
      std::runtime_error);
}

BOOST_AUTO_TEST_CASE(randv_dist_t3) {
  randv<char> v(NULL);
  Generator gen;
  gen(dist(v(), distribution<char>::create(weighted_range<char>(1, 5, 50))(weighted_range<char>(10, 20, 20))(
                    weighted_range<char>(-50, -50, 30))));
  int cnt1 = 0, cnt2 = 0, cnt3 = 0;
  int total = 50000;
  for (int i = 0; i < total; i++) {
    BOOST_REQUIRE(gen.next());
    BOOST_REQUIRE((1 <= v && v <= 5) || (10 <= v && v <= 20) || (v == -50));
    if (1 <= v && v <= 5) cnt1++;
    if (10 <= v && v <= 20) cnt2++;
    if (v == -50) cnt3++;
  }
  double q = 50.0 / cnt1;
  BOOST_REQUIRE_LT(abs(cnt2 * q - 20), 0.2);
  BOOST_REQUIRE_LT(abs(cnt3 * q - 30), 0.2);
}

BOOST_AUTO_TEST_CASE(randv_dist_t4) {
  randv<int> v(NULL);
  Generator gen;
  gen("x", dist(v(), distribution<int>::create(range<int>(0, 10))(range<int>(50, 75))(range<int>(100, 200))));
  gen("y", dist(v(), distribution<int>::simple_range(5000, 6000)));
  gen.disableConstraint("x");
  int total = 10000;
  for (int i = 0; i < total; i++) {
    BOOST_REQUIRE(gen.next());
    BOOST_REQUIRE(5000 <= v && v <= 6000);
  }
}

BOOST_AUTO_TEST_SUITE_END()  // Context

//  vim: ft=cpp:ts=2:sw=2:expandtab
