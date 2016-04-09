project(json11 CXX)

cmake_minimum_required(VERSION 3.2)

enable_testing()

add_library(json11 json11.cpp)
target_include_directories(json11 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_options(json11
  PUBLIC -std=c++11
  PRIVATE -fno-rtti -fno-exceptions -Wall -Wextra -Werror)

add_executable(json11_test test.cpp)
target_link_libraries(json11_test json11)
