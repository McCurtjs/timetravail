#include "game.h"
#include "game/levels.h"

#include "test_behaviors.h"
#include "game/game_behaviors.h"

void level_load_level_3(Game* game) {

  game->camera.pos = v4f(0, 0, 60, 1);
  game->camera.front = v4front;
  game->light_pos = v4f( -20, 40, 100, 1 );

  vec2 player_start = v2f(7, 1);
  vec2 goal_loc = v2f(-15, 46);

  static Line colliders[] = {
    { .a = {.f={  0,  0}}, .b = {.f={ 15,  0}} }, // starting plat
    { .a = {.f={  1, 28}}, .b = {.f={  1, -1}}, .wall = TRUE }, // left wall
    { .a = {.f={ 14, -1}}, .b = {.f={ 14,  2}}, .wall = TRUE }, // right wall
    { .a = {.f={ 14,  2}}, .b = {.f={ 18,  7}}, .wall = TRUE },
    { .a = {.f={ 18,  7}}, .b = {.f={ 20, 18}}, .wall = TRUE },
    { .a = {.f={  0,  3}}, .b = {.f={  3,  3}}, .droppable = TRUE }, // ladder
    { .a = {.f={  0,  6}}, .b = {.f={  3,  6}}, .droppable = TRUE },
    { .a = {.f={  0,  9}}, .b = {.f={  3,  9}}, .droppable = TRUE },
    { .a = {.f={  0, 15}}, .b = {.f={  3, 15}}, .droppable = TRUE }, // high
    { .a = {.f={ 20, 18}}, .b = {.f={ 25, 18}} }, // right-plat

    { .a = {.f={ 24, 17}}, .b = {.f={ 24, 30}}, .wall = TRUE }, // right second wall
    { .a = {.f={ 24, 30}}, .b = {.f={ 20, 34}}, .wall = TRUE },
    { .a = {.f={ 20, 34}}, .b = {.f={ 20, 35}}, .wall = TRUE },

    { .a = {.f={ 12, 27}}, .b = {.f={ -1, 27}}, .wall = TRUE }, // left ceiling
    { .a = {.f={ 13, 29}}, .b = {.f={ 12, 27}}, .wall = TRUE },

    { .a = {.f={ 20, 35}}, .b = {.f={ 34, 35}} }, // right big plat
    { .a = {.f={-20, 29}}, .b = {.f={ 13, 29}} }, // left big plat

    { .a = {.f={ 33, 34}}, .b = {.f={ 33, 48}}, .wall = TRUE }, // top right wall
    { .a = {.f={ 33, 48}}, .b = {.f={-18, 50}}, .wall = TRUE }, // ceiling
    { .a = {.f={-18, 50}}, .b = {.f={-18, 25}}, .wall = TRUE }, // far left wall

    { .a = {.f={-20, 43}}, .b = {.f={-12, 43}} }, // final plat

    { .a = {.f={-12, 43}}, .b = {.f={  5, 35}}, .wall = TRUE }, // ramp
    { .a = {.f={  5, 35}}, .b = {.f={  4, 34}}, .wall = TRUE },
    { .a = {.f={  4, 34}}, .b = {.f={-24, 38}}, .wall = TRUE },
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
