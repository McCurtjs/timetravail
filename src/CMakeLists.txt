add_executable(${PROJECT_NAME})
add_library(${PROJECT_NAME}_impl)
add_library(galogen)

#target_precompile_headers(${PROJECT_NAME} PRIVATE def.h)

include_directories(.)
include_directories(./eng)
include_directories(${PROJECT_SOURCE_DIR}/lib/SDL/include)

if (WIN32)
  target_compile_definitions(${PROJECT_NAME} PRIVATE
      _CRT_SECURE_NO_WARNINGS
  )
  target_compile_definitions(${PROJECT_NAME}_impl PRIVATE
      _CRT_SECURE_NO_WARNINGS
  )
endif()

if (MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX /wd4201 /std:clatest)
  target_compile_options(${PROJECT_NAME}_impl PRIVATE /W4 /WX /wd4201 /std:clatest)
else()
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)
  target_compile_options(${PROJECT_NAME}_impl PRIVATE
    -Wall -Wextra -Wpedantic -Werror
    #-Wno-cast-function-type # Galogen does this a lot and I'm not editing those
  )
  set_source_files_properties(eng/native/galogen.c PROPERTIES COMPILE_OPTIONS -Wall)
endif()

target_sources(${PROJECT_NAME}_impl PRIVATE
  game.c
  system_events.c
  test_behaviors.c
  animations.c

  game/combat.c
  game/game_behaviors.c
  game/levels.c
  game/moving_platform.c
  game/player_abilities.c
  game/player_behavior.c
  game/player_movement.c
  game/player_physics.c
  game/time_behavior.c

  levels/level_1.c
  levels/level_2.c
  levels/level_3.c
  levels/level_editor.c
  levels/level_test.c
  levels/og_test.c

  eng/array.c
  eng/str.c
  eng/camera.c
  eng/draw.c
  eng/entity.c
  eng/file.c
  eng/image.c
  eng/mat.c
  eng/model.c
  eng/shader.c
  eng/texture.c
  eng/utility.c
  eng/vec.c
  eng/wasm.c
)

target_sources(galogen PRIVATE
  eng/native/galogen.c
)

target_sources(${PROJECT_NAME} PRIVATE
  main.c
)

target_link_libraries(${PROJECT_NAME} PRIVATE
  libraries
  galogen
  ${PROJECT_NAME}_impl
)
