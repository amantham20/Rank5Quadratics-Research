#ifndef RANK5_SEARCH_HPP
#define RANK5_SEARCH_HPP

#include "Fraction.hpp"
#include <algorithm>
#include <atomic>
#include <chrono>
#include <exception>
#include <functional>
#include <numeric>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

namespace rank5 {
struct Record { Fraction input; Fraction value; };
using Match = std::pair<std::size_t, std::size_t>;
using Buckets = std::unordered_map<std::uint64_t, std::vector<std::size_t>>;
using Progress = std::function<void(std::size_t, std::size_t)>;

inline Fraction polynomial(const Fraction& x) {
  // Horner evaluation of x(x-1)(x^3-8x^2+5x+1).
  return x * (x-1) * (((x-8)*x+5)*x+1);
}

inline std::vector<Record> generate(int lower, int upper, const Progress& progress = {}) {
  if (lower < 1 || upper <= lower) throw std::invalid_argument("Require 1 <= lower < upper");
  std::vector<Record> records;
  const auto total = static_cast<std::size_t>(upper-lower);
  const auto interval = std::max<std::size_t>(1, total/100);
  for (int i = lower; i < upper; ++i) {
    for (int j = i; j < upper; ++j) {
      if (std::gcd(i,j) != 1) continue;
      Fraction input(i,j);
      Fraction value = polynomial(input);
      if (value != 0) records.push_back({std::move(input), std::move(value)});
    }
    const auto done = static_cast<std::size_t>(i-lower+1);
    if (progress && (done % interval == 0 || done == total)) progress(done,total);
  }
  return records;
}

inline bool is_square(const BIG_INT& value) {
  if (value < 0) return false;
  const BIG_INT root = sqrt(value);
  return root*root == value;
}

inline bool square_product(const Fraction& a, const Fraction& b) {
  const Fraction product = a*b;
  if (product.numerator < 0) return false;
  const auto possible_last_digit = [](const BIG_INT& n) {
    const unsigned digit = (n % 10).convert_to<unsigned>();
    return digit != 2 && digit != 3 && digit != 7 && digit != 8;
  };
  return possible_last_digit(product.numerator) && possible_last_digit(product.denominator)
      && is_square(product.numerator) && is_square(product.denominator);
}

inline Buckets make_buckets(const std::vector<Record>& records) {
  Buckets buckets;
  for (std::size_t i = 0; i < records.size(); ++i) {
    // Zero times anything is square and cannot use square-class filtering.
    // The polynomial generator excludes zero; reject it in imported datasets.
    if (records[i].value == 0) throw std::invalid_argument("Search values must be nonzero");
    buckets[records[i].value.signature()].push_back(i);
  }
  return buckets;
}

inline std::vector<Match> find_matches(const std::vector<Record>& records,
                                      std::size_t thread_count = 1,
                                      const Progress& progress = {}) {
  if (thread_count == 0) throw std::invalid_argument("Thread count must be positive");
  const auto buckets = make_buckets(records);
  std::vector<const std::vector<std::size_t>*> jobs;
  for (const auto& [key, indices] : buckets) if (indices.size() > 1) jobs.push_back(&indices);
  // Large buckets first to reduce the long tail.
  std::sort(jobs.begin(), jobs.end(), [](const auto* a, const auto* b) { return a->size() > b->size(); });
  if (jobs.empty()) { if (progress) progress(0,0); return {}; }
  thread_count = std::min(thread_count, jobs.size());
  std::atomic<std::size_t> next{0}, completed{0}, finished{0};
  std::vector<std::vector<Match>> local(thread_count);
  std::vector<std::exception_ptr> errors(thread_count);
  std::vector<std::jthread> workers;
  workers.reserve(thread_count);
  for (std::size_t worker = 0; worker < thread_count; ++worker) {
    workers.emplace_back([&, worker] {
      try {
        for (;;) {
          const auto job = next.fetch_add(1);
          if (job >= jobs.size()) break;
          const auto& indices = *jobs[job];
          for (std::size_t a = 0; a < indices.size(); ++a) {
            for (std::size_t b = 0; b < a; ++b) {
              const auto i = indices[b], j = indices[a];
              if (square_product(records[i].value, records[j].value)) local[worker].emplace_back(i,j);
            }
          }
          completed.fetch_add(1);
        }
      } catch (...) { errors[worker] = std::current_exception(); }
      finished.fetch_add(1);
    });
  }
  // Only the caller reports progress; workers never write to shared streams.
  if (progress) {
    while (finished.load() < thread_count) {
      progress(completed.load(), jobs.size());
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  }
  for (auto& worker : workers) worker.join();
  for (const auto& error : errors) if (error) std::rethrow_exception(error);
  if (progress) progress(jobs.size(), jobs.size());
  std::vector<Match> matches;
  for (auto& result : local) matches.insert(matches.end(), result.begin(), result.end());
  std::sort(matches.begin(), matches.end());
  return matches;
}

inline std::vector<Match> brute_force(const std::vector<Record>& records) {
  std::vector<Match> matches;
  for (std::size_t j = 0; j < records.size(); ++j) {
    for (std::size_t i = 0; i < j; ++i) {
      const Fraction product = records[i].value * records[j].value;
      // Independent oracle: no signatures or last-digit rejection.
      if (is_square(product.numerator) && is_square(product.denominator)) matches.emplace_back(i,j);
    }
  }
  std::sort(matches.begin(), matches.end());
  return matches;
}
} // namespace rank5
#endif
