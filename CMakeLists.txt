cmake_minimum_required(VERSION 3.6)
project(Donk)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/debug)

add_subdirectory(lib)

include_directories(./lib/SDL/include)

add_subdirectory(src)

# If we don't set this, ALL_BUILD will be set as the default startup project,
# which is just one configuration change, but is still annoying to remember
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
