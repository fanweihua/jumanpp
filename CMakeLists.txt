cmake_minimum_required(VERSION 3.1)
project(jumanpp)

set(CMAKE_MAJOR_VERSION 2)
set(CMAKE_MINOR_VERSION 0)
set(CMAKE_PATCH_VERSION 0)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

execute_process(COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/dependencies.sh)

include_directories("${CMAKE_SOURCE_DIR}/libs")

option(JPP_TEST_REPORT_JUNIT OFF)
option(ENABLE_BENCHMARKS OFF)

enable_testing()

find_package(Threads)

add_subdirectory(libs)
add_subdirectory(src)
add_subdirectory(v2)