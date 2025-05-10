cmake_minimum_required(VERSION 3.28)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_COMPILER g++)
project(GarbageCollector)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

file(GLOB_RECURSE SOURCE_FILES source/*.cpp)

file(GLOB_RECURSE HEADER_FILES "${CMAKE_SOURCE_DIR}/include/*.h" "${CMAKE_SOURCE_DIR}/include/*.hpp")

add_executable(GarbageCollector
    main/GC_main.cpp
    ${SOURCE_FILES}
    ${HEADER_FILES}
)
find_package(Threads REQUIRED)
target_link_libraries(GarbageCollector PRIVATE Threads::Threads)
target_include_directories(GarbageCollector PUBLIC
    "${CMAKE_SOURCE_DIR}/include"
)

source_group("Source Files" FILES ${SOURCE_FILES})
source_group("Header Files" FILES ${HEADER_FILES})

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)


add_executable(GC_TESTS
    tests/GC_TESTS.cpp
    ${SOURCE_FILES}
    ${HEADER_FILES}
)
target_link_libraries(GC_TESTS PRIVATE gtest_main Threads::Threads)
target_include_directories(GC_TESTS PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME GC_TESTS COMMAND GC_TESTS)