cmake_minimum_required(VERSION 2.8.12)
project(PackageTest CXX)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(decomp_package_test decomp_package_test.cpp)
target_link_libraries(decomp_package_test ${CONAN_LIBS})
