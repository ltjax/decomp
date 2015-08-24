cmake_minimum_required (VERSION 3.1.0)
project (decomp)

# Enable C++11 for Clang
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++")
endif()

# Build the main library
add_library(decomp 
  convex_decomposition.cpp
  convex_decomposition.hpp
  triangulation.hpp 
  triangulation.cpp
)

# Only enable tests for top-level projects
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  include(CTest)

  set(TESTS
    remove_holes
    ear_clipping
    half_edge
    decomposition
    winding
  )

  foreach(test ${TESTS})
    add_executable(test_${test} test/${test}.cpp)
    target_link_libraries(test_${test} decomp)
    add_test(NAME ${test} COMMAND test_${test})
  endforeach()
endif()
