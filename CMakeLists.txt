cmake_minimum_required(VERSION 3.25)
project(MathResearch2)

# --- OpenMP Setup for macOS ---
if(APPLE)
    # If using Homebrew LLVM Clang, prefer using that compiler
    # set(CMAKE_C_COMPILER "/opt/homebrew/opt/llvm/bin/clang")
    # set(CMAKE_CXX_COMPILER "/opt/homebrew/opt/llvm/bin/clang++")

    # Apple Clang needs special flags for OpenMP
    if(CMAKE_C_COMPILER_ID STREQUAL "AppleClang" OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
        set(OpenMP_C_FLAGS "-Xpreprocessor -fopenmp -I/opt/homebrew/opt/libomp/include")
        set(OpenMP_C_LIB_NAMES "omp")
        set(OpenMP_omp_LIBRARY /opt/homebrew/opt/libomp/lib/libomp.dylib)
        set(OpenMP_CXX_FLAGS "-Xpreprocessor -fopenmp -I/opt/homebrew/opt/libomp/include")
        set(OpenMP_CXX_LIB_NAMES "omp")
        set(OpenMP_omp_LIBRARY /opt/homebrew/opt/libomp/lib/libomp.dylib)
        include_directories(/opt/homebrew/opt/libomp/include)
        link_directories(/opt/homebrew/opt/libomp/lib)
    endif()
endif()

# --- Modern Boost & Catch2 Setup ---
if(POLICY CMP0167)
    cmake_policy(SET CMP0167 NEW)
endif()


find_package(Boost REQUIRED)
find_package(Catch2 REQUIRED)
find_package(OpenMP REQUIRED)

set(CMAKE_CXX_STANDARD 20)

#add_subdirectory(third_party/Catch2)
#set(CMAKE_CXX_COMPILER "/opt/homebrew/opt/llvm/bin/clang++")


#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")

# Runner Main executable
add_executable(MathResearch2 runnner_main.cpp Fraction.hpp BloomFilter.hpp Logger.hpp)
target_link_libraries(MathResearch2 PRIVATE Boost::boost)


# Generator Bloom Filter executable
add_executable(MathResearch2_Generator_bf generator_bf_main.cpp Fraction.hpp BloomFilter.hpp Logger.hpp)
target_link_libraries(MathResearch2_Generator_bf PRIVATE Boost::boost)

if(OpenMP_CXX_FOUND)
    target_link_libraries(MathResearch2_Generator_bf PUBLIC OpenMP::OpenMP_CXX)
endif()


# Checker Bloom Filter executable
add_executable(MathResearch2_Checker_bf checker_bf_main.cpp Fraction.hpp BloomFilter.hpp Logger.hpp)
target_link_directories(MathResearch2_Checker_bf PRIVATE Boost::boost)

# Test executable
add_executable(MathResearch2_Tests tests/test1.cpp Fraction.hpp tests/FractionTest.cpp tests/BitsetTest.cpp)
target_link_libraries(MathResearch2_Tests PRIVATE Catch2::Catch2WithMain Boost::boost)

add_executable(sample sample.cpp)

include(Ctest)
enable_testing()
add_test(NAME MathResearch2Test COMMAND MathResearch2_Tests)