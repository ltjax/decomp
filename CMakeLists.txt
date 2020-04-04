cmake_minimum_required (VERSION 3.2.0)
project (decomp)

set(CMAKE_CXX_STANDARD 11)
set(TARGET_NAME ${PROJECT_NAME})

option(${PROJECT_NAME}_USE_CONAN "Use conan" ON)
option(${PROJECT_NAME}_BUILD_TESTS "Build tests" ON)

if (${PROJECT_NAME}_BUILD_TESTS AND NOT ${PROJECT_NAME}_USE_CONAN)
  find_package(Catch2 REQUIRED)
endif()

if (${PROJECT_NAME}_USE_CONAN)
  include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
  conan_basic_setup(TARGETS)
endif()

set(HEADERS
  source/decomp/convex_decomposition.hpp
  source/decomp/triangulation.hpp
  source/decomp/operations.hpp
  source/decomp/output.hpp)

# Build the main library
add_library(${TARGET_NAME}
  ${HEADERS}
  source/decomp/convex_decomposition.cpp
  source/decomp/triangulation.cpp
  source/decomp/operations.cpp
  source/decomp/output.cpp)

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
  set(TEST_NAME test_${PROJECT_NAME})
  add_executable(${TEST_NAME}
    test/test_main.cpp
    test/remapper.cpp
    test/remove_holes.cpp
    test/ear_clipping.cpp
    test/half_edge.cpp
    test/decomposition.cpp
    test/winding.cpp)

  if (${PROJECT_NAME}_USE_CONAN)
    target_link_libraries(${TEST_NAME}
      PUBLIC decomp CONAN_PKG::Catch2)
  else()
    target_link_libraries(${TEST_NAME}
      PUBLIC decomp Catch2::Catch2)
  endif()
  include(CTest)
  enable_testing()
  add_test(NAME test_syntaxe COMMAND test_${PROJECT_NAME})
endif()

add_executable(decomp_demo
  demo/demo.cpp)

target_link_libraries(decomp_demo
  PUBLIC decomp)

install(TARGETS decomp_demo
  RUNTIME DESTINATION bin)