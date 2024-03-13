#include "game_behaviors.h"

#include "wasm.h"
#include "draw.h"

bool handle_player_collisions(
  Game* game, PlayerFrameData old_fd, PlayerFrameData* new_fd, uint inputs
) {
  const Line* drop_platform = NULL;
  bool move_cancel = FALSE;
  bool fall_off_edge = FALSE;

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
      fall_off_edge = TRUE;
    }

    // Drop through the platform if it's droppable and you press down
    else if (TRIGGERED(DROP) && new_fd->standing->droppable
    &&      !new_fd->in_combat && !new_fd->hitstun
    ) {
      drop_platform = new_fd->standing;
      new_fd->standing = NULL;
      new_fd->airborne = TRUE;
      new_fd->animation = ANIMATION_FALL;
    }
  }

  // only check against standable platforms first
  for (uint i = 0; i < game->collider_count; ++i) {
    Line* line = &game->colliders[i];
    vec2 p;

    if (line == new_fd->standing) continue;
    if (line->droppable && PRESSED(DROP)) continue;
    if (line->bouncy || line->wall) continue;

    if (v2seg_seg(old_fd.pos, new_fd->pos, line->a, line->b, &p)) {
      // we are intentionally dopping through this platform, so skip it
      if (drop_platform == &game->colliders[i]) {
        continue;
      }

      vec2 dir = v2sub(new_fd->pos, old_fd.pos);
      vec2 v = v2sub(line->b, line->a);
      vec2 v_unit = v2norm(v);
      vec2 n = v2perp(v_unit);

      if (v2dot(dir, n) < 0) {
        // account for length of chopped off segment?
        new_fd->pos = v2add(p, v2scale(n, physics_epsilon));

        new_fd->vel = v2scale(v_unit, v2dot(v_unit, new_fd->vel));

        if (!line->wall && !line->bouncy) {
          new_fd->standing = line;
          new_fd->airborne = FALSE;
          new_fd->has_double = TRUE;
          move_cancel = TRUE;
        }
      }
    }
  }

  vec2 stand_v = v2zero;
  vec2 stand_n = v2zero;

  if (new_fd->standing) {
    stand_v = v2norm(v2sub(new_fd->standing->b, new_fd->standing->a));
    stand_n = v2perp(stand_v);
  }

  // now check for non-standable colliders, handle them slightly differently
  for (uint i = 0; i < game->collider_count; ++i) {
    Line* line = &game->colliders[i];
    vec2 p;

    if (line->droppable && PRESSED(DROP)) continue;
    if (!line->bouncy && !line->wall) continue;

    if (v2seg_seg(old_fd.pos, new_fd->pos, line->a, line->b, &p)) {
      // we are intentionally dopping through this platform, so skip it
      if (drop_platform == &game->colliders[i]) {
        continue;
      }

      vec2 dir = v2sub(new_fd->pos, old_fd.pos);
      vec2 v = v2sub(line->b, line->a);
      vec2 v_unit = v2norm(v);
      vec2 n = v2perp(v_unit);

      // collision with the test line
      if (v2dot(dir, n) < 0) {

        // Handle the case where you land on a bouncy platform
        if (line->bouncy) {
          //new_fd->pos = v2add(p, v2scale(n, physics_epsilon));
          new_fd->vel = v2reflect(new_fd->vel, v_unit);
          new_fd->standing = NULL;
          new_fd->airborne = TRUE;

        // Handle the regular case where we slide along the line
        } else {
          new_fd->pos = v2add(p, v2scale(n, physics_epsilon));
          new_fd->vel = v2scale(v_unit, v2dot(v_unit, new_fd->vel));
        }

        // If we're standing on a platform and hit a wall...
        if (new_fd->standing && new_fd->standing == old_fd.standing) {

          vec2 new_dir = v2sub(new_fd->pos, old_fd.pos);

          // if it's a wedge and you're now sliding through the ground...
          if (v2dot(new_dir, stand_n) < physics_epsilon) {
            // make a line one epsilon away from the wall, and use that to find
            // the point that's one epsilon away from both the wall and platform
            float t;
            v2line_line(old_fd.pos, dir, new_fd->pos, v_unit, &t, NULL);
            new_fd->pos = v2add(old_fd.pos, v2scale(dir, t));

            if (v2dot(new_fd->vel, stand_n) < -physics_epsilon) {
              new_fd->vel = v2zero;
            }

            if (new_fd->animation != ANIMATION_JUMP
            && v2mag(new_fd->vel) < physics_epsilon
            ) {
              new_fd->animation = ANIMATION_BUMP_INTO_WALL;
            }

          //if it's a slope, you're being propelled up away from the platform
          } else {
            new_fd->standing = NULL;
            new_fd->airborne = TRUE;
          }
        }
      }
    }
  }

  // After all that, if we're still standing on something, make sure we're
  // really locked down to it
  if (new_fd->standing && new_fd->standing == old_fd.standing
  && v2dot(new_fd->vel, stand_n) < physics_epsilon) {
    vec2 p;
    vec2 v = v2sub(new_fd->standing->b, new_fd->standing->a);
    v2line_closest(new_fd->standing->a, v, new_fd->pos, &p);
    vec2 n = v2norm(v2perp(v));
    new_fd->pos = v2add(p, v2scale(n, physics_epsilon));
  }

  if (fall_off_edge && !new_fd->standing) {
  //  print("This is happening here");
    new_fd->airborne = TRUE;
    new_fd->animation = ANIMATION_FALL;
    move_cancel = TRUE;
  //  print_int((uint)game->frame);
  }

  //if (new_fd->standing && new_fd->standing != old_fd.standing) {
  //  print_int((uint)game->frame);
  //}

  // implicit ground plane
  //if (new_fd->pos.y <= 0) {
  //  if (new_fd->airborne) move_cancel = TRUE;
  //  new_fd->pos.y = 0;
  //  new_fd->vel.y = 0;
  //  new_fd->airborne = FALSE;
  //  new_fd->has_double = TRUE;
  //}

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
    || !(PRESSED(LEFT) || PRESSED(RIGHT))
    ) {
      new_fd->animation = ANIMATION_LAND;
    }

    // (if we're at running speed in hangtime-1, just go straight to running)
  }

  return move_cancel;
}
