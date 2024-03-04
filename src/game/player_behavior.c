#include "game_behaviors.h"

#include "wasm.h"
#include "draw.h"

#include "vector.h"
#include  "entity.h"

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

  if (PRESSED(RIGHT)) {
    acceleration.x +=
      (d->vel.x + accel[d->airborne] * dt > max_vel[d->airborne])
      ? max_vel[d->airborne] - d->vel.x
      : accel[d->airborne] * dt;
    if (!d->airborne && TRIGGERED(RIGHT) && d->vel.x < -16) {
      acceleration.x += -d->vel.x / 3;
    }
  }

  if (PRESSED(LEFT)) {
    acceleration.x +=
      d->vel.x - accel[d->airborne] * dt < -max_vel[d->airborne]
      ? -max_vel[d->airborne] - d->vel.x
      : -accel[d->airborne] * dt;
    if (!d->airborne && TRIGGERED(LEFT) && d->vel.x > 16) {
      acceleration.x += -d->vel.x / 3;
    }
  }

  if (!PRESSED(LEFT) && !PRESSED(RIGHT)) {
    if (!d->airborne) {
      acceleration.x += -d->vel.x * skid * dt;
    }
  }

  if (TRIGGERED(DROP) && d->airborne && d->vel.y < 0.2) {
    acceleration.y += -drop;
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
  } else if (PRESSED(JUMP) && d->airborne) {
    acceleration.y += lift * dt;
  }

  acceleration.y += -gravity * dt;
  d->vel = v2add(d->vel, acceleration);
  d->pos = v2add(d->pos, v2scale(d->vel, dt));
}

static int handle_collisions(
  PlayerFrameData _/*old_fd*/, PlayerFrameData* new_fd
) {
  if (new_fd->pos.y <= 0) {
    new_fd->pos.y = 0;
    new_fd->vel.y = 0;
    new_fd->airborne = FALSE;
    new_fd->has_double = TRUE;
    return 1;
  }
  return 0;
}

void behavior_player(Entity* e, Game* game, float _) {
  float dt = 0.016;

  // Initialize the replay data storage on first update
  if (e->replay.data == NULL) {
    vector_init_reserve(&e->replay, sizeof(ReplayNode), 60 * 60);
    vector_push_back(&e->replay, &(ReplayNode) {
      .frame = 0,
      .frame_until = 0,
      .buttons = 0,
      .data = e->fd,
    });
  }

  // Convert inputs from source game booleans to bitmask
  uint inputs = get_input_mask(game);

  // debug points
  draw_color(v3x);
  for (uint i = 0; i < e->replay.size; ++i) {
    ReplayNode node;
    vector_read(&e->replay, i, &node);
    draw_point(v2v3(node.data.pos, 0));
  }

  ReplayNode node; // this shouldn't be here, fix it

  // Handle input event recording
  if (!e->playback) {
    ReplayNode* prev_node = vector_get_back(&e->replay);

    if (game->frame - prev_node->frame >= max_replay_temp
    ||  prev_node->buttons != inputs
    ||  prev_node->data.airborne != e->fd.airborne
    ||  prev_node->data.has_double != e->fd.has_double
    ) {
      prev_node->frame_until = game->frame;

      vector_push_back(&e->replay, &(ReplayNode) {
        .frame = game->frame,
        .frame_until = game->frame,
        .buttons = inputs,
        .data = e->fd,
      });
    }

  // Handle replay playback
  } else if (e->replay.size) {
    if (e->replay_temp.data == NULL) {
      vector_init_reserve(&e->replay_temp, sizeof(ReplayNode), max_replay_temp);
    }

    uint index;
    ReplayNode node;

    // Find the snapshot containing the current frame
    // TODO: replace with binary search, this is gross
    for (index = 0; index < e->replay.size; ++index) {
      vector_read(&e->replay, index, &node);

      if (game->frame >= node.frame && game->frame < node.frame_until) {
        break;
      }
    }

    index = node.frame;

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

        until(++node.frame >= node.frame_until);

        PlayerFrameData updates = node.data;
        handle_input(&updates, dt, node.buttons);
        handle_collisions(node.data, &updates);
        node.data = updates;
      }
    }

    // Read the final correct node from the temp buffer
    if ((game->frame - index) < e->replay_temp.size) {
      vector_read(&e->replay_temp, game->frame - index, &node);
    } else {
      // if the frame isn't in the buffer, this entity ran out of time :(
      vector_read_back(&e->replay, &node);
    }

    // At this point, "node" should be set to the correct current frame
    temp = vector_get_back(&e->replay);
    if (game->frame != node.frame && game->frame < temp->frame_until) { // sanity check
      print("Node frame numbers mismatch! ! ! ! ! ! ! ! ! ! ! ! ");
      print_int(game->reverse_playback); print_int(node.frame); print_int(game->frame);
    }

    // Handle regular forward playback
    if (!game->reverse_playback) {
      e->fd = node.data;
      //inputs &= 0x00ff;
      inputs = node.buttons;

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

      e->fd.pos = node.data.pos;
      e->fd.vel = node.data.vel;

      vec3 _dbg_prev;
      for (uint i = 0; i < e->replay_temp.size; ++i) {
        ReplayNode node;
        vector_read(&e->replay_temp, i, &node);
        draw_point(v2v3(node.data.pos, 0));
        if (i > 0) draw_line(_dbg_prev, v2v3(node.data.pos, 0));
        _dbg_prev = v2v3(node.data.pos, 0);
      }
    }
  }

  // Simulate movement based on inputs
  if (!game->reverse_playback) {
    PlayerFrameData updates = e->fd;
    handle_input(&updates, dt, inputs);
    handle_collisions(node.data, &updates);
    handle_collisions(e->fd, &updates);
    e->fd = updates;
  }

  // Update rendering
  e->transform = m4translation(v2v3(v2add(e->fd.pos, (vec2){0, 0.5}), 0));
  draw_color(v3x);
  draw_offset(v2v3(e->fd.pos, 0));
  draw_vector(v2v3(v2scale(e->fd.vel, 0.5), 0));

  // Camera control
  static int lock_camera = FALSE;
  if (lock_camera) {
    game->camera.pos.xy = e->fd.pos;
  }
  if (game->input.triggered.camera_lock) {
    lock_camera = !lock_camera;
  }

  // Handle replay trigger
  if (inputs & REPLAY) {
    if (!e->playback) {
      e->playback = TRUE;
    }
  }
}
