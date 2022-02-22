cmake_minimum_required(VERSION 3.0)

project(circular_buffer)

add_definitions(-g -W -Wall -Wextra -std=c++17)

#set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(SRCS
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/test_array_circular_buffer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/test_list_circular_buffer.cpp)

add_executable(test_circular_buffer ${SRCS})

set(GTEST_ROOT "/usr/src/googletest/googletest")

find_package(GTest REQUIRED)

include_directories(${GTEST_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(test_circular_buffer ${GTEST_LIBRARIES} pthread)
