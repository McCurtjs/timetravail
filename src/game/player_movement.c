#include "game_behaviors.h"

#include <math.h>

//         pressed v
//      triggered v|
#define JUMP  0x0001
#define RIGHT 0x0002
#define LEFT  0x0004
#define DROP  0x0008
#define REPLAY (1 << 8)

#define PRESSED(mask) (inputs & (mask))
#define TRIGGERED(mask) (inputs & ((mask) << 4))

void handle_movement(PlayerFrameData* d, float dt, uint inputs, uint frame) {
  vec2 acceleration = v2zero;
  vec2 axis = v2x;
  uint animation_frame = frame - d->start_frame;

  if (d->standing) {
    axis = v2norm(v2sub(d->standing->b, d->standing->a));
  }

  if (PRESSED(RIGHT)) {
    acceleration = v2add(acceleration, v2scale(axis, accel[d->airborne] * dt));


    if (!d->airborne) {
      d->facing = FACING_RIGHT;
      // need to set animation direction in these, independent of velocity
      // have a turnaround animation based on velocity?
      //d->animation = ANIMATION_RUN;
    }

    //acceleration.x +=
    //  (d->vel.x + accel[d->airborne] * dt > max_vel[d->airborne])
    //  ? max_vel[d->airborne] - d->vel.x
    //  : accel[d->airborne] * dt;
    //if (!d->airborne && TRIGGERED(RIGHT) && d->vel.x < -16) {
    //  acceleration.x += -d->vel.x / 3;
    //}
  }

  if (PRESSED(LEFT)) {
    acceleration = v2add(acceleration, v2scale(axis, -accel[d->airborne] * dt));


    if (!d->airborne) {
      d->facing = FACING_LEFT;
      //d->animation = ANIMATION_RUN;
    }

    //acceleration.x +=
    //  d->vel.x - accel[d->airborne] * dt < -max_vel[d->airborne]
    //  ? -max_vel[d->airborne] - d->vel.x
    //  : -accel[d->airborne] * dt;
    //if (!d->airborne && TRIGGERED(LEFT) && d->vel.x > 16) {
    //  acceleration.x += -d->vel.x / 3;
    //}
  }

  if (!PRESSED(LEFT) && !PRESSED(RIGHT)) {
    if (!d->airborne) {
      acceleration = v2add(acceleration, v2scale(v2neg(d->vel), skid * dt));
      //acceleration.x += -d->vel.x * skid * dt;
    }
  }

  if (TRIGGERED(DROP) && d->airborne && d->vel.y < 0.2) {
    acceleration = v2add(acceleration, v2scale(v2down, drop));
    //acceleration.y += -drop;
  }

  bool about_to_jump = FALSE;
  if (TRIGGERED(JUMP)) {

    // regular jump has a delay to match the frame
    if (!d->airborne) {
      d->animation = ANIMATION_JUMP;
      about_to_jump = TRUE;

    // double jump
    } else if (d->has_double) {
      if ((d->vel.x < -1 && PRESSED(RIGHT))
      ||  (d->vel.x > 1 && PRESSED(LEFT))
      ) {
        // I made two animations, why not use them both. One of them rolls, the
        // other does not. Have fun using this for speedruns somehow :P
        d->animation = frame % 2 == 0 ?
          ANIMATION_DOUBLE_JUMP_REVERSE : ANIMATION_DOUBLE_JUMP_REVERSE_2;
        d->facing = !d->facing;
      } else {
        d->animation = ANIMATION_DOUBLE_JUMP;
      }
      d->has_double = FALSE;
    }

  // if we're already jumping, we can hold up to jump farther/higher
  } else if (PRESSED(JUMP) && d->airborne) {
    acceleration.y += lift * dt;
  }

  // trigger the delayed jump based on the animation frame
  if (d->animation == ANIMATION_JUMP && animation_frame <= jump_accel_frame) {
    // prevent later code from changing anim to idle before we actually jump
    about_to_jump = TRUE;

    if (animation_frame == jump_accel_frame) {
      //if (d->vel.y < -1) {
        acceleration.y += -d->vel.y + jump_str;
      //} else {
      //  acceleration = v2add(acceleration, v2scale(v2perp(axis), jump_str));
      //}
      d->airborne = TRUE;
      d->standing = NULL;
    }

  // trigger delayed jump velocity for double-jumps
  } else if (animation_frame == jump_double_accel_frame
  && (d->animation == ANIMATION_DOUBLE_JUMP
  ||  d->animation == ANIMATION_DOUBLE_JUMP_REVERSE
  ||  d->animation == ANIMATION_DOUBLE_JUMP_REVERSE_2
  )) {
    acceleration.y += -d->vel.y + jump_str;

    // switch horizontal direction if we're reverse-double-jumping
    if (d->animation != ANIMATION_DOUBLE_JUMP) {
      acceleration.x += -d->vel.x + d->vel.x * -jump_reverse_factor;
    }
  }

  if (!d->standing) {
    acceleration.y += -gravity * dt;
  }

  d->vel = v2add(d->vel, acceleration);

  // Apply the cap for maximum velocity (only horizontally)
  float walking = PRESSED(DROP) && !d->airborne ? walk_multiplier : 1.0;
  float total_max_v = max_vel[d->airborne] * walking;
  float v_along_axis = v2dot(d->vel, axis);

  if (v_along_axis > total_max_v) {
    d->vel = v2add(d->vel, v2scale(axis, total_max_v - v_along_axis));
  } else if (v_along_axis < -total_max_v) {
    d->vel = v2add(d->vel, v2scale(axis, -total_max_v - v_along_axis));
  }

  // Manage the type of walk/run animation based on the player's ground speed
  if (!d->airborne && !about_to_jump) {
    float player_speed = v2mag(d->vel);

    // this prevents animation switching from interrupting the sick roll
    if (d->animation == ANIMATION_ROLL_INTO_RUN && animation_frame < 60) {
      if (player_speed < min_roll_velocity) {
        float mv_sign = min_roll_velocity * (d->facing == FACING_LEFT ? -1 : 1);
        vec2 offset = v2sub(v2scale(axis, mv_sign), d->vel);
        d->vel = v2add(d->vel, offset);
      }

    // manage non-roll-oriented ground movement animations
    } else {
      // standing still
      if (player_speed < 1) {
        // bump into wall and landing animations include idle sequence loop
        if (!anim_is_idle(d->animation)) {
          d->animation = ANIMATION_IDLE;
        }

      // running speed
      } else if (player_speed > max_vel[0] - run_anim_threshold_diff
      &&        (PRESSED(LEFT) || PRESSED(RIGHT))
      ) {
        // roll animation already has the run loop baked into it
        if (d->animation != ANIMATION_ROLL_INTO_RUN) {
          d->animation = ANIMATION_RUN;
        }

      // walking pace
      } else {
        if (d->animation != ANIMATION_LAND || animation_frame >= 20) {
          d->animation = ANIMATION_WALK;
        }
      }
    }
  }

  // Finally, apply the velocity to the position of the player
  d->pos = v2add(d->pos, v2scale(d->vel, dt));
}
