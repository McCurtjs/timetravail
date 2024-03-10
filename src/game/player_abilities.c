#include "game_behaviors.h"

void handle_abilities(
  Game* game, PlayerFrameData old_fd, PlayerFrameData* new_fd,
  bool block_warp, bool move_cancel
) {
  uint animation_frame = game->frame - old_fd.start_frame;

  // If we are warping, don't change the animation no matter what
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

  // Cancel out of combat moves on request (for landing or falling off plats)
  if (move_cancel) {
    new_fd->in_combat = FALSE;
  }

  if (new_fd->in_combat && anim_finished(old_fd.animation, animation_frame)) {
    if (new_fd->airborne) {
      new_fd->animation = ANIMATION_FALL_2;
    }

    new_fd->in_combat = FALSE;
  }

  // READY? GO!
  if (!new_fd->in_combat) {

    // there is only one attack button, it is kick
    if (game->input.triggered.kick == TRUE) {
      new_fd->in_combat = TRUE;

      // in-air
      if (new_fd->airborne) {

        // trigger forward-air
        if ((game->input.pressed.left && new_fd->facing == FACING_LEFT)
        ||  (game->input.pressed.right && new_fd->facing == FACING_RIGHT)
        ) {
          new_fd->animation = ANIMATION_F_AIR;

        // trigger back-air, do a turnaround
        } else if ((game->input.pressed.left && new_fd->facing == FACING_RIGHT)
        ||         (game->input.pressed.right && new_fd->facing == FACING_LEFT)
        ) {
          new_fd->animation = ANIMATION_B_AIR;
          new_fd->facing = !new_fd->facing;

        // neutral-air
        } else {
          new_fd->animation = ANIMATION_N_AIR;
        }

      // ground attacks
      } else {
        float ground_speed = v2mag(new_fd->vel);
        float threshold = max_vel[0] - run_anim_threshold_diff;

        if (ground_speed < threshold) {
          new_fd->animation = ANIMATION_KICK;
        } else {
          new_fd->animation = ANIMATION_KICK_RUN;
        }
      }
    }

    // NO KICK, ONLY THROW
    //if (game->input.triggered.throw == TRUE) {
    //
    //}

  // Manage any ongoing combat from the previous frames
  } else if (anim_finished(old_fd.animation, animation_frame)) {
    new_fd->in_combat = FALSE;
  }
}
