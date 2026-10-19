# Counting Imaginary Quadratic Fields with an Ideal Class Group of 5-rank at least 2

Paper @ [https://arxiv.org/abs/2502.00845](https://arxiv.org/abs/2502.00845)

Co-Authors: Aaron Levin (MSU) and Kollin Bartz (MSU)


<details open><summary>Visitor Note</summary>
Please pardon the naming of my commit history. Past me was quite naive.
</details>

## C++ search

The C++ search evaluates `f(x) = x*(x-1)*(x^3-8*x^2+5*x+1)` at reduced
fractions `x=i/j` with `lower <= i <= j < upper`. Defaults are `lower=1`,
`upper=1500`; zero evaluations are excluded. It searches for distinct inputs
whose evaluated product is a rational square. It does not itself construct a
curve or verify a Jacobian's torsion.

This experimental polynomial and positive input range differ from the paper's
Section 3 Magma search. Use `Magma_Code/Section3Computations.magma` for that
construction and its numerator/denominator bound of 100; C++ counts should not
be presented as reproducing the paper's counts.

### Build and verify

Requires a C++20 compiler, CMake >= 3.25, Boost headers, and Catch2 >= 3 for tests.
On Ubuntu: `sudo apt install cmake g++ libboost-dev catch2`.

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j2
ctest --test-dir build --output-on-failure
```

Use `-DBUILD_TESTING=OFF` to build without Catch2. Threads are linked through
CMake's Threads target; OpenMP is not required.

### Run

```sh
build/MathResearch2_Generator_bf --upper 100 --threads 4 --output results.csv
build/MathResearch2_Generator_bf --upper 20 --mode brute-force --output reference.csv
build/MathResearch2 --input Numbers/NumberAndOut100.log --threads 4 --output imported.csv
```

`--help` lists all flags. `--quiet` disables diagnostics. `--upper` is exclusive;
`--lower`, `--upper`, and `--threads` must be positive, and `lower < upper`.
The log runner accepts `input_fraction,value_fraction` records without a header.
It skips zero values and exact duplicate inputs, rejects conflicting nonzero
duplicates and malformed records, and retains distinct inputs with equal values.
Imported values are used as supplied, without evaluating the polynomial again.

CSV match output is overwritten per run, has `#` metadata lines (mode, bounds,
threads, source), then columns `input_a,input_b,value_a,value_b,product`.
Rows are sorted by input record indices and are stable across thread scheduling.
When comparing different modes or thread counts, ignore their metadata lines.
For imported records, bounds are generation settings only and do not filter the
file. Progress measures generated rows and completed buckets, not a runtime ETA.

### Algorithm and experiments

The fraction type uses arbitrary-precision `cpp_int`, normalization, and
cross-cancellation. Each polynomial value is evaluated once. A numeric 50-bit
key records parity of prime exponents for the first 50 primes (2 through 229).
This is only a partial square-class fingerprint: primes above 229 and signs
are checked by testing the exact reduced product. Negative values are supported.
Pairs are checked once within each bucket using cached values. A fixed worker
pool processes buckets with larger buckets scheduled first; workers do not
write to shared output streams. The exhaustive reference checks every pair
without signature or last-digit filtering. Tests compare both algorithms.

Experimental modes are explicit rather than enabled by editing `main()`:

```sh
build/MathResearch2_Generator_bf --upper 100 --mode generator --output bloom_filter.csv
build/MathResearch2_Generator_bf --upper 100 --mode filter --signatures bloom_filter.csv --output repeated.csv
build/MathResearch2_Generator_bf --upper 100 --mode analysis --output histogram.csv
```

`generator` exports possible repeated 50-character bit signatures through a
Bloom filter sized to the generated record count; `filter` recounts selected
signatures exactly and exports `signature,count`; `analysis` exports
`bucket_size,bucket_count`. Use identical bounds/input for the first two passes.
All modes currently cache generated records in memory. The Bloom mode is an
experiment, not a guarantee of low overall memory usage or a step required by
the default checker. `MathResearch2_Checker_bf` retains the historical name for
the standalone priority-queue demonstration; it is not the search checker.
