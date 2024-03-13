#include "game/levels.h"

#include "camera.h"
#include "game.h"
#include "game/game_behaviors.h"
#include "test_behaviors.h"

void level_load_test(Game* game) {

  game->camera.pos = (vec4){0, 0, 60, 1};
  game->camera.front = v4front;

  // Set level geometry

  static Line colliders[] = {
    { .a = (vec2){-20, 5}, .b = (vec2){-15, 5} }, // Moving platform vertical
    { .a = (vec2){ 10, 10}, .b = (vec2){ 14, 10} }, // Moving platform diagonal
    { .a = (vec2){-20, 10}, .b = (vec2){-15, 10} }, // Moving platform fast

    // ground plane because it's not implicit anymore :v
    { .a = (vec2){-100, 0}, .b = (vec2){100, 0} },

    { .a = (vec2){-4, 3}, .b = (vec2){-1, 3} },
    { .a = (vec2){ 1, 6}, .b = (vec2){ 4, 6} },
    { .a = (vec2){ 4.0, 6}, .b = (vec2){ 10, 6} },
    { .a = (vec2){ 20.5, 6}, .b = (vec2){ 20.5, 8}, .wall = TRUE },
    { .a = (vec2){-15, 7}, .b = (vec2){-8, 5} },
    { .a = (vec2){ 10, 0}, .b = (vec2){ 17, 7} },
    { .a = (vec2){ 17, 7}, .b = (vec2){ 100, 7}, .droppable = TRUE },
    { .a = (vec2){ -15, 4}, .b = (vec2){ -15, -0.5}, .wall = TRUE },
    { .a = (vec2){ -15, -0.5}, .b = (vec2){ -15, 4}, .wall = TRUE },

    { .a = (vec2){ 30, 30}, .b = (vec2){ 60, 10} },

    { .a = (vec2){ 5, 13}, .b = (vec2){ 10, 13} },
    { .a = (vec2){ 5, 20}, .b = (vec2){ 10, 20} },
    { .a = (vec2){ 15, 25}, .b = (vec2){ 20, 25} },

    { .a = (vec2){-47.1, 7}, .b = (vec2){-40, -0.1} }, // left slope
    { .a = (vec2){-59, 25}, .b = (vec2){-43, 1}, .wall = TRUE }, // high slope
    { .a = (vec2){-50, 2}, .b = (vec2){-30, 2} }, // long floor

    { .a = (vec2){-27, 1}, .b = (vec2){ -23, 1}, .bouncy = TRUE, .wall = TRUE },

    { .a = (vec2){-12, 30}, .b = (vec2){ 12, 30} },   // battlefield base
    { .a = (vec2){-2.5, 37}, .b = (vec2){ 2.5, 37}, .droppable = TRUE }, // top plat
    { .a = (vec2){-10, 33.5}, .b = (vec2){ -5, 33.5}, .droppable = TRUE }, // left plat
    { .a = (vec2){ 5, 33.5}, .b = (vec2){ 10, 33.5}, .droppable = TRUE }, // right plat
    { .a = (vec2){-8, 25}, .b = (vec2){-12, 30}, .wall = TRUE }, // left wall
    { .a = (vec2){ 12, 30}, .b = (vec2){ 8, 25}, .wall = TRUE }, // right wall
    { .a = (vec2){ 8, 25}, .b = (vec2){-8, 25}, .wall = TRUE }, // bottom
  };

  game->colliders = colliders;
  game->collider_count = sizeof(colliders) / sizeof(Line);

  // Establish game entities

  // Debug Renderer
  game_add_entity(game, &(Entity) {
    .shader = &game->shaders.basic,
    .model = &game->models.grid,
    .transform = m4identity,
    .render = render_debug,
  });

  //// Camera Controller
  //game_add_entity(&game, &(Entity) {
  //  .behavior = behavior_test_camera,
  //});

  // Entity to draw physics lines
  game_add_entity(game, &(Entity) {
    .behavior = behavior_draw_physics_colliders
  });

  // Time Controller
  game_add_entity(game, &(Entity) {
    .behavior = behavior_time_controller,
  });

  // Moving platforms
  game_add_entity(game, &(Entity) { // vertical
    .type = ENTITY_PLATFORM,
    .movement_params = {
      .line = &game->colliders[0],
      .move_once = TRUE,
      .delay = 5,
      .target = (vec2){-20, 25},
      .duration = 20,
    },
    .behavior = behavior_moving_platform,
    .delete = delete_moving_platform,
  });

  game_add_entity(game, &(Entity) { // diagonal
    .type = ENTITY_PLATFORM,
    .movement_params = {
      .line = &game->colliders[1],
      .target = (vec2){30, 30},
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
      .target = (vec2){-100, 100},
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
    .pos = (vec3){0, 45, 0},
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
      .pos = (vec2){0, 20},
      .vel = v2zero,
      .airborne = TRUE,
      .has_double = TRUE
    },
    .anim_data = {
      .animations = player_animations,
      .hitboxes = player_hitboxes,
      .anim_count = ANIMATION_COUNT,
    },
    .replay = { .data = NULL },
    .replay_temp = { .data = NULL },
    .render = render_sprites,
    .behavior = behavior_player,
    .delete = delete_player,
  });

  vector_init(&game->timeguys, sizeof(PlayerRef));
  vector_push_back(&game->timeguys, &(PlayerRef){
    .start_frame = 0,
    .e = vector_get_back(&game->entities)
  });
}
