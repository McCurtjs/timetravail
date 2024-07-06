#include "game/levels.h"

#include "camera.h"
#include "game.h"
#include "game/game_behaviors.h"
#include "test_behaviors.h"

void level_load_test(Game* game) {

  game->camera.pos = v4f(0, 0, 60, 1);
  game->camera.front = v4front;
  game->light_pos = v4f(-20, 40, 100, 1);

  vec2 player_start = v2f(0, 20);
  vec2 goal_loc = v2f(0, 45);

  // Set level geometry

  static Line colliders[] = {
    { .a = {.f={-20   , 5     }}, .b = {.f={-15    , 5     }} }, // Moving platform vertical
    { .a = {.f={ 10   , 10    }}, .b = {.f={ 14    , 10    }} }, // Moving platform diagonal
    { .a = {.f={-20   , 10    }}, .b = {.f={-15    , 10    }} }, // Moving platform fast

    // ground plane because it's not implicit anymore :v
    { .a = {.f={-100  , 0     }}, .b = {.f={ 100   , 0     }} },

    { .a = {.f={-4    , 3     }}, .b = {.f={-1     , 3     }} },
    { .a = {.f={ 1    , 6     }}, .b = {.f={ 4     , 6     }} },
    { .a = {.f={ 4.f  , 6     }}, .b = {.f={ 10    , 6     }} },
    { .a = {.f={ 20.5f, 6     }}, .b = {.f={ 20.5f , 8     }}, .wall = TRUE },
    { .a = {.f={-15   , 7     }}, .b = {.f={-8     , 5     }} },
    { .a = {.f={ 10   , 0     }}, .b = {.f={ 17    , 7     }} },
    { .a = {.f={ 17   , 7     }}, .b = {.f={ 100   , 7     }}, .droppable = TRUE },
    { .a = {.f={-15   , 4     }}, .b = {.f={-15    ,-0.5f  }}, .wall = TRUE },
    { .a = {.f={-15   ,-0.5f  }}, .b = {.f={-15    , 4     }}, .wall = TRUE },

    { .a = {.f={ 30   , 30    }}, .b = {.f={ 60    , 10    }} },

    { .a = {.f={ 5    , 13    }}, .b = {.f={ 10    , 13    }} },
    { .a = {.f={ 5    , 20    }}, .b = {.f={ 10    , 20    }} },
    { .a = {.f={ 15   , 25    }}, .b = {.f={ 20    , 25    }} },

    { .a = {.f={-47.1f, 7     }}, .b = {.f={-40    ,-0.1f  }} }, // left slope
    { .a = {.f={-59   , 25    }}, .b = {.f={-43    , 1     }}, .wall = TRUE }, // high slope
    { .a = {.f={-50   , 2     }}, .b = {.f={-30    , 2     }} }, // long floor

    { .a = {.f={-27   , 1     }}, .b = {.f={-23    , 1     }}, .bouncy = TRUE, .wall = TRUE },

    { .a = {.f={-12   , 30    }}, .b = {.f={ 12    , 30    }} },   // battlefield base
    { .a = {.f={-2.5f , 37    }}, .b = {.f={ 2.5   , 37    }}, .droppable = TRUE }, // top plat
    { .a = {.f={-10   , 33.5f }}, .b = {.f={-5     , 33.5f }}, .droppable = TRUE }, // left plat
    { .a = {.f={ 5    , 33.5f }}, .b = {.f={ 10    , 33.5f }}, .droppable = TRUE }, // right plat
    { .a = {.f={-8    , 25    }}, .b = {.f={-12    , 30    }}, .wall = TRUE }, // left wall
    { .a = {.f={ 12   , 30    }}, .b = {.f={ 8     , 25    }}, .wall = TRUE }, // right wall
    { .a = {.f={ 8    , 25    }}, .b = {.f={-8     , 25    }}, .wall = TRUE }, // bottom
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
    .pos = v3zero,
    .transform = m4identity,
    .render = render_phong,
  });

  // Gear model
  game_add_entity(game, &(Entity) {
    .type = ENTITY_OTHER,
    .texture = &game->textures.brass,
    .shader = &game->shaders.light,
    .model = &game->models.gear,
    .pos = v3f(0, 40, -50),
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
    .pos = v3f(20.8f, 31.5f, -50),
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
      .target = v2f(-20, 25),
      .duration = 20,
    },
    .behavior = behavior_moving_platform,
    .delete = delete_moving_platform,
  });

  game_add_entity(game, &(Entity) { // diagonal
    .type = ENTITY_PLATFORM,
    .movement_params = {
      .line = &game->colliders[1],
      .target = v2f(30, 30),
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
      .target = v2f(-100, 100),
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

  game->timeguys = arr_pref_new();
  arr_pref_push_back(game->timeguys, (PlayerRef) {
    .start_frame = 0,
      .e = arr_ety_get_back_ref(game->entities)
  });
}
