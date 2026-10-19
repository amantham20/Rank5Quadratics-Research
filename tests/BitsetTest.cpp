#include <catch2/catch_test_macros.hpp>
#include "Fraction.hpp"

TEST_CASE("Prime parity includes numerator and denominator") {
  REQUIRE(Fraction("12/8").signature() == 3); // 3/2: odd powers of 2 and 3
  REQUIRE(Fraction("600112800/8748000").signature() == 12); // 7^3/5
  REQUIRE(Fraction(12,1).signature() == Fraction(75,1).signature());
  REQUIRE(Fraction(9,4).signature() == 0);
}

TEST_CASE("Constructor choice and sign do not change the parity signature") {
  for (int num = -20; num <= 20; ++num) {
    for (int den = 1; den <= 20; ++den) {
      const Fraction f(num,den);
      REQUIRE(f.signature() == Fraction(f.toString()).signature());
      REQUIRE(f.signature() == Fraction(-num,den).signature());
    }
  }
}

TEST_CASE("Signatures deliberately collide for primes above the filter") {
  REQUIRE(Fraction(233,1).signature() == Fraction(239,1).signature());
  REQUIRE(Fraction(233,1).toBitString().size() == 50);
}
