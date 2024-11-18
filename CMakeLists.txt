cmake_minimum_required(VERSION 3.5)


project(GC)

include_directories (include)
add_executable(GC source/GC_main.cpp include/header.hpp )