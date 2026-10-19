#include "SearchCli.hpp"
#include "BloomFilter.hpp"
#include <set>
#include <map>

namespace {
void signature_mode(const rank5::Options& options, const std::vector<rank5::Record>& records,
                    std::ostream& output) {
  if (options.mode == "generator") {
    bloom_parameters parameters;
    parameters.projected_element_count = std::max<std::size_t>(1,records.size());
    parameters.false_positive_probability = 0.000001;
    if (!parameters.compute_optimal_parameters()) throw std::runtime_error("Invalid Bloom parameters");
    bloom_filter bloom(parameters);
    std::set<std::string> repeated;
    for (const auto& record : records) {
      const auto key = record.value.toBitString();
      if (bloom.contains(key)) repeated.insert(key);
      else bloom.insert(key);
    }
    for (const auto& key : repeated) output << key << '\n';
    return;
  }
  const auto buckets = rank5::make_buckets(records);
  if (options.mode == "analysis") {
    std::map<std::size_t,std::size_t> histogram;
    for (const auto& [key,indices] : buckets) ++histogram[indices.size()];
    output << "bucket_size,bucket_count\n";
    for (const auto& [size,count] : histogram) output << size << ',' << count << '\n';
    return;
  }
  // Exact recount after the probabilistic pass removes Bloom false positives.
  std::ifstream file(options.signatures);
  if (!file) throw std::runtime_error("Cannot open signatures: " + options.signatures);
  std::map<std::uint64_t,std::size_t> selected;
  std::string line;
  while (std::getline(file,line)) {
    if (!line.empty() && line.back() == '\r') line.pop_back();
    if (line.empty() || line.size() > Fraction::NUMOFBITS || line.find_first_not_of("01") != std::string::npos)
      throw std::runtime_error("Invalid signature line");
    selected[std::bitset<Fraction::NUMOFBITS>(line).to_ullong()] = 0;
  }
  if (file.bad()) throw std::runtime_error("Failed reading signatures");
  for (auto& [key,count] : selected) {
    const auto it = buckets.find(key);
    if (it != buckets.end()) count = it->second.size();
  }
  output << "signature,count\n";
  for (const auto& [key,count] : selected) if (count > 1)
    output << std::bitset<Fraction::NUMOFBITS>(key).to_string() << ',' << count << '\n';
}
}

int main(int argc, char** argv) {
  try {
    auto options = rank5::parse_options(argc,argv);
    if (options.help) { rank5::usage(std::cout); return 0; }
    rank5::require_distinct_paths(options.input, options.output);
    if (options.mode == "filter") rank5::require_distinct_paths(options.signatures, options.output);
    const auto start = std::chrono::steady_clock::now();
    const auto records = options.input.empty()
        ? rank5::generate(options.lower,options.upper,rank5::progress_reporter(options.quiet,"Generate"))
        : rank5::read_records(options.input);
    std::ofstream file;
    if (!options.output.empty()) {
      file.open(options.output);
      if (!file) throw std::runtime_error("Cannot open output: " + options.output);
    }
    std::ostream& output = file.is_open() ? file : std::cout;
    if (options.mode == "checker" || options.mode == "brute-force") {
      const auto matches = options.mode == "checker"
          ? rank5::find_matches(records,options.threads,rank5::progress_reporter(options.quiet,"Check buckets"))
          : rank5::brute_force(records);
      rank5::write_matches(output,records,matches,options);
      if (!options.quiet) std::cerr << "Rational square products found: " << matches.size() << '\n';
    } else {
      signature_mode(options,records,output);
    }
    output.flush();
    if (!output) throw std::runtime_error("Failed writing output");
    if (!options.quiet) std::cerr << "Records: " << records.size() << "; elapsed seconds: "
        << std::chrono::duration<double>(std::chrono::steady_clock::now()-start).count() << '\n';
    return 0;
  } catch (const std::exception& error) {
    std::cerr << "Error: " << error.what() << '\n';
    return 1;
  }
}
