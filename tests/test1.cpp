
#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>

#include <boost/multiprecision/cpp_int.hpp>

using namespace boost::multiprecision;

bool isEvenFactors(const cpp_int& n, const cpp_int& f) {
  cpp_int n1 = n;

  size_t count = 0;
  while (n1 % f == 0) {
    n1 = n1 / f;
    count += 1;
  }

  return count % 2 == 0;
}

TEST_CASE("sample 1", "1==1"){
    REQUIRE(1==1);
}


TEST_CASE("sample 2", "1==1"){
  REQUIRE(isEvenFactors(2, 2)  == false);

  REQUIRE(isEvenFactors(4, 2)  == true);
  REQUIRE(isEvenFactors(22146434, 2)  == true);

}

