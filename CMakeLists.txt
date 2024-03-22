cmake_minimum_required(VERSION 3.29)
project(Donk)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/debug)

add_subdirectory(lib)

include_directories(./lib/SDL/include)

add_subdirectory(src)
