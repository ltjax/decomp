cmake_minimum_required(VERSION 3.24)
project(PackageTest CXX)

set(CMAKE_CXX_STANDARD 11)

find_package(decomp CONFIG REQUIRED)

add_executable(decomp_package_test decomp_package_test.cpp)
target_link_libraries(decomp_package_test decomp::decomp)
