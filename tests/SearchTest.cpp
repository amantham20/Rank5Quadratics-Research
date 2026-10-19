#include <catch2/catch_test_macros.hpp>
#include "SearchCli.hpp"
#include <sstream>

TEST_CASE("Bucketed search agrees with exhaustive pair checks") {
  for (int upper : {2,3,8,20,35}) {
    const auto records = rank5::generate(1,upper);
    // Separate, direct polynomial expression validates Horner evaluation too.
    for (const auto& record : records) {
      const auto& x = record.input;
      REQUIRE(record.value == x*(x-1)*(x*x*x-8*(x*x)+5*x+1));
    }
    const auto expected = rank5::brute_force(records);
    REQUIRE(rank5::find_matches(records,1) == expected);
    REQUIRE(rank5::find_matches(records,4) == expected);
  }
}

TEST_CASE("Partial signature collisions are resolved by exact arithmetic") {
  const std::vector<rank5::Record> records{
      {Fraction(1,10),Fraction(233,1)}, {Fraction(2,10),Fraction(239,1)},
      {Fraction(3,10),Fraction(233*4,1)}, {Fraction(4,10),Fraction(-233,1)},
      {Fraction(5,10),Fraction(-233*9,1)}, {Fraction(6,10),Fraction(233,4)}};
  const auto expected = rank5::brute_force(records);
  REQUIRE_FALSE(expected.empty());
  REQUIRE(rank5::find_matches(records,3) == expected);
  REQUIRE_FALSE(rank5::square_product(Fraction(233,1),Fraction(239,1)));
  REQUIRE(rank5::square_product(Fraction(-12,1),Fraction(-75,1)));
  REQUIRE_FALSE(rank5::square_product(Fraction(-12,1),Fraction(75,1)));
}

TEST_CASE("Small and empty bucket sets finish with bounded progress") {
  for (const auto& records : std::vector<std::vector<rank5::Record>>{
      {}, {{Fraction(1,2),Fraction(2,1)}},
      {{Fraction(1,2),Fraction(12,1)}, {Fraction(1,3),Fraction(75,1)}}}) {
    bool called = false;
    rank5::find_matches(records,2,[&](auto done, auto total) {
      called = true;
      REQUIRE(done <= total);
    });
    REQUIRE(called);
  }
  REQUIRE_THROWS_AS(rank5::find_matches({},0),std::invalid_argument);
  REQUIRE_THROWS_AS(rank5::generate(0,10),std::invalid_argument);
  REQUIRE_THROWS_AS(rank5::generate(10,10),std::invalid_argument);
  const std::vector<rank5::Record> zeros{{Fraction(1,2),Fraction(0,1)}};
  REQUIRE_THROWS_AS(rank5::find_matches(zeros),std::invalid_argument);
}

TEST_CASE("Thread scheduling does not change exported results") {
  const auto records = rank5::generate(1,35);
  rank5::Options options;
  options.upper = 35;
  std::ostringstream single, parallel;
  rank5::write_matches(single, records, rank5::find_matches(records,1), options);
  rank5::write_matches(parallel, records, rank5::find_matches(records,4), options);
  REQUIRE(single.str() == parallel.str());
}
