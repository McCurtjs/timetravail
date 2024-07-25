#include "game.h"
#include "game/levels.h"

#include "test_behaviors.h"
#include "game/game_behaviors.h"

void level_load_level_1(Game* game) {

  game->camera.pos = v4f(0, 0, 60, 1);
  game->camera.front = v4front;
  game->light_pos = v4f(-20, 40, 100, 1);

  vec2 player_start = v2f(-25, 21);
  vec2 goal_loc = v2f( 25, 25);

  static Line colliders[] = {
    { .a = {.f={-35, 20}}, .b = {.f={-15, 20}} }, // starting plat
    { .a = {.f={-15, 20}}, .b = {.f={-13, 14}}, .wall = TRUE }, // left wall
    { .a = {.f={-13, 14}}, .b = {.f={-12,  6}}, .wall = TRUE },
    { .a = {.f={-15,  7}}, .b = {.f={ 25,  7}} }, // ground
    { .a = {.f={ 18,  6}}, .b = {.f={ 18, 20}}, .wall = TRUE }, // right wall
    { .a = {.f={ 18, 20}}, .b = {.f={ 34, 20}} }, // right platform

    // ceiling (right to left)
    { .a = {.f={ 34, 19}}, .b = {.f={ 34, 30}}, .wall = TRUE },
    { .a = {.f={ 34, 30}}, .b = {.f={ 20, 40}}, .wall = TRUE },
    { .a = {.f={ 20, 40}}, .b = {.f={-10, 37}}, .wall = TRUE },
    { .a = {.f={-10, 37}}, .b = {.f={-27, 36}}, .wall = TRUE },
    { .a = {.f={-27, 36}}, .b = {.f={-33, 31}}, .wall = TRUE },
    { .a = {.f={-33, 31}}, .b = {.f={-35, 19}}, .wall = TRUE },
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
    .texture = &game->textures.level,
    .shader = &game->shaders.light,
    .model = &game->models.level_1,
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

  game->timeguys = arr_PlayerRef_new();
  arr_PlayerRef_push_back(game->timeguys, (PlayerRef){
    .start_frame = 0,
    .e = arr_ety_ref_back(game->entities)
  });
}
