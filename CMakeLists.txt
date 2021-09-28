cmake_minimum_required(VERSION 3.2.0)

project("OptionParser" CXX)

set(CMAKE_CXX_STANDARD 11)

include_directories(src)

add_subdirectory(lib/googletest)
add_subdirectory(tests)