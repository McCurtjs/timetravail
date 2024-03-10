#include "game_behaviors.h"

void handle_abilities(
  Game* game, PlayerFrameData old_fd, PlayerFrameData* new_fd, bool block_warp
) {
  // if we are warping, don't change the animation no matter what
  if (game->input.triggered.run_replay && !block_warp) {
    new_fd->warp_triggered = TRUE;
  }

  if (new_fd->warp_triggered) {
    if (!anim_is_warp(old_fd.animation)) {
      new_fd->animation = new_fd->airborne ?
        ANIMATION_WARP_AIR : ANIMATION_WARP_STANDING;
    } else {
      new_fd->animation = old_fd.animation;
    }
  }
}
