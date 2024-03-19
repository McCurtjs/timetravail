#include "game_behaviors.h"

#include "wasm.h"
#include "draw.h"

bool handle_player_collisions(
  Game* game, PlayerFrameData old_fd, PlayerFrameData* new_fd, uint inputs
) {
  bool move_cancel = FALSE;

  if (new_fd->standing) {
    // If you're on the platform and it's moving, apply that movement
    if (new_fd->standing && new_fd->standing->moving) {
      vec2 curr = platform_pos_at_frame(new_fd->standing->moving, game->frame);
      vec2 prev = platform_pos_at_frame(new_fd->standing->moving, game->frame - 1);
      new_fd->pos = v2add(new_fd->pos, v2sub(curr, prev));
    }

    // Drop through the platform if it's droppable and you press down
    else if (TRIGGERED(DROP) && new_fd->standing->droppable
    &&      !new_fd->in_combat && !new_fd->hitstun
    ) {
      //drop_platform = new_fd->standing;
      new_fd->standing = NULL;
      new_fd->airborne = TRUE;
      new_fd->animation = ANIMATION_FALL;
    }
  }

  // Handle all platform collisions (there might be a good case for doing non-
  // standable walls before platforms you can stand on/lock to)
  for (uint i = 0; i < game->collider_count; ++i) {
    Line* line = &game->colliders[i];
    vec2 p;

    // skip if we're dropping through the line
    if (line->droppable && PRESSED(DROP)) {
      continue;
    }

    // skip if our path of travel doesn't intersect the platform
    if (!v2seg_seg(old_fd.pos, new_fd->pos, line->a, line->b, &p)) {
      continue;
    }

    vec2 delta = v2sub(new_fd->pos, old_fd.pos);
    vec2 line_v = v2norm(v2sub(line->b, line->a));
    vec2 line_n = v2perp(line_v);

    // skip if we're moving through the platform backwards (in line with normal)
    if (v2dot(delta, line_n) >= 0) {
      continue;
    }

    // case where it's a bouncy platform
    if (line->bouncy) {
      new_fd->vel = v2reflect(new_fd->vel, line_v);
      new_fd->standing = NULL;
      new_fd->airborne = TRUE;
      continue;
    }

    // if we're standing on a platform, make a line one epsilon away from
    // the collider and find the intersection with the movement vector to
    // prevent deviating from the direction of travel. do this to prevent
    // walls from pushing the player through the ground.
    if (new_fd->standing) {
      float t;
      vec2 line_epsilon = v2add(line->a, v2scale(line_n, physics_epsilon));
      v2line_line(old_fd.pos, delta, line_epsilon, line_v, &t, NULL);
      new_fd->pos = v2add(old_fd.pos, v2scale(delta, t));

    // unfortunately, while the previous method is extremely reliable for
    // never falling through platforms (even very narrow v shapes), since
    // it always reverses along the direction we can lose per-frame
    // velocity, which causes a stutter when sliding along a wall, so if
    // we're not standing on something, juse use the normal method.
    //
    // note: becuase we're only using the first method when grounded, it does
    // mean that the "push through objects" issue still happens if the player
    // hits a V shape made only of non-standable walls. This is annoying, but
    // really shouldn't happen in practice for game design reasons.
    } else {
      new_fd->pos = v2add(p, v2scale(line_n, physics_epsilon));
    }

    new_fd->vel = v2scale(line_v, v2dot(line_v, new_fd->vel));

    // If we hit a new stand-able platform, we are now on that platform
    if (!line->wall) {
      new_fd->standing = line;
      new_fd->airborne = FALSE;
      new_fd->has_double = TRUE;
      move_cancel = TRUE;

    // Otherwise, we may be getting pulled away from a platform, so check
    // platform->wall physics cases here
    } else if (new_fd->standing) {
      vec2 stand_v = v2sub(new_fd->standing->b, new_fd->standing->a);
      vec2 stand_n = v2norm(v2perp(stand_v));

      // we're being pushed towards the ground, nullify velocity
      if (v2dot(new_fd->vel, stand_n) < -physics_epsilon) {
        new_fd->vel = v2zero;
      }

      // If we're coming to a stop it's probably because we hit a wall
      if (v2mag(new_fd->vel) < physics_bump_threshold) {

        // play the "run into wall" animation
        // don't change animation while jumping to prevent interrupting the
        // jump "windup" if the player is standing but pressing toward the wall.
        if (new_fd->animation != ANIMATION_JUMP) {
          new_fd->animation = ANIMATION_BUMP_INTO_WALL;
        }

      // we're being pulled up and away from the wall, leave the platform
      } else {
        new_fd->standing = NULL;
        new_fd->airborne = TRUE;
        new_fd->animation = ANIMATION_FALL;
        move_cancel = TRUE;
      }
    }
  }

  // Post-collision cleanup and sanity checks
  if (new_fd->standing && new_fd->standing == old_fd.standing) {
    vec2 plat_v = v2sub(new_fd->standing->b, new_fd->standing->a);
    float platform_length = v2mag(plat_v);
    plat_v = v2norm(plat_v);
    vec2 entity_v = v2sub(new_fd->pos, new_fd->standing->a);
    float dot = v2dot(plat_v, entity_v);

    // check to see if we've walked off the edge of a platform
    if (dot < 0 || dot > platform_length) {
      new_fd->standing = NULL;
      new_fd->airborne = TRUE;
      new_fd->animation = ANIMATION_FALL;
      move_cancel = TRUE;

    // if we're still on the platform after all that, really latch onto it
    } else {
      vec2 p;
      v2line_closest(new_fd->standing->a, plat_v, new_fd->pos, &p);
      vec2 plat_n = v2perp(plat_v);
      new_fd->pos = v2add(p, v2scale(plat_n, physics_epsilon));
      new_fd->vel = v2scale(plat_v, v2dot(plat_v, new_fd->vel));
    }
  }

  // Handle special animations for landing
  float player_speed = v2mag(new_fd->vel);

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

// TODO: Give stand-able colliders a left and right pointer to the next line in
//       sequence, and transfer the player directly onto that line when running
//       past the edge, rotating velocity to preserve perceived speed. This will
//       be to prevent 1-frame fall animations when walking across platform gaps
//       and make it possible to run along bumpy ground without getting airtime.
