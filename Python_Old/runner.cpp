#include <iostream>
#include <numeric>
#include <map>
#include <set>
#include <cmath>
#include <vector>
#include <thread>
#include <mutex>
#include <future>
#include <chrono>
#include <ctime>
#include <fstream>

const int PRODUCTIONS_UPPERBOUND = 500;
const int PRODUCTIONS_LOWERBOUND = 1;

// A simple struct to represent fractions
struct Fraction {
    int numerator;
    int denominator;

    Fraction(int num, int den) : numerator(num), denominator(den) {
        if (den == 0) {
            throw std::invalid_argument("Denominator cannot be zero.");
        }
        int gcd = std::gcd(num, den);
        numerator /= gcd;
        denominator /= gcd;
    }

    bool operator<(const Fraction& other) const {
        return numerator * other.denominator < other.numerator * denominator;
    }
};

// A function that calculates the result of the given function
int func(int x) {
    return x * (x - 1) * (std::pow(x, 3) - 8 * std::pow(x, 2) + 5 * x + 1);
}

// The testing function which generates and processes fractions
void testing(std::map<Fraction, int>& results) {
    for (int i = PRODUCTIONS_LOWERBOUND; i < PRODUCTIONS_UPPERBOUND; ++i) {
        for (int j = i; j < PRODUCTIONS_UPPERBOUND; ++j) {
            if (std::gcd(i, j) != 1) {
                continue;
            }

            Fraction frac(i, j);
            int out = func(frac.numerator);

            if (out == 0) {
                continue;
            }

            results[frac] = out;
        }
        if (i % (PRODUCTIONS_UPPERBOUND / 100) == 0) {
            std::cout << "Done with " << i << " iterations\n";
        }
    }
}

bool is_square(int n) {
    int root = static_cast<int>(std::sqrt(n));
    return n == root * root;
}


bool is_square_free(int numerator, int denominator) {
    return is_square(numerator) && is_square(denominator);
}


void checkingIfSquareFree(const std::map<Fraction, int>& results, int max_threads) {
    std::set<std::pair<Fraction, Fraction>> sameOne;
    std::vector<std::future<void>> futures;
    std::mutex mutex;

    auto process_pair = [&mutex, &results, &sameOne](const Fraction& i, const Fraction& j) {
        int numerator_product = results.at(i) * results.at(j);
        int denominator_product = i.denominator * j.denominator;

        if (is_square_free(numerator_product, denominator_product)) {
            std::lock_guard<std::mutex> lock(mutex);
            std::cout << "Square Free Pair Found: " << i.numerator << "/" << i.denominator
                      << " and " << j.numerator << "/" << j.denominator << std::endl;
        }
    };

    for (auto i = results.begin(); i != results.end(); ++i) {
        for (auto j = std::next(i); j != results.end(); ++j) {
            {
                std::lock_guard<std::mutex> lock(mutex);
                if (sameOne.count({i->first, j->first}) > 0) {
                    continue;
                }
                sameOne.insert({i->first, j->first});
            }

            // Wait if we have reached the maximum number of threads
            while (futures.size() >= max_threads) {
                futures.erase(std::remove_if(futures.begin(), futures.end(),
                                             [](const std::future<void>& future) { return future.wait_for(std::chrono::seconds(0)) == std::future_status::ready; }),
                              futures.end());
                std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Sleep to prevent tight loop
            }

            // Launch a new task to process the pair
            futures.emplace_back(std::async(std::launch::async, process_pair, i->first, j->first));
        }
    }

    // Wait for all tasks to complete
    for (auto& future : futures) {
        future.wait();
    }
}


// This function is not needed in C++ as it is simple to use time functions directly
// ...other functions would need to be implemented or replaced...

// In C++, we would create a main function to run our program
int main() {
    std::map<Fraction, int> results;
    std::set<std::pair<Fraction, Fraction>> sameOne;

    // Start time of computation
    auto start = std::chrono::system_clock::now();
    std::time_t start_time = std::chrono::system_clock::to_time_t(start);

    // Convert start time to string form
    char buf[100] = {0};
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&start_time));
    std::string timeOfComputation(buf);

    // Set up the logging
    std::ofstream logging;
    logging.open("output/Numbers_output" + timeOfComputation + ".log");

    // Run the testing function
    testing(results);
    std::cout << "Done with generating the numbers\n";

    // ...checkingIfSquareFree would need to be implemented using C++ features...
    const int max_threads = 10; // for example
    checkingIfSquareFree(results, max_threads);
    // Final logging and cleanup
    logging.close();

    return 0;
}
