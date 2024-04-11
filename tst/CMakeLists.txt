add_executable(${PROJECT_NAME}_tests)

#target_precompile_headers(${PROJECT_NAME} PRIVATE def.h)

include_directories(.)
include_directories(./src/eng)
include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/src/eng)

if (WIN32)
  target_compile_definitions(${PROJECT_NAME}_tests PRIVATE
      _CRT_SECURE_NO_WARNINGS
  )
endif()

if (MSVC)
  target_compile_options(${PROJECT_NAME}_tests PRIVATE /W4 /WX)

  # These link warnings get thrown because of cspec_malloc and friends defined
  # in cspec.c as they get referenced by other objs.
  target_link_options(${PROJECT_NAME}_tests PRIVATE /IGNORE:4217,4286)
else()
  target_compile_options(${PROJECT_NAME}_tests PRIVATE
    -Wall -Wextra -Wpedantic -Werror
    -Wno-cast-function-type # Galogen does this a lot and I'm not editing those
  )
endif()

target_compile_definitions(${PROJECT_NAME}_tests PRIVATE
  "malloc=cspec_malloc"
  "calloc=cspec_calloc"
  "realloc=cspec_realloc"
  "free=cspec_free"
)

target_sources(${PROJECT_NAME}_tests PRIVATE
  cspec.c
  test_main.c

  ./tst/cspec_spec.c

  ../src/eng/wasm.c
  ../src/eng/utility.c
  ../src/eng/array.c

  ../src/eng/str.c
  ./src/eng/str_spec.c
)

#target_link_libraries(${PROJECT_NAME}_tests PRIVATE ${PROJECT_NAME}_impl)
