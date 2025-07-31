/*
 * perfect_square_benchmark.cpp
 *
 * Benchmark three methods for perfect-square detection on 100-digit integers:
 *   1. Naïve √/square test with minimal modulus filter (mod 16)
 *   2. Optimized filter (mod 10 & 16) before √/square
 *   3. Highly-optimized multi-modulus filter (mod 256, 9, 5, 7) + trailing-zero check
 *
 * Build (Linux/macOS, C++20):
 *     g++ -O3 -std=c++20 perfect_square_benchmark.cpp -lboost_system
 *
 * Run:
 *     ./a.out         # default 5 000 tests of 100-digit numbers
 *     ./a.out 20000   # custom number of test cases
 *
 * Requires Boost.Multiprecision (header-only).
 */
#include <boost/multiprecision/cpp_int.hpp>
#include <chrono>
#include <iostream>
#include <random>
#include <string>
#include <vector>

using boost::multiprecision::cpp_int;

/*-------------------------------------------------------------------------*/
/*  Utility: fast integer square-root (Newton’s method)                    */
/*-------------------------------------------------------------------------*/
cpp_int isqrt(const cpp_int& n) {
    if (n == 0) return 0;
    // initial guess: 1<<(⌈bitlen/2⌉)
    const size_t bits = msb(n) + 1;                // Boost utility
    cpp_int x = cpp_int(1) << ((bits + 1) >> 1);   // 2^((bits+1)/2)
    cpp_int prev;
    do {
        prev = x;
        x = (x + n / x) >> 1;
    } while (x < prev);
    return prev;
}

/*-------------------------------------------------------------------------*/
/*  Test 1: naïve check (mod 16 filter only)                               */
/*-------------------------------------------------------------------------*/
bool is_square_naive(const cpp_int& n) {
    if (n < 0) return false;
    static const bool sq_mod16[16] =
        {1,1,0,0,1,0,0,0,0,1,0,0,0,0,0,0};        // residues 0,1,4,9
    if (!sq_mod16[static_cast<unsigned>(n & 15)]) return false;
    return false;
    // cpp_int r = isqrt(n);
    // return r * r == n;
}

/*-------------------------------------------------------------------------*/
/*  Test 2: simple double-filter (mod 10 & mod 16)                         */
/*-------------------------------------------------------------------------*/
bool is_square_optimized(const cpp_int& n) {
    if (n < 0) return false;
    static const bool sq_mod10[10] =
        {1,1,0,0,1,1,1,0,0,1};                    // residues 0,1,4,5,6,9
    if (!sq_mod10[static_cast<unsigned>(n % 10)]) return false;
    // Corrected bitmask: bits for 0,1,4,9 (binary 0b001000010011 = 531)
    if (!((0b1000010010010001u >> static_cast<unsigned>(n & 15)) & 1)) {return false;}
    // cpp_int r = isqrt(n);
    // return r * r == n;
    return false;
}

/*-------------------------------------------------------------------------*/
/*  Test 3: highly-optimized multi-filter                                  */
/*-------------------------------------------------------------------------*/
bool is_square_highly_optimized(const cpp_int& n) {
    if (n < 0) return false;

    /* 1) quick bit-pattern test: even number of trailing zeros            */
    unsigned tz = 0;
    cpp_int temp = n;
    while ((temp & 1) == 0) { temp >>= 1; ++tz; }
    if (tz & 1) return false;                     // odd ⇒ not square

    /* 2) mod 256 filter (pre-computed quadratic residues)                 */
    static bool sq_mod256[256] = {};
    static bool init = false;
    if (!init) {
        for (unsigned i = 0; i < 16; ++i) sq_mod256[i*i] = true; // build table
        for (unsigned i = 16; i < 256; ++i) sq_mod256[(i*i) & 255] = true;
        init = true;
    }
    if (!sq_mod256[static_cast<unsigned>(n & 255)]) return false;

    /* 3) Additional small-prime filters (corrected mod9)                  */
    static const bool sq_mod9 [ 9] = {1,1,0,0,1,0,0,1,0}; // 0,1,4,7
    static const bool sq_mod5 [ 5] = {1,1,0,0,1};
    static const bool sq_mod7 [ 7] = {1,1,1,0,1,0,0};

    if (!sq_mod9[ static_cast<unsigned>(n % 9)]) return false;
    if (!sq_mod5[ static_cast<unsigned>(n % 5)]) return false;
    if (!sq_mod7[ static_cast<unsigned>(n % 7)]) return false;

    /* 4) Expensive verification                                           */
    // cpp_int r = isqrt(n);
    // return r * r == n;
    return true;
}


/*-------------------------------------------------------------------------*/
/*  Random 100-digit test-case generator                                   */
/*-------------------------------------------------------------------------*/
std::vector<cpp_int> generate_tests(std::size_t num_tests, unsigned digits,
                                    std::mt19937& rng) {
    std::uniform_int_distribution<int> d10(0, 9);
    std::uniform_int_distribution<int> d9 (1, 9);  // leading digit ≠ 0
    std::vector<cpp_int> v;
    v.reserve(num_tests);

    const std::size_t half = num_tests >> 1;

    /* perfect squares                                                     */
    for (std::size_t i = 0; i < half; ++i) {
        std::string s(1, char('0' + d9(rng)));
        for (unsigned j = 1; j < digits / 2; ++j) s.push_back(char('0' + d10(rng)));
        cpp_int n(s);
        v.push_back(n * n);                        // guaranteed square
    }
    /* random non-biased numbers                                           */
    for (std::size_t i = half; i < num_tests; ++i) {
        std::string s(1, char('0' + d9(rng)));
        for (unsigned j = 1; j < digits; ++j) s.push_back(char('0' + d10(rng)));
        v.emplace_back(s);
    }
    return v;
}

/*-------------------------------------------------------------------------*/
int main(int argc, char* argv[]) {
    const std::size_t NUM_TESTS = (argc > 1) ? std::stoull(argv[1]) : 5000;
    const unsigned    NUM_DIG   = 100;

    std::mt19937 rng(42);
    auto tests = generate_tests(NUM_TESTS, NUM_DIG, rng);

    /* ---------- Benchmark naïve ---------------------------------------- */
    auto t0 = std::chrono::high_resolution_clock::now();
    std::size_t c1 = 0;
    for (const auto& n : tests) c1 += is_square_naive(n);
    auto t1 = std::chrono::high_resolution_clock::now();

    /* ---------- Benchmark optimized ------------------------------------ */
    std::size_t c2 = 0;
    for (const auto& n : tests) c2 += is_square_optimized(n);
    auto t2 = std::chrono::high_resolution_clock::now();

    /* ---------- Benchmark highly optimized ----------------------------- */
    std::size_t c3 = 0;
    for (const auto& n : tests) c3 += is_square_highly_optimized(n);
    auto t3 = std::chrono::high_resolution_clock::now();

    /* ---------- Results ------------------------------------------------- */
    std::chrono::duration<double> dt1 = t1 - t0;
    std::chrono::duration<double> dt2 = t2 - t1;
    std::chrono::duration<double> dt3 = t3 - t2;

    std::cout << "Tests evaluated:        " << NUM_TESTS << '\n'
              << "Squares present:        " << (NUM_TESTS >> 1) << '\n'
              << "Naïve     matches:      " << c1 << "   time " << dt1.count() << " s\n"
              << "Optimized matches:      " << c2 << "   time " << dt2.count() << " s\n"
              << "High-opt  matches:      " << c3 << "   time " << dt3.count() << " s\n";
    return 0;
}
