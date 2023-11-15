
#include <iostream>


int main() {
  std::cout << "Hello, World!" << std::endl;
  return 0;
}


//#include <iostream>
//#include <numeric>
//#include <map>
//#include <set>
//#include <cmath>
//#include <vector>
//#include <thread>
//#include <mutex>
//#include <future>
//#include <chrono>
//#include <ctime>
//#include <fstream>
//
//#include "Fraction.hpp"
//
//const int PRODUCTIONS_UPPERBOUND = 1000;
//const int PRODUCTIONS_LOWERBOUND = 1;
//
//// A function that calculates the result of the given function
//Fraction func(Fraction x) {
////  return x * (x - 1) * (std::pow(x, 3) - 8 * std::pow(x, 2) + 5 * x + 1);
//  return x * (x - 1) * ( (x*x*x) - 8 * x*x + 5 * x + 1);
//}
//
//// The testing function which generates and processes fractions
//void testing(std::unordered_map<Fraction, Fraction>& results) {
//
//
//  auto start = std::chrono::system_clock::now();
//  for (int i = PRODUCTIONS_LOWERBOUND; i < PRODUCTIONS_UPPERBOUND; ++i) {
//    for (int j = i; j < PRODUCTIONS_UPPERBOUND; ++j) {
//      if (std::gcd(i, j) != 1) {
//        continue;
//      }
//
//      Fraction frac = Fraction(i, j);
//      Fraction out = func(frac);
//
//      if (out.numerator == 0) {
//        continue;
//      }
//
//      results[frac] = out;
//    }
//    if ((i) % (PRODUCTIONS_UPPERBOUND / 100) == 0) {
//      std::cout << "Done with " << 1.0/PRODUCTIONS_UPPERBOUND*i*100 << "% iterations ";
//
//      auto end = std::chrono::system_clock::now();
//      std::chrono::duration<double> elapsed_seconds = end - start;
//      std::cout << "Time of Computed: " << elapsed_seconds.count() << "   ";
//      std::cout << "ExpectedTime: " << elapsed_seconds.count() * 100/i << "s\n";
//    }
//  }
//}
//
//
//
//
//bool is_square(long long n) {
//  long long root = static_cast<long long>(std::sqrt(n));
//  return n == root * root;
//}
//
//
//bool is_square_free(long long numerator, long long denominator) {
//  return is_square(numerator) && is_square(denominator);
//}
//
//
//void checkingIfSquareFree(const std::unordered_map<Fraction, Fraction>& results, int max_threads) {
////  std::set<std::pair<Fraction, Fraction>> sameOne;
//  std::set<std::pair<std::string, std::string>> sameOne;
//  std::vector<std::future<void>> futures;
//  std::mutex mutex;
//
//  auto process_pair = [&mutex, &results, &sameOne](const Fraction& i, const Fraction& j) {
//
//      long long numerator_product = i.numerator * j.numerator;
//      long long denominator_product = i.denominator * j.denominator;
//
//      if (is_square_free(numerator_product, denominator_product)) {
//        std::lock_guard<std::mutex> lock(mutex);
//        std::cout << "Square Free Pair Found: " << i.numerator << "/" << i.denominator
//                  << " and " << j.numerator << "/" << j.denominator <<
//                  " with product " << numerator_product << "/" << denominator_product << std::endl;
//      }
//  };
//
//  for (auto i = results.begin(); i != results.end(); ++i) {
//    for (auto j = std::next(i); j != results.end(); ++j) {
//      {
//        std::lock_guard<std::mutex> lock(mutex);
//        auto i_fraction = i->first.toString();
//        auto j_fraction = j->first.toString();
//        auto it  = sameOne.find({i_fraction, j_fraction});
//        if (it != sameOne.end()) {
//          continue;
//        }
//
//        sameOne.insert({i_fraction, j_fraction});
//      }
//
//      // Wait if we have reached the maximum number of threads
//      while (futures.size() >= max_threads) {
//        futures.erase(std::remove_if(futures.begin(), futures.end(),
//                                     [](const std::future<void>& future) { return future.wait_for(std::chrono::seconds(0)) == std::future_status::ready; }),
//                      futures.end());
//        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Sleep to prevent tight loop
//      }
//
//      // Launch a new task to process the pair
//      futures.emplace_back(std::async(std::launch::async, process_pair, i->first, j->first));
//    }
//  }
//
//  // Wait for all tasks to complete
//  for (auto& future : futures) {
//    future.wait();
//  }
//}
//
//
//// This function is not needed in C++ as it is simple to use time functions directly
//// ...other functions would need to be implemented or replaced...
//
//// In C++, we would create a main function to run our program
//int main() {
//  std::unordered_map<Fraction, Fraction> results;
//  std::set<std::pair<Fraction, Fraction>> sameOne;
//
//  // Start time of computation
//  auto start = std::chrono::system_clock::now();
//  std::time_t start_time = std::chrono::system_clock::to_time_t(start);
//
//  // Convert start time to string form
//  char buf[100] = {0};
//  std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&start_time));
//  std::string timeOfComputation(buf);
//
//  // Set up the logging
////  std::ofstream logging;
////  logging.open("output/Numbers_output" + timeOfComputation + ".log");
//
//
//  // Run the testing function
//  testing(results);
//
//  auto end = std::chrono::system_clock::now();
//  std::chrono::duration<double> elapsed_seconds = end - start;
//  std::cout << "Time of generation: " << elapsed_seconds.count() << "s\n";
//  std::cout << "Done with generating the numbers\n";
//
////  print 5 numbers from the results
////  int count = 0;
////
////  for (auto i = results.begin(); i != results.end(); ++i) {
////    if (count < 5) {
////      std::cout << i->first.toString() << " : " << i->second.toString() << std::endl;
////      logging << i->first.toString() << " : " << i->second.toString() << std::endl;
////      count++;
////    }
////  }
//
////  const int max_threads = 10; // for example
////  checkingIfSquareFree(results, max_threads);
//
//  // Final logging and cleanup
////  logging.close();
//
//  return 0;
//}
