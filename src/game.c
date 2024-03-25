#include "game.h"

#include "./game/game_behaviors.h"

#include "wasm.h"

void game_init(Game* game) {
  game->entities = array_new(Entity);
  game->frame = 0;
}

void game_add_entity(Game* game, const Entity* entity) {
  array_push_back(game->entities, entity);
}

Entity* game_get_active_player(Game* game) {
  PlayerRef active = { NULL, 0, 0 }, temp;
  for (uint index = 0; index < game->timeguys->size; ++index) {
    array_read(game->timeguys, index, &temp);

    if (index == 0 || temp.start_frame <= game->frame) {
      active = temp;
    }
  }

  return active.e;
}

void game_update(Game* game, float dt) {
  for (uint i = 0 ; i < game->entities->size; ++i) {
    Entity* entity = array_get(game->entities, i);

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

  for (uint i = 0; i < game->entities->size; ++i) {
    Entity* entity = array_get(game->entities, i);

    if (entity->render && !entity->hidden) {
      entity->render(entity, game);
    }
  }

  // render the resulting sprite sheet
  finish_rendering_sprites(game, &game->models.player.sprites, &game->textures.player);
}

void game_cleanup(Game* game) {
  for (uint i = 0; i < game->entities->size; ++i) {
    Entity* e = array_get(game->entities, i);

    if (e->delete) {
      e->delete(e);
    }
  }

  array_delete(&game->timeguys);
  array_delete(&game->entities);
  game->colliders = NULL;
  game->collider_count = 0;
}
