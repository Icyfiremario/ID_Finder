cmake_minimum_required(VERSION 4.2)
project(ID_Finder)

set(CMAKE_CXX_STANDARD 20)

find_package(OpenMP REQUIRED)

add_executable(ID_Finder main.cpp)

if (OpenMP_CXX_FOUND)
    target_link_libraries(ID_Finder PUBLIC OpenMP::OpenMP_CXX)
endif ()

target_compile_options(ID_Finder PRIVATE -O3 -mavx2)