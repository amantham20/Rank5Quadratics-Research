//
// Created by Aman Dhruva Thamminana on 11/14/23.
//
#ifndef MATHRESEACH2_FRACTION_HPP
#define MATHRESEACH2_FRACTION_HPP


#include <stdexcept>
//#include <string>
//#include <algorithm>
//#include <numeric>
//#include <cstdint>

#include <boost/multiprecision/cpp_int.hpp>

#include <boost/math/common_factor.hpp>

using namespace boost::multiprecision;


struct Fraction{
  int512_t numerator;
  int512_t denominator;


    std::string removeLeadingSpaces(std::string str) {
      // Find the first character position after excluding leading white space characters
      size_t startPos = str.find_first_not_of(" \t\n\r\f\v");

      // Check whether the string is all whitespaces or empty
      if (startPos == std::string::npos) {
        return "";
      }

      // Erase the leading whitespaces
      str.erase(0, startPos);

      return str;
    }


  Fraction(std::string fraction) {

    size_t slashPos = fraction.find('/');

    // Extracting numerator and denominator
    std::string numeratorStr = fraction.substr(0, slashPos);
    std::string denominatorStr = fraction.substr(slashPos + 1);

    // Removing leading spaces
    numeratorStr = removeLeadingSpaces(numeratorStr);
    denominatorStr = removeLeadingSpaces(denominatorStr);



    // Converting to integers
    int512_t num = int512_t(numeratorStr);
    int512_t den = int512_t(denominatorStr);



    if (den == 0) {
      throw std::invalid_argument("Denominator cannot be zero.");
    }
    int512_t gcd = boost::math::gcd(num, den);
    numerator = num / gcd;
    denominator = den / gcd;

    if (denominator < 0) {
      numerator *= -1;
      denominator *= -1;
    }

  }

  Fraction(int512_t num, int512_t den) : numerator(num), denominator(den)
  {

    if (den == 0) {
      throw std::invalid_argument("Denominator cannot be zero.");
    }
    int512_t gcd = boost::math::gcd(num, den);
    numerator /= gcd;
    denominator /= gcd;
  }

  Fraction operator*(const Fraction& other) const {
    int512_t new_numerator = numerator * other.numerator;
    int512_t new_denominator = denominator * other.denominator;

    return Fraction(new_numerator, new_denominator);
  }


  void operator<<(std::ostream& os) const {
    os << numerator << "/" << denominator;
  }

  std::string toString() const {
    return numerator.str() + "/" + denominator.str();
  }

};


//struct Fraction {
//    __int128_t numerator;
//    __int128_t denominator;
//
//    bool negNum;
//
//    Fraction(__int128_t num, __int128_t den, bool negNum, bool negDen) : numerator(num), denominator(den)
//    {
//
//        if (den == 0) {
//            throw std::invalid_argument("Denominator cannot be zero.");
//        }
//        __int128_t gcd = std::gcd(num, den);
//        numerator /= gcd;
//        denominator /= gcd;
//    }
//
//    Fraction operator*(const Fraction& other) const {
//        __int128_t new_numerator = numerator * other.numerator;
//        __int128_t new_denominator = denominator * other.denominator;
//
//
//
//        return Fraction(new_numerator, new_denominator, negNum, negDen);
//    }
//
//
//
//
//};



