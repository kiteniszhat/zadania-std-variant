cmake_minimum_required(VERSION 3.20)
project(variant_examples CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

foreach(nazwa example_01_podstawy example_02_visit example_03_praktyczne example_04_ciekawostki test)
    add_executable(${nazwa} ${nazwa}.cpp)
endforeach()
