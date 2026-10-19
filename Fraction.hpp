#ifndef RANK5_FRACTION_HPP
#define RANK5_FRACTION_HPP

#include <array>
#include <bitset>
#include <cstdint>
#include <ostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <boost/multiprecision/cpp_int.hpp>

using BIG_INT = boost::multiprecision::cpp_int;

// Exact, normalized rational arithmetic. No fixed-width intermediate overflow.
struct Fraction {
  BIG_INT numerator;
  BIG_INT denominator;
  static constexpr std::size_t NUMOFBITS = 50;
  static constexpr std::array<unsigned, NUMOFBITS> PRIMES{
      2,3,5,7,11,13,17,19,23,29,31,37,41,43,47,53,59,61,67,71,
      73,79,83,89,97,101,103,107,109,113,127,131,137,139,149,151,
      157,163,167,173,179,181,191,193,197,199,211,223,227,229};

  static BIG_INT gcd(BIG_INT a, BIG_INT b) {
    if (a < 0) a = -a;
    if (b < 0) b = -b;
    while (b != 0) {
      BIG_INT remainder = a % b;
      a = b;
      b = remainder;
    }
    return a;
  }

  Fraction(BIG_INT num, BIG_INT den = 1)
      : numerator(std::move(num)), denominator(std::move(den)) {
    if (denominator == 0) throw std::invalid_argument("Denominator cannot be zero");
    if (denominator < 0) {
      numerator = -numerator;
      denominator = -denominator;
    }
    const BIG_INT divisor = gcd(numerator, denominator);
    numerator /= divisor;
    denominator /= divisor;
  }

  explicit Fraction(const std::string& text) : Fraction(parse(text)) {}

  // Partial square-class fingerprint: equal signatures are necessary, not
  // sufficient, for a nonzero product to be a rational square. Sign and primes
  // above 229 are deliberately left to the exact product check.
  std::uint64_t signature() const {
    if (numerator == 0) return 0;
    BIG_INT num = numerator < 0 ? -numerator : numerator;
    BIG_INT den = denominator;
    std::uint64_t bits = 0;
    for (std::size_t i = 0; i < PRIMES.size(); ++i) {
      bool odd = false;
      while (num % PRIMES[i] == 0) { num /= PRIMES[i]; odd = !odd; }
      while (den % PRIMES[i] == 0) { den /= PRIMES[i]; odd = !odd; }
      if (odd) bits |= std::uint64_t{1} << i;
      if (num == 1 && den == 1) break;
    }
    return bits;
  }

  std::string toBitString() const { return std::bitset<NUMOFBITS>(signature()).to_string(); }
  std::string toString() const { return numerator.str() + "/" + denominator.str(); }

  Fraction operator*(const Fraction& other) const {
    // Cross-cancel before multiplying to keep intermediates small.
    const BIG_INT a = gcd(numerator, other.denominator);
    const BIG_INT b = gcd(other.numerator, denominator);
    return Fraction((numerator/a)*(other.numerator/b),
                    (denominator/b)*(other.denominator/a));
  }
  Fraction operator+(const Fraction& other) const {
    const BIG_INT common = gcd(denominator, other.denominator);
    return Fraction(numerator*(other.denominator/common) + other.numerator*(denominator/common),
                    denominator*(other.denominator/common));
  }
  Fraction operator-(const Fraction& other) const {
    return *this + Fraction(-other.numerator, other.denominator);
  }
  Fraction operator*(int value) const { return *this * Fraction(value); }
  Fraction operator+(int value) const { return *this + Fraction(value); }
  Fraction operator-(int value) const { return *this - Fraction(value); }
  friend Fraction operator*(int value, const Fraction& f) { return f * value; }
  bool operator==(const Fraction& other) const = default;
  bool operator==(int value) const { return denominator == 1 && numerator == value; }
  friend std::ostream& operator<<(std::ostream& out, const Fraction& f) { return out << f.toString(); }

private:
  static BIG_INT decimal(std::string text) {
    const auto first = text.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) throw std::invalid_argument("Empty integer");
    const auto last = text.find_last_not_of(" \t\r\n");
    text = text.substr(first, last-first+1);
    std::size_t i = (text[0] == '-' || text[0] == '+') ? 1 : 0;
    if (i == text.size()) throw std::invalid_argument("Missing integer digits");
    BIG_INT value = 0;
    for (; i < text.size(); ++i) {
      if (text[i] < '0' || text[i] > '9') throw std::invalid_argument("Invalid integer");
      value *= 10;
      value += text[i]-'0';
    }
    return text[0] == '-' ? -value : value;
  }
  static Fraction parse(const std::string& text) {
    const auto slash = text.find('/');
    if (slash == std::string::npos) return Fraction(decimal(text));
    return Fraction(decimal(text.substr(0, slash)), decimal(text.substr(slash+1)));
  }
};
#endif
