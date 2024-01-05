
#include <boost/multiprecision/cpp_int.hpp>
#include <iostream>
#include <cmath>

#include <unordered_map>
#include <map>
#include <unordered_set>
#include <set>
#include <queue>
#include <omp.h>

#include "Fraction.hpp"
#include "BloomFilter.hpp"
#include "Logger.hpp"

#include <thread>
#include <future>

Fraction func(Fraction x) {
//  return  (2*x-1)*(4*(x*x)-2*x-1);
  return x * (x - 1) * ((x * x * x) - 8 * (x * x) + 5 * x + 1);
}

//def func(x):
//  # return  (2*x-1)*(4*(x*x)-2*x-1)
//  return  x*(x-1)*(x**3-8*x**2+5*x+1)

const int PRODUCTIONS_LOWERBOUND = 1; // Define these bounds
const int PRODUCTIONS_UPPERBOUND = 1500; // Define these bounds


bool is_square(const cpp_int &n) {
  if (n < 0) {
    return false; // Negative numbers are not perfect squares
  }

  cpp_int root = sqrt(n);
  return root * root == n;
}


void process_result(const std::string &i, const std::string &j, const Fraction &val_i, const Fraction &val_j) {
  Fraction product = val_i * val_j;

  auto numMod = product.numerator % 10;
  auto denMod = product.denominator % 10;

  if (numMod == 2 || numMod == 3 || numMod == 7 || numMod == 8) {
    return;
  }

  if (denMod == 2 || denMod == 3 || denMod == 7 || denMod == 8) {
    return;
  }


  if (is_square(product.numerator) && is_square(product.denominator)) {
    std::cout << "Square Free" << std::endl;
    std::cout << i << ", " << j << std::endl;
    std::cout << val_i.numerator << "/" << val_i.denominator << ", "
              << val_j.numerator << "/" << val_j.denominator << std::endl;
    std::cout << product.numerator << "/" << product.denominator << std::endl;
    // Log info here
  }
}


