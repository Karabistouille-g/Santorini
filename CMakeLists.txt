# How to use:
#   mkdir build
#   cd build
#   cmake ..
#   make
#
cmake_minimum_required(VERSION 3.10)

project(Santorini
  LANGUAGES CXX C
)

find_package(Threads)


add_executable(santorini
  src/board.cpp
  src/builder.cpp
  src/case.cpp
  src/controller.cpp
  src/main.cpp
  src/model.cpp
  src/network_manager.cpp
)

add_executable(tests
  src/board.cpp
  src/builder.cpp
  src/case.cpp
  src/controller.cpp
  src/model.cpp
  src/network_manager.cpp

  tests/board_test.cpp
  tests/builder_test.cpp
  tests/case_test.cpp
  tests/model_test.cpp

  lib/googletest/googletest/src/gtest-all.cc
  lib/googletest/googletest/src/gtest_main.cc
)

target_include_directories(santorini
  PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_include_directories(tests
  PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/googletest/googletest/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/googletest/googletest"
)

target_link_libraries(tests
  PRIVATE
    Threads::Threads
)

target_compile_options(santorini
  PRIVATE
    "-Wall" "-Wextra" "-pedantic" "-g" "-O2"
)

target_compile_options(tests
  PRIVATE
    "-Wall" "-Wextra" "-pedantic" "-g" "-O2"
)

set_target_properties(santorini
  PROPERTIES
    CXX_STANDARD 17
    CXX_EXTENSIONS OFF
)

set_target_properties(tests
  PROPERTIES
    CXX_STANDARD 17
    CXX_EXTENSIONS OFF
)