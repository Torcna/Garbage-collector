cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(GC_B)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

file(GLOB_RECURSE SOURCE_FILES source/*.cpp)

file(GLOB_RECURSE HEADER_FILES "${CMAKE_SOURCE_DIR}/include/*.h" "${CMAKE_SOURCE_DIR}/include/*.hpp")

add_executable(GC_B
    main/GC_main.cpp
    ${SOURCE_FILES}
    ${HEADER_FILES}
)

target_include_directories(GC_B PUBLIC
    "${CMAKE_SOURCE_DIR}/include"
)

source_group("Source Files" FILES ${SOURCE_FILES})
source_group("Header Files" FILES ${HEADER_FILES})