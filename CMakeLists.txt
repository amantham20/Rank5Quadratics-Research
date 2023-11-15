cmake_minimum_required(VERSION 3.25)
project(MathReseach2)

find_package(Boost REQUIRED)
find_package(Catch2 REQUIRED)

set(CMAKE_CXX_STANDARD 20)

add_executable(MathReseach2 main.cpp Fraction.hpp)
target_link_libraries(MathReseach2 PRIVATE Boost::boost)
target_link_libraries(MathReseach2 PRIVATE Catch2::Catch2)

