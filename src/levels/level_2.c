#include "game.h"
#include "game/levels.h"

#include "test_behaviors.h"
#include "game/game_behaviors.h"

void level_load_level_2(Game* game) {

  game->camera.pos = (vec4){0, 0, 60, 1};
  game->camera.front = v4front;
  game->light_pos = (vec4){ -20, 40, 100 };

  vec2 player_start = (vec2){-25, 21};
  vec2 goal_loc = (vec2){ 25, 25};

  static Line colliders[] = {
    { .a = (vec2){-35, 20}, .b = (vec2){-15, 20} }, // starting plat
    { .a = (vec2){-15, 20}, .b = (vec2){-13, 14}, .wall = TRUE }, // left wall
    { .a = (vec2){-13, 14}, .b = (vec2){-12,  6}, .wall = TRUE },
    { .a = (vec2){-15,  7}, .b = (vec2){ 25,  7} }, // ground
    { .a = (vec2){ 18,  6}, .b = (vec2){ 18, 20}, .wall = TRUE }, // right wall
    { .a = (vec2){ 18, 20}, .b = (vec2){ 34, 20} }, // right platform

    // ceiling (right to left)
    { .a = (vec2){ 34, 19}, .b = (vec2){ 34, 30}, .wall = TRUE },
    { .a = (vec2){ 34, 30}, .b = (vec2){ 20, 40}, .wall = TRUE },
    { .a = (vec2){ 20, 40}, .b = (vec2){-10, 37}, .wall = TRUE },
    { .a = (vec2){-10, 37}, .b = (vec2){-27, 36}, .wall = TRUE },
    { .a = (vec2){-27, 36}, .b = (vec2){-33, 31}, .wall = TRUE },
    { .a = (vec2){-33, 31}, .b = (vec2){-35, 19}, .wall = TRUE },
  };

  game->colliders = colliders;
  game->collider_count = sizeof(colliders) / sizeof(Line);

  // Time Controller
  game_add_entity(game, &(Entity) {
    .behavior = behavior_time_controller,
  });

  //* // Debug Renderer
  game_add_entity(game, &(Entity) {
    .shader = &game->shaders.basic,
    .model = &game->models.grid,
    .transform = m4identity,
    .render = render_debug,
    .behavior = behavior_draw_physics_colliders
  });

  /*/ // Level model
  game_add_entity(game, &(Entity) {
    .type = ENTITY_OTHER,
    .texture = &game->textures.crate,
    .shader = &game->shaders.light,
    .model = &game->models.level_test,
    .pos = (vec3){0, 0, 0},
    .transform = m4identity,
    .render = render_phong,
  }); //*/

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
