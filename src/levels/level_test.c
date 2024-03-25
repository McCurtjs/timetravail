#include "game/levels.h"

#include "camera.h"
#include "game.h"
#include "game/game_behaviors.h"
#include "test_behaviors.h"

void level_load_test(Game* game) {

  game->camera.pos = (vec4){0, 0, 60, 1};
  game->camera.front = v4front;
  game->light_pos = (vec4){ -20, 40, 100 };

  vec2 player_start = {0, 20};
  vec2 goal_loc = {0, 45};

  // Set level geometry

  static Line colliders[] = {
    { .a = {-20, 5},      .b = {-15, 5} }, // Moving platform vertical
    { .a = { 10, 10},     .b = { 14, 10} }, // Moving platform diagonal
    { .a = {-20, 10},     .b = {-15, 10} }, // Moving platform fast

    // ground plane because it's not implicit anymore :v
    { .a = {-100, 0},     .b = {100, 0} },

    { .a = {-4, 3},       .b = {-1, 3} },
    { .a = { 1, 6},       .b = { 4, 6} },
    { .a = { 4.f, 6},     .b = { 10, 6} },
    { .a = { 20.5f, 6},   .b = { 20.5f, 8}, .wall = TRUE },
    { .a = {-15, 7},      .b = {-8, 5} },
    { .a = { 10, 0},      .b = { 17, 7} },
    { .a = { 17, 7},      .b = { 100, 7}, .droppable = TRUE },
    { .a = { -15, 4},     .b = { -15, -0.5f}, .wall = TRUE },
    { .a = { -15, -0.5f}, .b = { -15, 4}, .wall = TRUE },

    { .a = { 30, 30},     .b = { 60, 10} },

    { .a = { 5, 13},      .b = { 10, 13} },
    { .a = { 5, 20},      .b = { 10, 20} },
    { .a = { 15, 25},     .b = { 20, 25} },

    { .a = {-47.1f, 7},   .b = {-40, -0.1f} }, // left slope
    { .a = {-59, 25},     .b = {-43, 1}, .wall = TRUE }, // high slope
    { .a = {-50, 2},      .b = {-30, 2} }, // long floor

    { .a = {-27, 1},      .b = { -23, 1}, .bouncy = TRUE, .wall = TRUE },

    { .a = {-12, 30},     .b = { 12, 30} },   // battlefield base
    { .a = {-2.5f, 37},   .b = { 2.5, 37}, .droppable = TRUE }, // top plat
    { .a = {-10, 33.5f},  .b = { -5, 33.5f}, .droppable = TRUE }, // left plat
    { .a = { 5, 33.5f},   .b = { 10, 33.5f}, .droppable = TRUE }, // right plat
    { .a = {-8, 25},      .b = {-12, 30}, .wall = TRUE }, // left wall
    { .a = { 12, 30},     .b = { 8, 25}, .wall = TRUE }, // right wall
    { .a = { 8, 25},      .b = {-8, 25}, .wall = TRUE }, // bottom
  };

  game->colliders = colliders;
  game->collider_count = sizeof(colliders) / sizeof(Line);

  // Establish game entities

  // Debug Renderer
  //game_add_entity(game, &(Entity) {
  //  .shader = &game->shaders.basic,
  //  .model = &game->models.grid,
  //  .transform = m4identity,
  //  .render = render_debug,
  //});

  //// Camera Controller
  //game_add_entity(&game, &(Entity) {
  //  .behavior = behavior_test_camera,
  //});

  // Entity to draw physics lines
  //game_add_entity(game, &(Entity) {
  //  .behavior = behavior_draw_physics_colliders
  //});

  // Time Controller
  game_add_entity(game, &(Entity) {
    .behavior = behavior_time_controller,
  });

  // Level model
  game_add_entity(game, &(Entity) {
    .type = ENTITY_OTHER,
    .texture = &game->textures.crate,
    .shader = &game->shaders.light,
    .model = &game->models.level_test,
    .pos = (vec3){0, 0, 0},
    .transform = m4identity,
    .render = render_phong,
  });

  // Gear model
  game_add_entity(game, &(Entity) {
    .type = ENTITY_OTHER,
    .texture = &game->textures.brass,
    .shader = &game->shaders.light,
    .model = &game->models.gear,
    .pos = (vec3){0, 40, -50},
    .angle = 5,
    .transform = m4identity,
    .render = render_phong,
    .behavior = behavior_gearspin,
  });

  game_add_entity(game, &(Entity) {
    .type = ENTITY_OTHER,
    .texture = &game->textures.brass,
    .shader = &game->shaders.light,
    .model = &game->models.gear,
    .pos = (vec3){20.8f, 31.5f, -50},
    .angle = -5,
    .transform = m4identity,
    .render = render_phong,
    .behavior = behavior_gearspin,
  });

  // Moving platforms
  game_add_entity(game, &(Entity) { // vertical
    .type = ENTITY_PLATFORM,
    .movement_params = {
      .line = &game->colliders[0],
      .move_once = TRUE,
      .delay = 5,
      .target = {-20, 25},
      .duration = 20,
    },
    .behavior = behavior_moving_platform,
    .delete = delete_moving_platform,
  });

  game_add_entity(game, &(Entity) { // diagonal
    .type = ENTITY_PLATFORM,
    .movement_params = {
      .line = &game->colliders[1],
      .target = {30, 30},
      .duration = 10,
    },
    .behavior = behavior_moving_platform,
    .delete = delete_moving_platform,
  });

  game_add_entity(game, &(Entity) { // fast
    .type = ENTITY_PLATFORM,
    .movement_params = {
      .line = &game->colliders[2],
      .move_once = TRUE,
      .delay = 5,
      .target = {-100, 100},
      .duration = 5,
    },
    .behavior = behavior_moving_platform,
    .delete = delete_moving_platform,
  });

  // Goal
  game_add_entity(game, &(Entity) {
    .type = ENTITY_OTHER,
    .shader = &game->shaders.basic,
    .model = &game->models.color_cube,
    .pos = v23(goal_loc),
    .transform = m4identity,
    .render = render_basic,
    .behavior = behavior_goal,
  });

  // Player
  game_add_entity(game, &(Entity) {
    .type = ENTITY_PLAYER,
    .shader = &game->shaders.light,
    .model = &game->models.player,
    .fd = {
      .pos = player_start,
      .vel = v2zero,
      .airborne = TRUE,
      .has_double = TRUE
    },
    .anim_data = {
      .animations = player_animations,
      .hitboxes = player_hitboxes,
      .anim_count = ANIMATION_COUNT,
    },
    .replay = NULL,
    .replay_temp = NULL,
    .render = render_sprites,
    .behavior = behavior_player,
    .delete = delete_player,
  });

  game->timeguys = array_new(PlayerRef);
  array_push_back(game->timeguys, &(PlayerRef){
    .start_frame = 0,
    .e = array_get_back(game->entities)
  });
}
