cmake_minimum_required(VERSION 3.16.3)
project(cpp_ds)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDART_REQUIRED True)

add_subdirectory(lib/gtest)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

enable_testing()

add_executable(runTests test/unordered_set_test.cpp)
target_link_libraries(runTests gtest gtest_main)

add_test(
	NAME runTests
	COMMAND runTests
)
