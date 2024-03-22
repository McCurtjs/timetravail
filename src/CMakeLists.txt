add_executable(${PROJECT_NAME})

#target_precompile_headers(${PROJECT_NAME} PRIVATE def.h)

include_directories(.)
include_directories(./eng)

if (WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE
       _CRT_SECURE_NO_WARNINGS
    )
endif()

target_sources(${PROJECT_NAME} PRIVATE
  main.c
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

  eng/native/galogen.c
)

target_link_libraries(${PROJECT_NAME} PRIVATE libraries)