//
//struct Fraction {
//    long long numerator;
//    long long denominator;
//
//    Fraction(long long num, long long den) : numerator(num), denominator(den)
//    {
//
//      if (den == 0) {
//        throw std::invalid_argument("Denominator cannot be zero.");
//      }
//      long long gcd = std::gcd(num, den);
//      numerator /= gcd;
//      denominator /= gcd;
//    }
//
//    Fraction() {
//      numerator = 0;
//      denominator = 1;
//    };
//
//    long long getNumerator() const {
//      return numerator;
//    }
//
//    long long getDenominator() const {
//      return denominator;
//    }
//
//    bool operator<(const Fraction& other) const {
//      return numerator * other.denominator < other.numerator * denominator;
//    }
//
//    // Operator overloading for addition
//    Fraction operator+(const Fraction& other) const {
//      long long new_numerator = numerator * other.denominator + other.numerator * denominator;
//      long long new_denominator = denominator * other.denominator;
//      return Fraction(new_numerator, new_denominator);
//    }
//
//    // Operator overloading for subtraction
//    Fraction operator-(const Fraction& other) const {
//      long long new_numerator = numerator * other.denominator - other.numerator * denominator;
//      long long new_denominator = denominator * other.denominator;
//      return Fraction(new_numerator, new_denominator);
//    }
//
//    std::tuple<long long, long long, long long, long long> crossCancel(const Fraction& other) const {
//      long long gcd1 = std::gcd(numerator, other.denominator);
//      long long gcd2 = std::gcd(other.numerator, denominator);
//
//      return std::make_tuple(numerator / gcd1,
//                             denominator / gcd2,
//                             other.numerator / gcd2, other.denominator / gcd1);
//    }
//
//    // Operator overloading for multiplication
//    Fraction operator*(const Fraction& other) const {
//
//      auto [curr_numerator, curr_denominator, other_numerator, other_denominator] = crossCancel(other);
//
//      return Fraction(curr_numerator * other_numerator, curr_denominator * other_denominator);
////      long long new_numerator = numerator * other.numerator;
////      long long new_denominator = denominator * other.denominator;
////      return Fraction(new_numerator, new_denominator);
//    }
//
//    // Operator overloading for division
//    Fraction operator/(const Fraction& other) const {
//      if (other.numerator == 0) {
//        throw std::invalid_argument("Cannot divide by fraction with numerator zero.");
//      }
//      long long new_numerator = numerator * other.denominator;
//      long long new_denominator = denominator * other.numerator;
//      return Fraction(new_numerator, new_denominator);
//    }
//
//    // Operator overloading for multiplication with an long longeger
//    Fraction operator*(long long scalar) const {
//      long long new_numerator = numerator * scalar;
//      // The denominator remains the same since we're only scaling the numerator
//      return Fraction(new_numerator, denominator);
//    }
//
//
//    bool operator<(long long scalar) const {
//      return numerator < scalar * denominator;
//    }
//
//    bool operator>(long long scalar) const {
//      return numerator > scalar * denominator;
//    }
//
//    bool operator<(Fraction other) const {
//      return numerator * other.denominator < other.numerator * denominator;
//    }
//
//    bool operator>(Fraction other) const {
//      return numerator * other.denominator > other.numerator * denominator;
//    }
//
//
//
////    bool operator==(long long scalar) const {
////      return numerator == scalar * denominator;
////    }
////
////    bool operator!=(long long scalar) const {
////      return numerator != scalar * denominator;
////    }
//
//    constexpr inline  std::size_t hash_combine(std::size_t hash1, std::size_t hash2) const noexcept
//    {
//      return hash1 ^ (hash2 + 0x9e3779b9 + (hash1 << 6) + (hash1 >> 2));
//    }
//
//    friend Fraction operator*(long long scalar, const Fraction& fraction) {
//      return fraction * scalar; // Reuse the member operator* for commutative multiplication
//    }
//
////    // Operator overloading for addition with a Fraction
////    Fraction operator+(const Fraction& other) const {
////      long long new_numerator = numerator * other.denominator + other.numerator * denominator;
////      long long new_denominator = denominator * other.denominator;
////      return Fraction(new_numerator, new_denominator);
////    }
//
//    // Operator overloading for addition with an long longeger
//    Fraction operator+(long long scalar) const {
//      long long new_numerator = numerator + scalar * denominator;
//      // The denominator remains the same since we're only adding to the numerator
//      return Fraction(new_numerator, denominator);
//    }
//
//
//
//    // Operator overloading for subtraction with a Fraction
////    Fraction operator-(const Fraction& other) const {
////      long long new_numerator = numerator * other.denominator - other.numerator * denominator;
////      long long new_denominator = denominator * other.denominator;
////      return Fraction(new_numerator, new_denominator);
////    }
//
//    // Operator overloading for subtraction with an long longeger
//    Fraction operator-(long long scalar) const {
//      long long new_numerator = numerator - scalar * denominator;
//      // The denominator remains the same since we're only subtracting from the numerator
//      return Fraction(new_numerator, denominator);
//    }
//
//    // Non-member function overloads for addition and subtraction with an long longeger
//    // These should be friends of the class if they need to access private members directly
//    friend Fraction operator+(long long scalar, const Fraction& fraction) {
//      return fraction + scalar; // Reuse the member operator+ for commutative addition
//    }
//
//    friend Fraction operator-(long long scalar, const Fraction& fraction) {
//      // Since subtraction is not commutative, we need to handle this separately
//      long long new_numerator = scalar * fraction.denominator - fraction.numerator;
//      return Fraction(new_numerator, fraction.denominator);
//    }
//
//    bool operator==(const Fraction& other) const {
//      return numerator * other.denominator == other.numerator * denominator;
//    }
//
//    std::string toString() const {
//      return std::to_string(numerator) + "/" + std::to_string(denominator);
//    }
//
//
//
//};
//
//
//namespace std {
//    template <>
//    struct hash<Fraction> {
//        std::size_t operator()(const Fraction & f) const {
//          std::hash<size_t> hasher;
//          std::size_t h1 = hasher(f.getNumerator());
//          std::size_t h2 = hasher(f.getDenominator());
//          return f.hash_combine(h1, h1);
//        }
//    };
//}


#endif //MATHRESEACH2_FRACTION_HPP
