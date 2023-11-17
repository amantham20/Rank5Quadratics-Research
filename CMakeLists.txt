cmake_minimum_required(VERSION 3.25)
project(MathResearch2)

find_package(Boost REQUIRED)
find_package(Catch2 REQUIRED)

set(CMAKE_CXX_STANDARD 20)

#add_subdirectory(third_party/Catch2)


# Main executable
add_executable(MathResearch2 main.cpp Fraction.hpp)
target_link_libraries(MathResearch2 PRIVATE Boost::boost)

# Test executable
add_executable(MathResearch2_Tests tests/test1.cpp Fraction.hpp tests/FractionTest.cpp tests/BitsetTest.cpp)
target_link_libraries(MathResearch2_Tests PRIVATE Catch2::Catch2WithMain Boost::boost)

include(Ctest)
enable_testing()
add_test(NAME MathResearch2Test COMMAND MathResearch2_Tests)