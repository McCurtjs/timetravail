#include "game.h"
#include "game/levels.h"

#include "test_behaviors.h"
#include "game/game_behaviors.h"

void level_load_level_1(Game* game) {

  game->camera.pos = (vec4){0, 0, 60, 1};
  game->camera.front = v4front;

  // Debug Renderer
  game_add_entity(game, &(Entity) {
    .shader = &game->shaders.basic,
    .model = &game->models.grid,
    .transform = m4identity,
    .render = render_debug,
  });

}
