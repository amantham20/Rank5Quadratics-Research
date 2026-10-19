#include "SearchCli.hpp"

// Compatibility executable for searching a previously generated input,value log.
int main(int argc, char** argv) {
  try {
    auto options = rank5::parse_options(argc,argv);
    if (options.help) { rank5::usage(std::cout); return 0; }
    if (options.mode != "checker") throw std::invalid_argument("Runner supports --mode checker only");
    if (options.input.empty()) options.input = "../Numbers/NumberAndOut1000.log";
    rank5::require_distinct_paths(options.input, options.output);
    const auto records = rank5::read_records(options.input);
    const auto matches = rank5::find_matches(records,options.threads,
        rank5::progress_reporter(options.quiet,"Check buckets"));
    std::ofstream file;
    if (!options.output.empty()) {
      file.open(options.output);
      if (!file) throw std::runtime_error("Cannot open output: " + options.output);
    }
    auto& output = file.is_open() ? static_cast<std::ostream&>(file) : std::cout;
    rank5::write_matches(output,records,matches,options);
    output.flush();
    if (!output) throw std::runtime_error("Failed writing output");
    return 0;
  } catch (const std::exception& error) {
    std::cerr << "Error: " << error.what() << '\n';
    return 1;
  }
}
