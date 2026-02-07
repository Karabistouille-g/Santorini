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
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

# View libraries
add_library(glad lib/glad/glad.c)
add_library(shader src/shader.cpp)
add_library(view src/view.cpp src/mesh.cpp)
# Model libraries
add_library(board src/board.cpp)
add_library(builder src/builder.cpp)
add_library(case src/case.cpp)
add_library(model src/model.cpp)
# Controller libraries
add_library(network_manager src/network_manager.cpp)
add_library(controller src/controller.cpp)

# Link dependencies between libraries
target_link_libraries(model PRIVATE board builder case)
target_link_libraries(view PRIVATE glfw OpenGL::GL shader glad)
target_link_libraries(controller PRIVATE network_manager model view)

add_executable(santorini src/main.cpp)
add_executable(tests
  tests/board_test.cpp
  tests/builder_test.cpp
  tests/case_test.cpp
  tests/model_test.cpp

  lib/googletest/googletest/src/gtest-all.cc
  lib/googletest/googletest/src/gtest_main.cc
)

# Make all targets include necessary directories
get_directory_property(all_targets BUILDSYSTEM_TARGETS)
foreach(target IN ITEMS ${all_targets})
    target_include_directories(${target} PUBLIC
        ${GLFW_INCLUDE_DIRS}
        ${OPENGL_INCLUDE_DIR}
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/lib
    )
endforeach()
# Add googletest include directories for tests
target_include_directories(tests
  PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/googletest/googletest/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/googletest/googletest"
)

target_link_libraries(santorini PRIVATE controller)
target_link_libraries(tests PRIVATE Threads::Threads controller)

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