void analysis() {
//  read data from file
  std::unordered_map<std::string, size_t> results;

  std::ifstream file("bloom_filter.csv");
  std::string line;
  while (std::getline(file, line)) {
    size_t firstOne = line.find('1');
    if (firstOne != std::string::npos) {
      line = line.substr(firstOne);
    }
    results[line] = 0;
  }

  std::cout << "Completed Loading Data" << std::endl;


  const auto start_time = std::chrono::high_resolution_clock::now();

//#pragma omp parallel for collapse(2) shared(results)
  for (int i = PRODUCTIONS_LOWERBOUND; i < PRODUCTIONS_UPPERBOUND; ++i) {
    for (int j = i; j < PRODUCTIONS_UPPERBOUND; ++j) {
      Fraction frac(i, j);
      if (boost::math::gcd(i, j) != 1) {
        continue;
      }

      Fraction out = func(frac);
      out.setBitSet();

      std::string out_bit_str = out.toBitString();
      size_t firstOne = out_bit_str.find('1');
      if (firstOne != std::string::npos) {
        out_bit_str = out_bit_str.substr(firstOne);
      }

      if (out == 0) {
        continue;
      }

      if (results.find(out_bit_str) != results.end()) {
        results[out_bit_str]++;
      }

    }

    if (i % static_cast<int>(std::floor(PRODUCTIONS_UPPERBOUND / 100.0)) == 0) {
      std::cout << "\n\n\n";
      std::cout << "Done with " << i << " iterations\n";
      auto curr_time = std::chrono::high_resolution_clock::now();
      auto time_elapsed = std::chrono::duration_cast<std::chrono::seconds>(curr_time - start_time).count();
      std::cout << "Time elapsed: " << time_elapsed << " seconds\n";
      std::cout << "Percent complete " << (i / static_cast<double>(PRODUCTIONS_UPPERBOUND)) * 100 << "%" << std::endl;
      std::cout << "Count: " << results.size() << std::endl;
      std::cout << "Expected Time Remaining: "
                << (time_elapsed / (i / static_cast<double>(PRODUCTIONS_UPPERBOUND))) - time_elapsed << " seconds\n";

      // Get current time as time_point
      auto now = std::chrono::system_clock::now();
      std::time_t now_c = std::chrono::system_clock::to_time_t(now);
      struct tm *parts = std::localtime(&now_c);
      std::cout << std::put_time(parts, "%H:%M:%S") << std::endl;
    }
  }

//  get statistics

// Highest bucket
//  size_t highest_bucket = 0;
//  std::string highest_bucket_str;
  using Bucket = std::pair<size_t, std::string>;

//  count map
  std::map<size_t, int> count_map;

  auto cmp = [](const Bucket &a, const Bucket &b) { return a.first > b.first; };
  std::priority_queue<Bucket, std::vector<Bucket>, decltype(cmp)> top_buckets(cmp);

  double average_bucket_size = 0;

  int num_buckets_size_one = 0;

  double average_bucket_size_gt_one = 0;
  int num_buckets_size_gt_one = 0;


  for (const auto &result: results) {
    if (top_buckets.size() < 20) {
      top_buckets.push(std::make_pair(result.second, result.first));
    } else if (top_buckets.top().first < result.second) {
      top_buckets.pop();
      top_buckets.push(std::make_pair(result.second, result.first));
    }
    count_map[result.second]++;

    average_bucket_size += result.second;

    if (result.second == 1) {
      num_buckets_size_one++;
    }

    if (result.second > 1) {
      average_bucket_size_gt_one += result.second;
      num_buckets_size_gt_one++;
    }

  }

//  std::cout << "Highest bucket: " << highest_bucket << std::endl;
//  std::cout << "Highest bucket string: " << highest_bucket_str << std::endl;

  // Output the top 10 buckets
  std::vector<Bucket> top_10;
  while (!top_buckets.empty()) {
    top_10.push_back(top_buckets.top());
    top_buckets.pop();
  }

  std::cout << "Top 10 buckets: " << std::endl;
  for (int i = top_10.size() - 1; i >= 0; --i) {
    std::cout << "Bucket: " << top_10[i].second << " Count: " << top_10[i].first << std::endl;
  }

  average_bucket_size /= results.size();
  std::cout << "Average bucket size: " << average_bucket_size << std::endl;

  std::cout << "Number of buckets with size == 1: " << num_buckets_size_one << std::endl;

  average_bucket_size_gt_one /= num_buckets_size_gt_one;
  std::cout << "Average bucket size with size > 1: " << average_bucket_size_gt_one << std::endl;

  for (int i = 0; i < 10; ++i) {
    std::cout << "Bucket size: " << i << " Count: " << count_map[i] << std::endl;
  }
}


void process(const std::pair<std::string, std::set<std::string>> &result) {
  auto out_bit_str = result.first;
  for (const auto &frac_i: result.second) {
    for (const auto &frac_j: result.second) {
      if (frac_i == frac_j) {
        break;
      }
      Fraction val_i(frac_i);
      Fraction val_j(frac_j);

      Fraction out_i = func(val_i);
      Fraction out_j = func(val_j);
      process_result(frac_i, frac_j, out_i, out_j);
    }
  }

}


