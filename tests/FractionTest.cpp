#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>
#include "../Fraction.hpp"

#include <boost/multiprecision/cpp_int.hpp>

#include <boost/math/common_factor.hpp>

using namespace boost::multiprecision;

TEST_CASE("Fraction Constructor with String Input") {
Fraction frac("12/8");
REQUIRE(frac.numerator == 3);
REQUIRE(frac.denominator == 2);
}

TEST_CASE("Fraction Constructor with Integer Inputs") {
Fraction frac(12, 8);
REQUIRE(frac.numerator == 3);
REQUIRE(frac.denominator == 2);
}

TEST_CASE("Fraction Constructor Throws for Zero Denominator") {
REQUIRE_THROWS_AS(Fraction(1, 0), std::invalid_argument);
}

TEST_CASE("Fraction Simplification") {
Fraction frac(100, 25);
REQUIRE(frac.numerator == 4);
REQUIRE(frac.denominator == 1);
}

TEST_CASE("Fraction Multiplication Operation") {
Fraction frac1(1, 2);
Fraction frac2(2, 3);
Fraction result = frac1 * frac2;
REQUIRE(result.numerator == 1);
REQUIRE(result.denominator == 3);
}

TEST_CASE("Fraction Edge Cases") {
// Add tests for negative numbers, large numbers, etc.
}


TEST_CASE("Fraction with Large Negative Numerator") {
  Fraction frac("-9979014795670452/99850089973004049757");
  REQUIRE(frac.numerator == -9979014795670452); // Expected simplified numerator
  int256_t den = int256_t("99850089973004049757");
  REQUIRE(frac.denominator == den ); // Expected simplified denominator
}

TEST_CASE("Fraction with Large Positive Numerator") {
  Fraction frac("9983009098040147/99900039992000799968");
  REQUIRE(frac.numerator == 9983009098040147); // Expected simplified numerator
  int256_t den = int256_t("99900039992000799968");
  REQUIRE(frac.denominator == den); // Expected simplified denominator
}

TEST_CASE("Fraction with Large Negative Denominator") {
  Fraction frac("19956022396120174/-99950009999000049999");
  REQUIRE(frac.numerator == -19956022396120174); // Expected simplified numerator
  int256_t den = int256_t("99950009999000049999");
  REQUIRE(frac.denominator == den); // Expected simplified denominator
}

TEST_CASE("Fraction Close to One") {
  Fraction frac("9987004599390026/99950009999000049999");
  REQUIRE(frac.numerator == 9987004599390026); // Expected simplified numerator
  int256_t den = int256_t("99950009999000049999");
  REQUIRE(frac.denominator == den); // Expected simplified denominator
}