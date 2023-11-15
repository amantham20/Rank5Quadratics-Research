
#include <iostream>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/algorithm/string.hpp>
#include "Fraction.hpp"
#include <boost/math/common_factor.hpp>
#include <fstream>



using std::string;
using std::vector;
using std::ifstream;
using std::cout;
using std::cerr;
using std::endl;
using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::seconds;


using namespace boost::multiprecision;

bool is_square(const cpp_int& n) {
  if (n < 0) {
    return false; // Negative numbers are not perfect squares
  }

  cpp_int root = sqrt(n);
  return root * root == n;
}

void process_result(const std::string& i, const std::string& j, const Fraction& val_i, const Fraction& val_j) {
  Fraction product = val_i * val_j;

  if (is_square(product.numerator) && is_square(product.denominator)) {
    std::cout << "Square Free" << std::endl;
    std::cout << i << ", " << j << std::endl;
    std::cout << val_i.numerator << "/" << val_i.denominator << ", "
         << val_j.numerator << "/" << val_j.denominator << std::endl;
    std::cout << product.numerator << "/" << product.denominator << std::endl;
    // Log info here
  }
}

void show_progress(size_t current, size_t total, std::chrono::steady_clock::time_point start_time) {
  const int barWidth = 70;
  double progress = static_cast<double >(current) / total;
  int barLength = static_cast<int>(progress * barWidth);

  // Calculate elapsed time
  auto current_time = std::chrono::steady_clock::now();
  auto elapsed_time = std::chrono::duration_cast<std::chrono::seconds>(current_time - start_time).count();



  std::cout << "[";
  for (int i = 0; i < barLength; ++i) {
    std::cout << "=";
  }
  for (int i = barLength; i < barWidth; ++i) {
    std::cout << " ";
  }
  std::cout << "] ";

//  print progress with a precision of 4 decimal places
  std::cout.precision(4);
  std::cout << std::fixed << progress * 100.0 << "%";



  // Display estimated time remaining and time to complete
  if (current > 0) {
    std::cout << " ETA: " << elapsed_time/progress << "s";
    std::cout << " Elapsed: " << elapsed_time << "s" ;
  }

  std::cout << "\r";
  std::cout.flush();
}

vector<string> split(const string& str, const string& delim) {
  vector<string> tokens;
  boost::split(tokens, str, boost::is_any_of(delim));
  return tokens;
}
// 100 = 3002
// 1000 = 303791


int main() {
  string filename = "../Numbers/NumberAndOut1000.log";
  ifstream file(filename);
  string line;

//  size_t total_lines = 3002;
  size_t total_lines = 303791;

  total_lines = total_lines * (total_lines + 1) / 2;

  if (!file.is_open()) {
    cerr << "Error opening file." << endl;
    return 1;
  }

  // Start timing
  std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

  int line_count = 0;

  while (getline(file, line)) {
    vector<string> split_line1 = split(line, ",");

    Fraction frac1(split_line1[1]); // Assuming the fractions are integers

    ifstream file2(filename);
    string line2;
    while (getline(file2, line2)) {
      if (line == line2) {
        break;
      }

      vector<string> split_line2 = split(line2, ",");
      Fraction frac2(split_line2[1]); // Assuming the fractions are integers

      try {
        process_result(split_line1[0], split_line2[0], frac1, frac2);
      } catch (std::invalid_argument& e) {
          std::cout << "Invalid Argument" << std::endl;
          std::cout << split_line1[0] <<  split_line2[0] << frac1.toString() <<  frac2.toString() << std::endl;
      }

    }

    line_count += 1;
    if (line_count % 150 == 0) {
      size_t tempCount = line_count * (line_count + 1) / 2;
      show_progress(tempCount, total_lines, start);
    }

    file2.close();
  }

  // End timing
  std::chrono::steady_clock::time_point stop = std::chrono::steady_clock::now();
  auto duration = duration_cast<seconds>(stop - start);
  cout << duration.count() << " seconds" << endl;

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
