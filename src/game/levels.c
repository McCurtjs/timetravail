#include "levels.h"

#include "game.h"

void level_switch_check(Game* game) {
  if (game->input.triggered.level_reload) {
    level_switch(game, game->level);
    return;
  }

  for (uint i = 0; i < LEVEL_COUNT; ++i) {
    if (game->input.triggered.levels[i]) {
      game->level = i;
      level_switch(game, i);
      return;
    }
  }
}

void level_switch(Game* game, uint level) {
  if (level >= LEVEL_COUNT) return;
  if (!game_levels[level]) return;
  game_cleanup(game);
  game_init(game);
  game_levels[level](game);
}

LoadLevelFn game_levels[LEVEL_COUNT] = {
  level_load_level_1,
  level_load_level_2,
  level_load_level_3,
  level_load_test,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  level_load_editor,
  level_load_editor_test,
  level_load_og_test,
};
