cmake_minimum_required (VERSION 3.2.0)
project (decomp)

set(CMAKE_CXX_STANDARD 11)
set(TARGET_NAME ${PROJECT_NAME})

option(${PROJECT_NAME}_BUILD_TESTS "Build tests" OFF)


set(HEADERS
  source/decomp/convex_decomposition.hpp  
  source/decomp/triangulation.hpp)

# Build the main library
add_library(${TARGET_NAME}
  ${HEADERS}
  source/decomp/convex_decomposition.cpp
  source/decomp/triangulation.cpp)

target_include_directories(${TARGET_NAME}
  INTERFACE source)
  
install(TARGETS ${TARGET_NAME}
  ARCHIVE DESTINATION lib)

install(FILES 
  ${HEADERS}
  DESTINATION
  include/decomp/)

# Only enable tests for top-level projects
if(${PROJECT_NAME}_BUILD_TESTS)
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
