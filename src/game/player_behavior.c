#include "player_behavior.h"

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
  uint buttons;
  PlayerFrameData data;
} ReplayNode;

// these all probably need to go in the player entity
static Vector replay = {.data = NULL};
static Vector replay_rev = {.data = NULL};

static uint index = 0;
static int playback = FALSE;
static int reverse_playback = FALSE;

static uint frame = 1; // should be managed by the game itself

const static float accel[2] = {45, 16}; // [0] = ground, [1] = air
const static float max_vel[2] = {20, 23};
const static float lift = 17;
const static float skid = 9;
const static float drop = 10;
const static float gravity = 9.8 * 5;
const static float jump_str = 16;
const static float jump_reverse_factor = 0.2;

static int since_last_record = 0;
const static int max_between_records = 60;

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
    acceleration.x += d->vel.x + accel[d->airborne] * dt > max_vel[d->airborne] ? max_vel[d->airborne] - d->vel.x : accel[d->airborne] * dt;
    if (!d->airborne && TRIGGERED(RIGHT) && d->vel.x < -16) {
      acceleration.x += -d->vel.x / 3;
    }
  }

  if (PRESSED(LEFT)) {
    acceleration.x += d->vel.x - accel[d->airborne] * dt < -max_vel[d->airborne] ? -max_vel[d->airborne] - d->vel.x : -accel[d->airborne] * dt;
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
    new_fd->airborne = FALSE; // these will have to be added to the fds later
    new_fd->has_double = TRUE;
    return 1;
  }
  return 0;
}

void behavior_player(Entity* e, Game* game, float _) {
  float dt = 0.016;

  // Initialize the replay data storage on first update
  if (replay.data == NULL) {
    vector_init_reserve(&replay, sizeof(ReplayNode), 60 * 60);
    vector_push_back(&replay, &(ReplayNode) {
      .frame = 0,
      .buttons = 0,
      .data = e->fd,
    });
    since_last_record = 0;
  }

  // Convert inputs from source game booleans to bitmask
  uint inputs = get_input_mask(game);
  ReplayNode node;

  // debug points
  draw_color(v3x);
  for (uint i = 0; i < replay.size; ++i) {
    ReplayNode node;
    vector_read(&replay, i, &node);
    draw_point(v2v3(node.data.pos, 0));
  }

  // Handle playback recording
  if (!playback) {
    vector_read_back(&replay, &node);

    if (node.buttons != inputs
    ||  node.data.airborne != e->fd.airborne
    ||  node.data.has_double != e->fd.has_double
    ||  since_last_record >= max_between_records
    ) {
      vector_push_back(&replay, &(ReplayNode) {
        .frame = frame,
        .buttons = inputs,
        .data = e->fd,
      });
      since_last_record = 0;
    } else {
      ++since_last_record;
    }

  // Handle playback in reverse
  } else if (reverse_playback) {
    if (replay_rev.data == NULL) {
      vector_init_reserve(&replay_rev, sizeof(ReplayNode), max_between_records);
    }

    if (replay_rev.size == 0) { // if replay buffer is empty, fill with next batch
      vector_read(&replay, index, &node); // vector_read(&replay, index, &node);
      print_int(index);
      if (frame >= node.frame) {
        print("Should only happen once per reverse replay");
        vector_push_back(&replay_rev, &node);
      } else {
        vector_read(&replay, --index, &node);

        loop {
          vector_push_back(&replay_rev, &node);

          until(node.frame++ >= frame);

          PlayerFrameData updates = node.data;
          handle_input(&updates, dt, node.buttons);
          handle_collisions(node.data, &updates);
          node.data = updates;
        }
      }
    }

    draw_color(v3z);
    vec3 _dbg_prev;
    for (uint i = 0; i < replay_rev.size; ++i) {
      ReplayNode node;
      vector_read(&replay_rev, i, &node);
      draw_point(v2v3(node.data.pos, 0));
      if (i > 0) draw_line(_dbg_prev, v2v3(node.data.pos, 0));
      _dbg_prev = v2v3(node.data.pos, 0);
    }

    if (replay_rev.size) { // if replay buffer has frames, pop and apply
      vector_pop_back(&replay_rev, &node);

      e->fd.pos = node.data.pos;
      e->fd.vel = node.data.vel;

      if (frame != node.frame) { // sanity check
        print("Node frame numbers mismatch!");
        print_int(node.frame); print_int(frame);
      }
    }

  // Handle regular forward playback
  } else {
    inputs = 0;

    if (index < replay.size) {
      vector_read(&replay, index, &node);

      if (frame == node.frame) {
        e->fd = node.data;
        ++index;
      } else {
        vector_read(&replay, index-1, &node);
      }

      inputs = node.buttons;
    }
  }

  // Simulate movement based on inputs
  if (!reverse_playback) {
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

  // Tick frame
  if (!reverse_playback) {
    ++frame;
  } else if (frame > 0) {
    --frame;
  } else {
    reverse_playback = FALSE;
  }

  // Handle replay trigger
  if (inputs & REPLAY) {
    if (!playback) {
      frame = 0;
      index = 0;
      e->fd.vel = v2zero;
      playback = TRUE;
    } else if (!reverse_playback) {
      reverse_playback = TRUE;
      index = replay.size - 1;
      ReplayNode node;
      vector_read_back(&replay, &node);
      frame = node.frame;
      print("----");
      print_int(index);
      print("----");
    }
  }
}

// TODO:
// - For reverse playback, bake a node every 60 frames or so with pos/vel data
// - for a reverse frame, track to previous stored node and simulate up to that
//   frame. Store all the intervening nodes in a temporary buffer to reuse.
