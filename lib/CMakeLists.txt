set(SDL_TEST_LIBRARY OFF)

set(glew-cmake_BUILD_STATIC ON)
set(glew-cmake_BUILD_SHARED OFF)
set(ONLY_LIBS ON)
#set(PKG_CONFIG_REPRESENTATIVE_TARGET libglew_static)

add_subdirectory(SDL)
#add_subdirectory(MurmurHash3)
#add_subdirectory(glew-2.1.0)
#add_subdirectory(glew-cmake)

add_library(libraries INTERFACE)
target_link_libraries(libraries INTERFACE SDL3::SDL3)
#target_link_libraries(libraries INTERFACE MurmurHash)
#target_link_libraries(libraries INTERFACE libglew_static)
