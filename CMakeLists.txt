cmake_minimum_required (VERSION 3.2.0)
project (decomp)

set(CMAKE_CXX_STANDARD 11)
set(TARGET_NAME ${PROJECT_NAME})

option(${PROJECT_NAME}_USE_CONAN "Use conan" ON)
option(${PROJECT_NAME}_BUILD_TESTS "Build tests" ON)

if (${${PROJECT_NAME}_BUILD_TESTS} AND NOT ${${PROJECT_NAME}_USE_CONAN} )
  message(FATAL_ERROR "Cannot build tests without conan")
endif()

if (${${PROJECT_NAME}_USE_CONAN})
  find_file( Conan_BUILD_INFO conanbuildinfo.cmake ${CMAKE_BINARY_DIR} )
  if( NOT EXISTS ${Conan_BUILD_INFO} )
    execute_process(COMMAND conan install -g cmake ${CMAKE_SOURCE_DIR} )
    find_file( Conan_BUILD_INFO conanbuildinfo.cmake ${CMAKE_BINARY_DIR} )
    if(NOT EXISTS ${Conan_BUILD_INFO})
      MESSAGE( FATAL_ERROR "Unable to build conan info file")
    endif()
  endif()
  include(${Conan_BUILD_INFO})
  message(${Conan_BUILD_INFO})

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
if(${${PROJECT_NAME}_BUILD_TESTS})
  set(TEST_NAME test_${PROJECT_NAME})
  add_executable(${TEST_NAME}
    test/test_main.cpp
    test/remapper.cpp
    test/remove_holes.cpp
    test/ear_clipping.cpp
    test/half_edge.cpp
    test/decomposition.cpp
    test/winding.cpp)

  target_link_libraries(${TEST_NAME}
    PUBLIC decomp CONAN_PKG::Catch2)
endif()
