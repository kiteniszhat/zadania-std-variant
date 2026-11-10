cmake_minimum_required(VERSION 3.14)
project(std_variant_lab LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release)
endif()

find_package(GTest QUIET)

include(GoogleTest)
enable_testing()

set(discovery_mode "")
if(NOT CMAKE_VERSION VERSION_LESS 3.18)
  set(discovery_mode DISCOVERY_MODE PRE_TEST)
endif()

foreach(i RANGE 1 5)
  set(target zadanie${i})
  add_executable(${target} tests/test_zadanie${i}.cpp)
  target_include_directories(${target} PRIVATE tasks)
  target_link_libraries(${target} PRIVATE GTest::gtest_main)
  if(MSVC)
    target_compile_options(${target} PRIVATE /W4 /utf-8 /wd4100)
  else()
    target_compile_options(${target} PRIVATE -Wall -Wextra -Wno-unused-parameter)
  endif()
  gtest_discover_tests(${target}
    TEST_PREFIX "${target}."
    ${discovery_mode}
    PROPERTIES LABELS ${target})
endforeach()
