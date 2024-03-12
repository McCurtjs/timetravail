#include "game.h"

#include "./game/game_behaviors.h"

#include "wasm.h"

void game_init(Game* game) {
  vector_init(&game->entities, sizeof(Entity));
}

void game_add_entity(Game* game, const Entity* entity) {
  vector_push_back(&game->entities, entity);
}

void game_update(Game* game, float dt) {
  for (uint i = 0 ; i < game->entities.size; ++i) {
    Entity* entity = vector_get(&game->entities, i);

    if (entity->behavior) {
      entity->behavior(entity, game, dt);
    }
  }

  // reset button triggers (only one frame on trigger)
  for (int i = 0; i < game_button_input_count; ++i) {
    game->input.triggered.buttons[i] = FALSE;
    game->input.released.buttons[i] = FALSE;
  }
}

void game_render(Game* game) {
  game->camera.projview = camera_projection_view(&game->camera);

  for (uint i = 0; i < game->entities.size; ++i) {
    Entity* entity = vector_get(&game->entities, i);

    if (entity->render && !entity->hidden) {
      entity->render(entity, game);
    }
  }

  // render the resulting sprite sheet
  finish_rendering_sprites(game, &game->models.player.sprites, &game->textures.player);
}

void game_cleanup(Game* game) {
  for (uint i = 0; i < game->entities.size; ++i) {
    Entity* e = vector_get(&game->entities, i);

    if (e->delete) {
      e->delete(e);
    }
  }

  vector_delete(&game->timeguys);
  vector_delete(&game->entities);
}
