#ifndef RANK5_SEARCH_CLI_HPP
#define RANK5_SEARCH_CLI_HPP
#include "Search.hpp"
#include <charconv>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <string_view>

namespace rank5 {
struct Options {
  int lower = 1, upper = 1500;
  std::size_t threads = 10;
  std::string mode = "checker", input, output;
  std::string signatures = "bloom_filter.csv";
  bool quiet = false, help = false;
};
inline int positive_int(std::string_view text) {
  int value = 0;
  const auto result = std::from_chars(text.data(), text.data()+text.size(), value);
  if (result.ec != std::errc{} || result.ptr != text.data()+text.size() || value < 1)
    throw std::invalid_argument("Expected a positive integer");
  return value;
}
inline Options parse_options(int argc, char** argv) {
  Options options;
  for (int i = 1; i < argc; ++i) {
    const std::string arg = argv[i];
    if (arg == "--help") { options.help = true; continue; }
    if (arg == "--quiet") { options.quiet = true; continue; }
    if (i+1 == argc) throw std::invalid_argument("Missing value for " + arg);
    const std::string value = argv[++i];
    if (arg == "--lower") options.lower = positive_int(value);
    else if (arg == "--upper") options.upper = positive_int(value);
    else if (arg == "--threads") options.threads = static_cast<std::size_t>(positive_int(value));
    else if (arg == "--mode") options.mode = value;
    else if (arg == "--input") options.input = value;
    else if (arg == "--output") options.output = value;
    else if (arg == "--signatures") options.signatures = value;
    else throw std::invalid_argument("Unknown option: " + arg);
  }
  if (options.upper <= options.lower) throw std::invalid_argument("Require lower < upper");
  if (options.mode != "checker" && options.mode != "brute-force" && options.mode != "generator"
      && options.mode != "filter" && options.mode != "analysis") throw std::invalid_argument("Unknown mode");
  return options;
}
inline void require_distinct_paths(const std::string& input, const std::string& output) {
  if (input.empty() || output.empty()) return;
  const auto a = std::filesystem::weakly_canonical(input);
  const auto b = std::filesystem::weakly_canonical(output);
  if (a == b || (std::filesystem::exists(a) && std::filesystem::exists(b)
                && std::filesystem::equivalent(a,b)))
    throw std::invalid_argument("Input and output paths must differ");
}
inline void usage(std::ostream& out) {
  out << "Options: --lower 1 --upper 1500 (exclusive) --threads 10\n"
      << "         --mode checker|brute-force|generator|filter|analysis\n"
      << "         --input input.log --output results.csv --signatures bloom_filter.csv --quiet\n"
      << "Input format: input_fraction,value_fraction (no header); zero values excluded.\n";
}
inline std::vector<Record> read_records(const std::string& path) {
  std::ifstream file(path);
  if (!file) throw std::runtime_error("Cannot open input: " + path);
  std::vector<Record> records;
  std::unordered_map<std::string, std::size_t> seen;
  std::string line;
  std::size_t line_number = 0;
  while (std::getline(file,line)) {
    ++line_number;
    if (line.find_first_not_of(" \t\r") == std::string::npos) continue;
    try {
      const auto comma = line.find(',');
      if (comma == std::string::npos) throw std::invalid_argument("Missing comma");
      Record record{Fraction(line.substr(0,comma)), Fraction(line.substr(comma+1))};
      const auto key = record.input.toString();
      const auto previous = seen.find(key);
      if (previous != seen.end()) {
        if (records[previous->second].value != record.value) throw std::invalid_argument("Conflicting duplicate input");
        continue;
      }
      if (record.value == 0) continue;
      seen.emplace(key, records.size());
      records.push_back(std::move(record));
    } catch (const std::exception& error) {
      throw std::runtime_error(path + ":" + std::to_string(line_number) + ": " + error.what());
    }
  }
  if (file.bad()) throw std::runtime_error("Failed reading input: " + path);
  return records;
}
inline void write_matches(std::ostream& out, const std::vector<Record>& records,
                          const std::vector<Match>& matches, const Options& options) {
  // Stable record-index order, independent of worker scheduling.
  out << "# polynomial=" << (options.input.empty() ? "x*(x-1)*(x^3-8*x^2+5*x+1)" : "not evaluated (imported values)") << '\n'
      << "# mode=" << options.mode << ",lower=" << options.lower << ",upper_exclusive=" << options.upper
      << ",threads=" << options.threads << '\n';
  out << "# source=" << (options.input.empty() ? "generated" : "imported (values supplied by input)") << '\n';
  out << "input_a,input_b,value_a,value_b,product\n";
  for (const auto& [i,j] : matches) {
    out << records[i].input << ',' << records[j].input << ',' << records[i].value << ','
        << records[j].value << ',' << records[i].value*records[j].value << '\n';
  }
  if (!out) throw std::runtime_error("Failed writing results");
}
inline Progress progress_reporter(bool quiet, std::string phase) {
  if (quiet) return {};
  return [phase=std::move(phase)](std::size_t done, std::size_t total) {
    std::cerr << '\r' << phase << ": " << done << '/' << total << std::flush;
    if (done == total) std::cerr << '\n';
  };
}
} // namespace rank5
#endif
