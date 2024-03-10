#include "game_behaviors.h"

#include "wasm.h"
#include "draw.h"

bool handle_player_collisions(
  Game* game, PlayerFrameData old_fd, PlayerFrameData* new_fd
) {
  const Line* was_standing = new_fd->standing;
  bool move_cancel = FALSE;

  if (new_fd->standing) {
    // If you're on the platform and it's moving, apply that movement
    if (new_fd->standing && new_fd->standing->moving) {
      vec2 curr = platform_pos_at_frame(new_fd->standing->moving, game->frame);
      vec2 prev = platform_pos_at_frame(new_fd->standing->moving, game->frame - 1);
      new_fd->pos = v2add(new_fd->pos, v2sub(curr, prev));
    }

    vec2 extent = v2sub(new_fd->standing->b, new_fd->standing->a);
    float length = v2mag(extent);
    vec2 entity = v2sub(new_fd->pos, new_fd->standing->a);
    float dot = v2dot(v2norm(extent), entity);

    // Fall off the edge of the platform if you run past its extents
    if (dot < 0 || dot > length) {
      new_fd->standing = NULL;
      new_fd->airborne = TRUE;
      new_fd->animation = ANIMATION_FALL;
      move_cancel = TRUE;
    }

    // Drop through the platform if it's droppable and you press down
    if (game->input.pressed.back
    && !new_fd->in_combat && was_standing->droppable
    ) {
      new_fd->standing = NULL;
      new_fd->airborne = TRUE;
      new_fd->animation = ANIMATION_FALL;
      new_fd->vel.y -= 10;
    }
  }

  float closest = 99999;
  vec2 closest_p;
  vec2 closest_n;
  Line* closest_line = NULL;

  for (uint i = 0; i < game->collider_count; ++i) {

    Line line = game->colliders[i];
    vec2 p;

    if (v2seg_seg(old_fd.pos, new_fd->pos, line.a, line.b, &p)) {

      if (was_standing == &game->colliders[i]) {
        continue;
      }

      vec2 dir = v2sub(new_fd->pos, old_fd.pos);
      vec2 n = v2perp(v2norm(v2sub(line.b, line.a)));

      if (v2dot(dir, n) < 0 && !(line.droppable && game->input.pressed.back)) {

        float dist = v2dist(new_fd->pos, p);
        if (closest_line == NULL || dist < closest) {
          closest = dist;
          closest_p = p;
          closest_n = n;
          closest_line = &game->colliders[i];
        }
      }
    }
  }

  if (closest_line) {
    if (!closest_line->wall && !closest_line->bouncy) {
      new_fd->standing = closest_line;
      new_fd->airborne = FALSE;
      new_fd->has_double = TRUE;
      move_cancel = TRUE;
    }

    new_fd->pos = v2add(closest_p, v2scale(closest_n, 0.01));

    vec2 v = v2sub(closest_line->b, closest_line->a);
    vec2 v_unit = v2norm(v);

    if (closest_line->bouncy) {
      new_fd->vel = v2reflect(new_fd->vel, v_unit);
    } else {
      new_fd->vel = v2scale(v_unit, v2dot(v_unit, new_fd->vel));

      // if we're fully grounded and bump into a wall, play a little animation
      if (!old_fd.airborne && !new_fd->airborne && closest_line->wall
      && new_fd->animation != ANIMATION_JUMP
      ) {
        new_fd->animation = ANIMATION_BUMP_INTO_WALL;
      }
    }
  }

  if (new_fd->pos.y <= 0) {
    if (new_fd->airborne) move_cancel = TRUE;
    new_fd->pos.y = 0;
    new_fd->vel.y = 0;
    new_fd->airborne = FALSE;
    new_fd->has_double = TRUE;
  }

  float player_speed = v2mag(new_fd->vel);

  // Handle special animations for landing
  if (old_fd.airborne && !new_fd->airborne) {
    move_cancel = TRUE;

    // if we're at running speed and in hangtime-2, land into a sick roll
    if (player_speed > max_vel[1] - roll_anim_threshold_diff
    &&  anim_is_hangtime_2(new_fd->animation)
    ) {
      new_fd->animation = ANIMATION_ROLL_INTO_RUN;

    // if we're at a walking or idle pace, regular land
    } else if (player_speed < max_vel[0] - run_anim_threshold_diff
    || !(game->input.pressed.left || game->input.pressed.right)
    ) {
      new_fd->animation = ANIMATION_LAND;
    }

    // (if we're at running speed in hangtime-1, just go straight to running)
  }

  return move_cancel;
}
