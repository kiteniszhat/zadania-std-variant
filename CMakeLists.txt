cmake_minimum_required(VERSION 3.14)
project(std_variant_lab LANGUAGES CXX)

# C++17 - std::variant, std::visit, inline constexpr.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release)
endif()

# GoogleTest: systemowy, a gdy go brak - pobierany (git).
find_package(GTest QUIET)
if(NOT GTest_FOUND)
  message(STATUS "GoogleTest nie znaleziony w systemie - pobieram (FetchContent)")
  include(FetchContent)
  FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.15.2
    GIT_SHALLOW    TRUE
  )
  set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(googletest)
endif()

include(GoogleTest)
enable_testing()

set(discovery_mode "")
if(NOT CMAKE_VERSION VERSION_LESS 3.18)
  set(discovery_mode DISCOVERY_MODE PRE_TEST)
endif()

# Dla zadania N powstaje plik wykonywalny zadanieN = tests/test_zadanieN.cpp + tasks/zadanieN.hpp.
foreach(i RANGE 1 5)
  set(target zadanie${i})
  add_executable(${target} tests/test_zadanie${i}.cpp)
  target_include_directories(${target} PRIVATE tasks)
  target_link_libraries(${target} PRIVATE GTest::gtest_main)
  if(MSVC)
    # 4100: nieużywany parametr - szkielety celowo go mają.
    target_compile_options(${target} PRIVATE /W4 /utf-8 /wd4100)
  else()
    target_compile_options(${target} PRIVATE -Wall -Wextra -Wno-unused-parameter)
  endif()
  gtest_discover_tests(${target}
    TEST_PREFIX "${target}."
    ${discovery_mode}
    PROPERTIES LABELS ${target})
endforeach()
