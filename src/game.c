#include "game.h"

#include "./game/game_behaviors.h"

#include "wasm.h"

void game_init(Game* game) {
  game->entities = arr_ety_new();
  game->frame = 0;
}

void game_add_entity(Game* game, const Entity* entity) {
  arr_ety_write_back(game->entities, entity);
}

Entity* game_get_active_player(Game* game) {
  PlayerRef active = { NULL, 0, 0 };

  PlayerRef* array_foreach_index(temp, i, game->timeguys) {
    if (i == 0 || temp->start_frame <= game->frame) {
      active = *temp;
    }
  }

  return active.e;
}

void game_update(Game* game, float dt) {

  Entity* array_foreach(entity, game->entities) {
    if (entity->behavior) {
      entity->behavior(entity, game, dt);
    }
  }

  // reset button triggers (only one frame on trigger)
  for (int i = 0; i < game_button_input_count; ++i) {
    game->input.triggered.buttons[i] = FALSE;
    game->input.released.buttons[i] = FALSE;
  }

  game->input.mouse.move = v2zero;
}

void game_render(Game* game) {
  game->camera.projview = camera_projection_view(&game->camera);

  Entity* array_foreach(entity, game->entities) {
    if (entity->render && !entity->hidden) {
      entity->render(entity, game);
    }
  }

  // render the resulting sprite sheet
  finish_rendering_sprites(game, &game->models.player.sprites, &game->textures.player);
}

void game_cleanup(Game* game) {
  Entity* array_foreach(entity, game->entities) {
    if (entity->delete) {
      entity->delete(entity);
    }
  }

  arr_PlayerRef_delete(&game->timeguys);
  arr_ety_delete(&game->entities);
  game->colliders = NULL;
  game->collider_count = 0;
}