void checker(int bucket_size = 0) {

  std::map<std::string, std::set<std::string>> results;


//  //  2_filter.csv
//  const std::string fileName = "4_filter.csv";
////  const std::string fileName = "5_10_filter.csv";
////  const std::string fileName = "10_INF_filter.csv";
//
//  std::ifstream file(fileName);
//
//  std::string line;
//  while (std::getline(file, line)) {
//    results[line] = std::set < std::string > ();
//  }
//
//
//  std::cout << "Completed Loading Data from " << fileName << std::endl;


  auto start_time = std::chrono::high_resolution_clock::now();
  for (int i = PRODUCTIONS_LOWERBOUND; i < PRODUCTIONS_UPPERBOUND; ++i) {
    for (int j = i; j < PRODUCTIONS_UPPERBOUND; ++j) {
      Fraction frac(i, j);
      if (boost::math::gcd(i, j) != 1) {
        continue;
      }

      Fraction out = func(frac);
      out.setBitSet();

      std::string out_bit_str = out.toBitString();
      size_t firstOne = out_bit_str.find('1');
      if (firstOne != std::string::npos) {
        out_bit_str = out_bit_str.substr(firstOne);
      }

      if (out == 0) {
        continue;
      }

//      if (results.find(out_bit_str) != results.end()) {
        results[out_bit_str].insert(frac.toString());

//        if (results[out_bit_str].size() > 2) {
//          results.erase(out_bit_str);
//        }

//        if (bucket_size > 1 && results[out_bit_str].size() == bucket_size) {
//
//          for (const auto &frac_i: results[out_bit_str]) {
//            for (const auto &frac_j: results[out_bit_str]) {
//              if (frac_i == frac_j) {
//                break;
//              }
//              Fraction val_i(frac_i);
//              Fraction val_j(frac_j);
//
//              Fraction out_i = func(val_i);
//              Fraction out_j = func(val_j);
//              process_result(frac_i, frac_j, out_i, out_j);
//            }
//          }
//
//          results.erase(out_bit_str);
//
//        }
//      }
    }

    if (i % static_cast<int>(std::floor(PRODUCTIONS_UPPERBOUND / 100.0)) == 0) {
      std::cout << "\n\n\n";
      std::cout << "Done with " << i << " iterations\n";
      auto curr_time = std::chrono::high_resolution_clock::now();
      auto time_elapsed = std::chrono::duration_cast<std::chrono::seconds>(curr_time - start_time).count();
      std::cout << "Time elapsed: " << time_elapsed << " seconds\n";
      std::cout << "Percent complete " << (i / static_cast<double>(PRODUCTIONS_UPPERBOUND)) * 100 << "%" << std::endl;
      std::cout << "Count: " << results.size() << std::endl;
      std::cout << "Expected Time Remaining: "
                << (time_elapsed / (i / static_cast<double>(PRODUCTIONS_UPPERBOUND))) - time_elapsed << " seconds\n";

      // Get current time as time_point
      auto now = std::chrono::system_clock::now();
      std::time_t now_c = std::chrono::system_clock::to_time_t(now);
      struct tm *parts = std::localtime(&now_c);
      std::cout << std::put_time(parts, "%H:%M:%S") << std::endl;
    }
  }

//  erase all buckets with size 1
  for (auto it = results.begin(); it != results.end();) {
    if (it->second.size() == 1) {
      it = results.erase(it);
    } else {
      ++it;
    }
  }

// resudial units

  std::cout << "Residual units" << std::endl;
  std::cout << "Count: " << results.size() << std::endl;

  const size_t result_len = results.size();
  const size_t result_percent_unit = result_len / 100;


  std::vector<std::thread> threads;
  const size_t maxThreads = 10; // Maximum number of threads you want to allow.
  size_t threadsComplete = 0;

  for (const auto &result: results) {
    if (threads.size() >= maxThreads) {
      // Wait for one of the existing threads to finish
      int i = 0;
      while (true) {
        if (threads[i].joinable()) {
          threads[i].join();
          threads.erase(threads.begin() + i);
          threads.emplace_back(std::thread(process, result));

          break;
        }
        i = (i + 1) % threads.size();
      }

      threadsComplete++;

      if (threadsComplete % result_percent_unit == 0) {
        std::cout << "Percent complete " << (threadsComplete / static_cast<double>(result_len)) * 100 << "%" << std::endl;
      }
    }
    // Launch a new thread
    threads.emplace_back(std::thread(process, result));
  }

  // Join any remaining threads
  for (auto &th : threads) {
    if (th.joinable()) {
      th.join();
    }
  }

  auto curr_time = std::chrono::high_resolution_clock::now();
  auto time_elapsed = std::chrono::duration_cast<std::chrono::seconds>(curr_time - start_time).count();
  std::cout << "Time elapsed: " << time_elapsed << " seconds\n";

}





