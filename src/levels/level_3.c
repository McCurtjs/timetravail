#include "game.h"
#include "game/levels.h"

#include "test_behaviors.h"
#include "game/game_behaviors.h"

void level_load_level_3(Game* game) {

  game->camera.pos = (vec4){0, 0, 60, 1};
  game->camera.front = v4front;
  game->light_pos = (vec4){ -20, 40, 100 };

  vec2 player_start = (vec2){7, 1};
  vec2 goal_loc = (vec2){-15, 46};

  static Line colliders[] = {
    { .a = {  0,  0}, .b = { 15,  0} }, // starting plat
    { .a = {  1, 28}, .b = {  1, -1}, .wall = TRUE }, // left wall
    { .a = { 14, -1}, .b = { 14,  2}, .wall = TRUE }, // right wall
    { .a = { 14,  2}, .b = { 18,  7}, .wall = TRUE },
    { .a = { 18,  7}, .b = { 20, 18}, .wall = TRUE },
    { .a = {  0,  3}, .b = {  3,  3}, .droppable = TRUE }, // ladder
    { .a = {  0,  6}, .b = {  3,  6}, .droppable = TRUE },
    { .a = {  0,  9}, .b = {  3,  9}, .droppable = TRUE },
    { .a = {  0, 15}, .b = {  3, 15}, .droppable = TRUE }, // high
    { .a = { 20, 18}, .b = { 25, 18} }, // right-plat

    { .a = { 24, 17}, .b = { 24, 30}, .wall = TRUE }, // right second wall
    { .a = { 24, 30}, .b = { 20, 34}, .wall = TRUE },
    { .a = { 20, 34}, .b = { 20, 35}, .wall = TRUE },

    { .a = { 12, 27}, .b = { -1, 27}, .wall = TRUE }, // left ceiling
    { .a = { 13, 29}, .b = { 12, 27}, .wall = TRUE },

    { .a = { 20, 35}, .b = { 34, 35} }, // right big plat
    { .a = {-20, 29}, .b = { 13, 29} }, // left big plat

    { .a = { 33, 34}, .b = { 33, 48}, .wall = TRUE }, // top right wall
    { .a = { 33, 48}, .b = {-18, 50}, .wall = TRUE }, // ceiling
    { .a = {-18, 50}, .b = {-18, 25}, .wall = TRUE }, // far left wall

    { .a = {-20, 43}, .b = {-12, 43} }, // final plat

    { .a = {-12, 43}, .b = {  5, 35}, .wall = TRUE }, // ramp
    { .a = {  5, 35}, .b = {  4, 34}, .wall = TRUE },
    { .a = {  4, 34}, .b = {-24, 38}, .wall = TRUE },
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
