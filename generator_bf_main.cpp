#include <boost/multiprecision/cpp_int.hpp>
#include <iostream>
#include <cmath>

#include <unordered_map>
#include <unordered_set>

#include "Fraction.hpp"
#include "BloomFilter.hpp"
#include "Logger.hpp"

Fraction func(Fraction x) {
//  return  (2*x-1)*(4*(x*x)-2*x-1);
   return  x*(x-1)*((x*x*x)-8*(x*x)+5*x+1);
}

//def func(x):
//  # return  (2*x-1)*(4*(x*x)-2*x-1)
//  return  x*(x-1)*(x**3-8*x**2+5*x+1)

const int PRODUCTIONS_LOWERBOUND = 1; // Define these bounds
const int PRODUCTIONS_UPPERBOUND = 100; // Define these bounds


void generator() {

  bloom_parameters parameters;

  // How many elements roughly do we expect to insert?
  parameters.projected_element_count = 2'000'000'000;
  parameters.false_positive_probability = 0.000001;

  parameters.compute_optimal_parameters();


  CSVLogger logger("bloom_filter.csv");

  bloom_filter filter(parameters);

//  std::unordered_map<std::string, std::string> results;
  const auto start_time = std::chrono::high_resolution_clock::now();
  std::unordered_set<std::string> results;

  long long int count = 0;

  for (int i = PRODUCTIONS_LOWERBOUND; i < PRODUCTIONS_UPPERBOUND; ++i) {
    for (int j = i; j < PRODUCTIONS_UPPERBOUND; ++j) {
      Fraction frac(i, j);
      if (boost::math::gcd(i, j) != 1) {
        continue;
      }

      Fraction out = func(frac);
      std::string out_str = out.toString();

      out.setBitSet();
      frac.setBitSet();

      std::string frac_str = out.toBitString();


      if (out == 0) {
        continue;
      }

      if (filter.contains(out.toBitString())) {
        results.insert(out.toBitString());
//        logger.log(frac.toString(), out.toString(), out.toBitString());
      }
      else {
        filter.insert(out.toBitString());
      }

//      std::cout << frac.toString() << ", " << out.toString() << std::endl;
    }

    if (i % static_cast<int>(std::floor(PRODUCTIONS_UPPERBOUND / 100.0)) == 0) {
      std::cout << "\n\n\n";
      std::cout << "Done with " << i << " iterations\n";
      auto curr_time = std::chrono::high_resolution_clock::now();
      auto time_elapsed = std::chrono::duration_cast<std::chrono::seconds>(curr_time - start_time).count();
      std::cout << "Time elapsed: " << time_elapsed << " seconds\n";
      std::cout << "Percent complete " << (i / static_cast<double>(PRODUCTIONS_UPPERBOUND)) * 100 << "%" << std::endl;
      std::cout << "Count: " << results.size() << std::endl;
      std::cout << "Expected Time Remaining: " << (time_elapsed / (i / static_cast<double>(PRODUCTIONS_UPPERBOUND))) - time_elapsed << " seconds\n";
    }
  }


  for (const auto& result : results) {
    logger.log(result);
  }
}


int main () {


  generator();

  return EXIT_SUCCESS;
}