void filter() {

  std::unordered_map<std::string, size_t> results;


  std::ifstream file("bloom_filter.csv");
  std::string line;
  while (std::getline(file, line)) {
    size_t firstOne = line.find('1');
    if (firstOne != std::string::npos) {
      line = line.substr(firstOne);
    }
    results[line] = 0;
  }

  std::cout << "Completed Loading Data" << std::endl;

  auto start_time = std::chrono::high_resolution_clock::now();
  for (int i = PRODUCTIONS_LOWERBOUND; i < PRODUCTIONS_UPPERBOUND; ++i) {
    for (int j = i; j < PRODUCTIONS_UPPERBOUND; ++j) {
      Fraction frac(i, j);
      if (boost::math::gcd(i, j) != 1) {
        continue;
      }

      Fraction out = func(frac);
      out.setBitSet();

      std::string out_bit_str = out.toBitString();
      size_t firstOne = out_bit_str.find('1');
      if (firstOne != std::string::npos) {
        out_bit_str = out_bit_str.substr(firstOne);
      }

      if (out == 0) {
        continue;
      }

      if (results.find(out_bit_str) != results.end()) {
        results[out_bit_str]++;
      }

    }

    if (i % static_cast<int>(std::floor(PRODUCTIONS_UPPERBOUND / 100.0)) == 0) {
      std::cout << "\n\n\n";
      std::cout << "Done with " << i << " iterations\n";
      auto curr_time = std::chrono::high_resolution_clock::now();
      auto time_elapsed = std::chrono::duration_cast<std::chrono::seconds>(curr_time - start_time).count();
      std::cout << "Time elapsed: " << time_elapsed << " seconds\n";
      std::cout << "Percent complete " << (i / static_cast<double>(PRODUCTIONS_UPPERBOUND)) * 100 << "%" << std::endl;
      std::cout << "Count: " << results.size() << std::endl;
      std::cout << "Expected Time Remaining: "
                << (time_elapsed / (i / static_cast<double>(PRODUCTIONS_UPPERBOUND))) - time_elapsed << " seconds\n";

      // Get current time as time_point
      auto now = std::chrono::system_clock::now();
      std::time_t now_c = std::chrono::system_clock::to_time_t(now);
      struct tm *parts = std::localtime(&now_c);
      std::cout << std::put_time(parts, "%H:%M:%S") << std::endl;
    }
  }


  CSVLogger logger_2("2_filter.csv");
  CSVLogger logger_3("3_filter.csv");
  CSVLogger logger_4("4_filter.csv");
  CSVLogger logger_5("5_10_filter.csv");
  CSVLogger logger_10("10_INF_filter.csv");
  std::map<size_t, int> count_map;
  for (const auto &result: results) {
    if (result.second == 2) {
      logger_2.log(result.first);
      count_map[2]++;
    } else if (result.second == 3) {
      logger_3.log(result.first);
      count_map[3]++;
    } else if (result.second == 4) {
      logger_4.log(result.first);
      count_map[4]++;
    } else if (result.second >= 5 && result.second <= 10) {
      logger_5.log(result.first);
      count_map[5]++;
    } else if (result.second > 10) {
      logger_10.log(result.first);
      count_map[11]++;
    }

  }
  for (auto item: count_map) {
    std::cout << item.first << " " << item.second << std::endl;
  }


}


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
  std::unordered_set < std::string > results;

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
      } else {
        filter.insert(out.toBitString());
      }

    }

    if (i % static_cast<int>(std::floor(PRODUCTIONS_UPPERBOUND / 100.0)) == 0) {
      std::cout << "\n\n\n";
      std::cout << "Done with " << i << " iterations\n";
      auto curr_time = std::chrono::high_resolution_clock::now();
      auto time_elapsed = std::chrono::duration_cast<std::chrono::seconds>(curr_time - start_time).count();
      std::cout << "Time elapsed: " << time_elapsed << " seconds\n";
      std::cout << "Percent complete " << (i / static_cast<double>(PRODUCTIONS_UPPERBOUND)) * 100 << "%" << std::endl;
      std::cout << "Count: " << results.size() << std::endl;
      std::cout << "Expected Time Remaining: "
                << (time_elapsed / (i / static_cast<double>(PRODUCTIONS_UPPERBOUND))) - time_elapsed << " seconds\n";
    }
  }


  for (const auto &result: results) {
    logger.log(result);
  }
}


int main() {


//  generator();

//  analysis();

//  filter();

  checker();


  return EXIT_SUCCESS;
}