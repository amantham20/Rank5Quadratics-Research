cmake_minimum_required(VERSION 3.25)
project(MathResearch2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
find_package(Boost REQUIRED)
find_package(Threads REQUIRED)

add_library(rank5 INTERFACE)
target_include_directories(rank5 INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(rank5 INTERFACE Boost::boost Threads::Threads)

add_executable(MathResearch2 runnner_main.cpp)
target_link_libraries(MathResearch2 PRIVATE rank5)
add_executable(MathResearch2_Generator_bf generator_bf_main.cpp)
target_link_libraries(MathResearch2_Generator_bf PRIVATE rank5)
# Historical executable name retained for the priority-queue experiment.
add_executable(MathResearch2_Checker_bf checker_bf_main.cpp)
add_executable(sample sample.cpp)

include(CTest)
if(BUILD_TESTING)
  find_package(Catch2 3 REQUIRED)
  add_executable(MathResearch2_Tests tests/FractionTest.cpp tests/BitsetTest.cpp tests/SearchTest.cpp)
  target_link_libraries(MathResearch2_Tests PRIVATE rank5 Catch2::Catch2WithMain)
  add_test(NAME MathResearch2Test COMMAND MathResearch2_Tests)
  set_tests_properties(MathResearch2Test PROPERTIES TIMEOUT 60)
  add_test(NAME CliRegression COMMAND ${CMAKE_COMMAND}
    -DSEARCH=$<TARGET_FILE:MathResearch2_Generator_bf>
    -DRUNNER=$<TARGET_FILE:MathResearch2>
    -DWORK=${CMAKE_CURRENT_BINARY_DIR}/cli-test
    -P ${CMAKE_CURRENT_SOURCE_DIR}/tests/VerifyCli.cmake)
  add_test(NAME SmallSearch COMMAND MathResearch2_Generator_bf --upper 8 --threads 2 --quiet)
  add_test(NAME EmptySearch COMMAND MathResearch2_Generator_bf --upper 2 --quiet)
  add_test(NAME InvalidThreads COMMAND MathResearch2_Generator_bf --threads 0)
  set_tests_properties(InvalidThreads PROPERTIES WILL_FAIL TRUE)
endif()
