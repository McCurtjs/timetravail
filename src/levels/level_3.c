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
    { .a = (vec2){  0,  0}, .b = (vec2){ 15,  0} }, // starting plat
    { .a = (vec2){  1, 28}, .b = (vec2){  1, -1}, .wall = TRUE }, // left wall
    { .a = (vec2){ 14, -1}, .b = (vec2){ 14,  2}, .wall = TRUE }, // right wall
    { .a = (vec2){ 14,  2}, .b = (vec2){ 18,  7}, .wall = TRUE },
    { .a = (vec2){ 18,  7}, .b = (vec2){ 20, 18}, .wall = TRUE },
    { .a = (vec2){  0,  3}, .b = (vec2){  3,  3}, .droppable = TRUE }, // ladder
    { .a = (vec2){  0,  6}, .b = (vec2){  3,  6}, .droppable = TRUE },
    { .a = (vec2){  0,  9}, .b = (vec2){  3,  9}, .droppable = TRUE },
    { .a = (vec2){  0, 15}, .b = (vec2){  3, 15}, .droppable = TRUE }, // high
    { .a = (vec2){ 20, 18}, .b = (vec2){ 25, 18} }, // right-plat

    { .a = (vec2){ 24, 17}, .b = (vec2){ 24, 30}, .wall = TRUE }, // right second wall
    { .a = (vec2){ 24, 30}, .b = (vec2){ 20, 34}, .wall = TRUE },
    { .a = (vec2){ 20, 34}, .b = (vec2){ 20, 35}, .wall = TRUE },

    { .a = (vec2){ 12, 27}, .b = (vec2){ -1, 27}, .wall = TRUE }, // left ceiling
    { .a = (vec2){ 13, 29}, .b = (vec2){ 12, 27}, .wall = TRUE },

    { .a = (vec2){ 20, 35}, .b = (vec2){ 34, 35} }, // right big plat
    { .a = (vec2){-20, 29}, .b = (vec2){ 13, 29} }, // left big plat

    { .a = (vec2){ 33, 34}, .b = (vec2){ 33, 48}, .wall = TRUE }, // top right wall
    { .a = (vec2){ 33, 48}, .b = (vec2){-18, 50}, .wall = TRUE }, // ceiling
    { .a = (vec2){-18, 50}, .b = (vec2){-18, 25}, .wall = TRUE }, // far left wall

    { .a = (vec2){-20, 43}, .b = (vec2){-12, 43} }, // final plat

    { .a = (vec2){-12, 43}, .b = (vec2){  5, 35}, .wall = TRUE }, // ramp
    { .a = (vec2){  5, 35}, .b = (vec2){  4, 34}, .wall = TRUE },
    { .a = (vec2){  4, 34}, .b = (vec2){-24, 38}, .wall = TRUE },
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
