#include "game_behaviors.h"

#include "wasm.h"

static uint reverse_speed = 1;
static uint reverse_time = 0;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
void behavior_time_controller(Entity* _, Game* game, float dt) {
#pragma clang diagnostic pop
  // Tick frame
  if (!game->reverse_playback) {
    ++game->frame;
  } else if (game->frame > 0) {
    if (game->frame < reverse_speed)
      game->frame = 0;
    else
      game->frame -= reverse_speed;

    if (++reverse_time % 80 == 0) {
      ++reverse_speed;
    }
  } else {
    game->reverse_playback = FALSE;
    reverse_speed = 1;
    reverse_time = 0;
  }

  // Trigger reverse mode
  if (game->input.triggered.run_replay) {
    print("Entering reverse playback");
    game->reverse_playback = !game->reverse_playback;
  }
}
