cmake_minimum_required(VERSION 3.2)
project(json11 VERSION 1.0.0 LANGUAGES CXX)

enable_testing()

option(JSON11_BUILD_TESTS "Build unit tests" ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INSTALL_PREFIX /usr)

add_library(json11 json11.cpp)
target_include_directories(json11 PUBLIC ${CMAKE_SOURCE_DIR})
target_compile_options(json11
  PRIVATE -fPIC -fno-rtti -fno-exceptions -Wall -Wextra -Werror)
configure_file("json11.pc.in" "json11.pc" @ONLY)

if (JSON11_BUILD_TESTS)
  add_executable(json11_test test.cpp)
  target_link_libraries(json11_test json11)
endif()

install(TARGETS json11 DESTINATION lib/${CMAKE_LIBRARY_ARCHITECTURE})
install(FILES "${CMAKE_SOURCE_DIR}/json11.hpp" DESTINATION include/${CMAKE_LIBRARY_ARCHITECTURE})
install(FILES "${CMAKE_BINARY_DIR}/json11.pc" DESTINATION lib/${CMAKE_LIBRARY_ARCHITECTURE}/pkgconfig)
