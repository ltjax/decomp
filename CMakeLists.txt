cmake_minimum_required (VERSION 3.1.0)
project (triangulation)
set( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -stdlib=libc++ -std=c++11" )
add_library(triangulation triangulation.hpp triangulation.cpp)

include(CTest)

set(TESTS
  remove_holes
  ear_clipping
)

foreach(test ${TESTS})
	add_executable(test_${test} test/${test}.cpp)
	target_link_libraries(test_${test} triangulation)
	add_test(NAME ${test} COMMAND test_${test})
endforeach()
