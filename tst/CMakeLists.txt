add_executable(${PROJECT_NAME}_tests)

#target_precompile_headers(${PROJECT_NAME} PRIVATE def.h)

include_directories(.)
include_directories(./eng)
include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/src/eng)

if (WIN32)
  target_compile_definitions(${PROJECT_NAME}_tests PRIVATE
      _CRT_SECURE_NO_WARNINGS
  )
endif()

target_compile_definitions(${PROJECT_NAME}_tests PRIVATE
  "malloc=malloc_test"
  "calloc=calloc_test"
  "realloc=realloc_test"
  "free=free_test"
)

target_sources(${PROJECT_NAME}_tests PRIVATE
  tst_main.c
  tst.c

  ../src/eng/wasm.c
  ../src/eng/utility.c
  ../src/eng/array.c
  ../src/eng/str.c

  eng/tst_str.c
)

#target_link_libraries(${PROJECT_NAME}_tests PRIVATE ${PROJECT_NAME}_impl)
