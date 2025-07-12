#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>
#include "../Fraction.hpp"

#include <boost/multiprecision/cpp_int.hpp>

#include <boost/math/common_factor.hpp>

using namespace boost::multiprecision;

#include <bitset>


TEST_CASE("BitSet") {
  Fraction frac("12/8");
  REQUIRE(frac.numerator == 3);
  REQUIRE(frac.denominator == 2);

  // REQUIRE(frac.numSign == std::bitset<32>("10"));
  //
  // REQUIRE(frac.denSign == std::bitset<32>("01"));
}

TEST_CASE("BitSet 2") {
  Fraction frac("600112800/8748000");
  REQUIRE(frac.numerator == 343);
  REQUIRE(frac.denominator == 5);

  // REQUIRE(frac.numSign == std::bitset<32>("1000"));
  //
  // REQUIRE(frac.denSign == std::bitset<32>("100"));
}

TEST_CASE("BitSet 3") {
  Fraction frac("600112800/7919");
  REQUIRE(frac.numerator == 600112800);
  REQUIRE(frac.denominator == 7919);

  // REQUIRE(frac.numSign == std::bitset<32>("1011"));
  //
  // REQUIRE(frac.denSign == std::bitset<32>("000"));
}
