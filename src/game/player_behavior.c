#include "game_behaviors.h"

#include "wasm.h"
#include "draw.h"

#include "../test_behaviors.h"
#include "vector.h"
#include  "entity.h"

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

typedef struct ReplayNode {
  uint frame;
  uint frame_until;
  uint buttons;
  PlayerFrameData data;
} ReplayNode;

const static float accel[2] = {45, 16}; // [0] = ground, [1] = air
const static float max_vel[2] = {20, 23};
const static float lift = 17;
const static float skid = 9;
const static float drop = 10;
const static float gravity = 9.8 * 5;
const static float jump_str = 16;
const static float jump_reverse_factor = 0.2;

const static int max_replay_temp = 60;

static uint get_input_mask(Game* game) {
  uint inputs = 0;
  inputs |= game->input.pressed.forward << 0;
  inputs |= game->input.pressed.right << 1;
  inputs |= game->input.pressed.left << 2;
  inputs |= game->input.pressed.back << 3;
  inputs |= game->input.triggered.forward << 4;
  inputs |= game->input.triggered.right << 5;
  inputs |= game->input.triggered.left << 6;
  inputs |= game->input.triggered.back << 7;
  inputs |= game->input.triggered.run_replay << 8;
  return inputs;
}

static void handle_input(PlayerFrameData* d, float dt, uint inputs) {
  vec2 acceleration = v2zero;
  vec2 axis = v2x;

  if (d->standing) {
    axis = v2norm(v2sub(d->standing->b, d->standing->a));
  }

  if (PRESSED(RIGHT)) {
    acceleration = v2add(acceleration, v2scale(axis, accel[d->airborne] * dt));

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

  if (d->vel.x > max_vel[d->airborne]) {
    d->vel.x = max_vel[d->airborne];
  } else if (d->vel.x < -max_vel[d->airborne]) {
    d->vel.x = -max_vel[d->airborne];
  }

  if (TRIGGERED(DROP) && d->airborne && d->vel.y < 0.2) {
    acceleration = v2add(acceleration, v2scale(v2down, drop));
    //acceleration.y += -drop;
  }

  if (TRIGGERED(JUMP) && (d->has_double || !d->airborne)) {
    acceleration.y += -d->vel.y + jump_str;
    if (d->airborne) {
      if ((d->vel.x < -1 && PRESSED(RIGHT))
      ||  (d->vel.x > 1 && PRESSED(LEFT))
      ){
        acceleration.x += -d->vel.x + d->vel.x * -jump_reverse_factor;
      }
      d->has_double = FALSE;
    }
    d->airborne = TRUE;
    d->standing = NULL;
  } else if (PRESSED(JUMP) && d->airborne) {
    acceleration.y += lift * dt;
  }

  if (!d->standing) {
    acceleration.y += -gravity * dt;
  }

  d->vel = v2add(d->vel, acceleration);
  d->pos = v2add(d->pos, v2scale(d->vel, dt));
}

void behavior_player(Entity* e, Game* game, float _) {
  float dt = 0.016;

  // Convert inputs from source game booleans to bitmask
  uint inputs = get_input_mask(game);

  // Initialize the replay data storage on first update
  if (e->replay.data == NULL) {
    vector_init_reserve(&e->replay, sizeof(ReplayNode), 200);
    vector_push_back(&e->replay, &(ReplayNode) {
      .frame = game->frame,
      .frame_until = game->frame,
      .buttons = 0,
      .data = e->fd,
    });

    // Store a a pointer to this player entity along with the current frame
    // to mark it as the "active" player for its frame set
    ((PlayerRef*)vector_get_back(&game->timeguys))->end_frame = game->frame;
    vector_push_back(&game->timeguys, &(PlayerRef) {
      .start_frame = game->frame,
      .e = e,
    });

    print("New dude!");
    print_int((int)e);
  }

  /* debug points
  draw_color(v3x);
  for (uint i = 0; i < e->replay.size; ++i) {
    ReplayNode node;
    vector_read(&e->replay, i, &node);
    draw_point(v2v3(node.data.pos, 0));
  } //*/


  // Handle input event recording
  if (!e->playback) {
    ReplayNode* prev_node = vector_get_back(&e->replay);

    if (game->frame - prev_node->frame >= max_replay_temp
    ||  game->reverse_triggered
    ||  prev_node->buttons != inputs
    ||  prev_node->data.airborne != e->fd.airborne
    ||  prev_node->data.has_double != e->fd.has_double
    ||  prev_node->data.standing != e->fd.standing
    ) {
      // Because fractional frames, make sure we aren't double-counting frames
      if ((uint)game->frame!= prev_node->frame) {
        prev_node->frame_until = game->frame;

        vector_push_back(&e->replay, &(ReplayNode) {
         .frame = game->frame,
          .frame_until = game->frame,
          .buttons = inputs,
          .data = e->fd,
        });
      }

      // Stop recording when the game tells us the slowdown is over
      if (game->reverse_triggered) {
        e->playback = TRUE;
      }
    }

    // Simulate movement based on inputs
    PlayerFrameData updates = e->fd;
    handle_input(&updates, dt * game->reverse_speed, inputs);
    handle_player_collisions(game, e->fd, &updates);
    e->fd = updates;

  // Handle replay playback
  } else if (e->replay.size) {
    if (e->replay_temp.data == NULL) {
      vector_init_reserve(&e->replay_temp, sizeof(ReplayNode), max_replay_temp);
    }

    uint index;
    ReplayNode node, next;

    // First check if the current frame is before the first playback frame
    vector_read_front(&e->replay, &node);
    if (node.frame > game->frame) {
      e->hidden = TRUE;
      return; // hard exit, don't waste time simulating playback
    } else {
      e->hidden = FALSE;
    }

    // Find the snapshot containing the current frame
    // TODO: replace with binary search, this is gross
    for (index = 0; index < e->replay.size; ++index) {
      vector_read(&e->replay, index, &node);

      if (game->frame >= node.frame && game->frame < node.frame_until) {
        break;
      }
    }

    uint block_start = node.frame;

    // If the current frame is within the bounds of the temp array, get it
    ReplayNode* temp = NULL;
    if (e->replay_temp.size > 0) {
      temp = vector_get_front(&e->replay_temp);
    }

    // If it's not in the temp buffer, we need to simulate a new range
    if (temp == NULL || temp->frame != node.frame) {
      vector_clear(&e->replay_temp);

      loop {
        vector_push_back(&e->replay_temp, &node);

        // go one past the end so we can be sure the temp buffer also contains
        // the next frame (pre-inc is exact, post-inc overlaps the next block)
        until(node.frame++ >= node.frame_until);

        PlayerFrameData updates = node.data;
        handle_input(&updates, dt, node.buttons);
        handle_player_collisions(game, node.data, &updates);
        node.data = updates;
      }
    }

    // Read the final correct node from the temp buffer
    if ((game->frame - block_start + 1) < e->replay_temp.size) {
      vector_read(&e->replay_temp, game->frame - block_start, &node);
      vector_read(&e->replay_temp, game->frame - block_start + 1, &next);
    } else {
      // if the frame isn't in the buffer, this entity ran out of time :(
      vector_read_back(&e->replay, &next);
      next = node;
    }

    // Interpolate position between the current and next frame for decimal frame
    float frame_t = game->frame - floorf(game->frame);
    node.data.pos = v2lerp(node.data.pos, next.data.pos, frame_t);

    // At this point, "node" should be set to the correct current frame
    temp = vector_get_back(&e->replay);
    if ((uint)game->frame != node.frame && game->frame < temp->frame_until) { // sanity check
      print("Node frame numbers mismatch! ! ! ! ! ! ! ! ! ! ! ! ");
      print_int((int)e); print_int(node.frame); print_int(game->frame);
    }

    // Finally, set the location of the entity
    e->fd = node.data;

    //* Handle regular forward playback
    if (!game->reverse_playback) {
      draw_color(v3y);
      vec3 _dbg_prev;
      for (uint i = 0; i < e->replay_temp.size; ++i) {
        ReplayNode node;
        vector_read(&e->replay_temp, i, &node);
        draw_point(v2v3(node.data.pos, 0));
        if (i > 0) draw_line(_dbg_prev, v2v3(node.data.pos, 0));
        _dbg_prev = v2v3(node.data.pos, 0);
      }

    // Handle playback of the replay in reverse
    } else {
      draw_color(v3z);
      vec3 _dbg_prev;
      for (uint i = 0; i < e->replay_temp.size; ++i) {
        ReplayNode node;
        vector_read(&e->replay_temp, i, &node);
        draw_point(v2v3(node.data.pos, 0));
        if (i > 0) draw_line(_dbg_prev, v2v3(node.data.pos, 0));
        _dbg_prev = v2v3(node.data.pos, 0);
      }
    } //*/
  }

  // Update rendering
  e->transform = m4translation(v2v3(v2add(e->fd.pos, (vec2){0, 0.5}), 0));

  // Camera control
  // (you want to guarantee the camera control is at the end to avoid stuttering
  // when the player moves after the camera in update sequence)
  PlayerRef active, temp;
  for (uint index = 0; index < game->timeguys.size; ++index) {
    vector_read(&game->timeguys, index, &temp);

    if (index == 0 || temp.start_frame <= game->frame) {
      active = temp;
    }
  }

  if (active.e == e) {
    static int lock_camera = FALSE;
    if (lock_camera) {
      game->camera.pos.xy = e->fd.pos;
    }
    if (game->input.triggered.camera_lock) {
      lock_camera = !lock_camera;
    }
  }
}

// TODO:
//  - differentiate platform types/reactions based on their angle
//    - steep slopes (walk up slower)
//    - walls (no standing changes - will need a collision frame record)
//    - no-stand collider with bouncy property?
//    - moving platforms (tie physics platforms to entities, record movement in Line class?)
//      - give Line its own "behavior" setup to react when collided with?
//      - moving platforms will probably just have to move based on time functions (cos(frame))
//    - platform drop
//    - make sure if the player is moving fast and hits two platforms, pick the nearest
//  - test the epsilon thing again? or ignore it since it never works well
//    - or set epsilon to zero so the platforms can be EXACT
