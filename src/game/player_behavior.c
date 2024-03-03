#include "player_behavior.h"

#include "wasm.h"
#include "draw.h"
#include "vector.h"

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
  vec2 pos;
  vec2 vel;
} ReplayNode;

static Vector replay = {.data = NULL};
static uint frame = 1;
static uint index = 0;
static int playback = FALSE;

void behavior_player(Entity* e, Game* game, float _) {
  float dt = 0.016;
  if (replay.data == NULL) {
    vector_init_reserve(&replay, sizeof(ReplayNode), 60 * 60);
    vector_push_back(&replay, &(ReplayNode) {
      .frame = 0,
      .buttons = 0,
      .pos = e->pos.xy,
      .vel = e->vel.xy,
    });
  }

  const static float accel[2] = {45, 16}; // [0] = ground, [1] = air
  const static float max_vel[2] = {20, 23};
  const static float lift = 17;
  const static float skid = 9;
  const static float drop = 10;
  const static float gravity = 9.8 * 5;
  const static float jump_str = 16;
  const static float jump_reverse_factor = 0.2;

  static int airborne = FALSE;
  static int has_double = FALSE;

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

  if (!playback) {
    ReplayNode node = *(ReplayNode*)vector_back(&replay);
    if (node.buttons != inputs) { // || frame % 60 == 0
      vector_push_back(&replay, &(ReplayNode) {
        .frame = frame,
        .buttons = inputs,
        .vel = e->vel.xy,
        .pos = e->pos.xy,
      });
      print_int(replay.size);
    }
  } else { // playback replay!
    inputs = 0;

    if (index < replay.size) {
      ReplayNode node = *(ReplayNode*)vector_get(&replay, index);

      if (frame == node.frame) {
        e->vel.xy = node.vel;
        e->pos.xy = node.pos;
        ++index;
      } else {
        node = *(ReplayNode*)vector_get(&replay, index-1);
      }

      inputs = node.buttons;
    }
  }

  vec3 acceleration = v3zero;

  if (PRESSED(RIGHT)) {
    acceleration.x += e->vel.x + accel[airborne] * dt > max_vel[airborne] ? max_vel[airborne] - e->vel.x : accel[airborne] * dt;
    if (!airborne && TRIGGERED(RIGHT) && e->vel.x < -16) {
      acceleration.x += -e->vel.x / 3;
    }
  }

  if (PRESSED(LEFT)) {
    acceleration.x += e->vel.x - accel[airborne] * dt < -max_vel[airborne] ? -max_vel[airborne] - e->vel.x : -accel[airborne] * dt;
    if (!airborne && TRIGGERED(LEFT) && e->vel.x > 16) {
      acceleration.x += -e->vel.x / 3;
    }
  }

  if (!PRESSED(LEFT) && !PRESSED(RIGHT)) {
    if (!airborne) {
      acceleration.x += -e->vel.x * skid * dt;
    }
  }

  if (TRIGGERED(DROP) && airborne && e->vel.y < 0.2) {
    acceleration.y += -drop;
  }

  if (TRIGGERED(JUMP) && (has_double || !airborne)) {
    acceleration.y += -e->vel.y + jump_str;
    if (airborne) {
      if ((e->vel.x < -1 && PRESSED(RIGHT))
      ||  (e->vel.x > 1 && PRESSED(LEFT))
      ){
        acceleration.x += -e->vel.x + e->vel.x * -jump_reverse_factor;
      }
      has_double = FALSE;
    }
    airborne = TRUE;
  } else if (PRESSED(JUMP) && airborne) {
    acceleration.y += lift * dt;
  }

  acceleration.y += -gravity * dt;
  e->vel = v3add(e->vel, acceleration);
  e->pos = v3add(e->pos, v3scale(e->vel, dt));

  if (e->pos.y <= 0) {
    e->vel.y = 0;
    e->pos.y = 0;
    airborne = FALSE;
    has_double = TRUE;
  }

  draw_offset(e->pos);
  draw_vector(v3scale(e->vel, 0.5));

  static int lock_camera = FALSE;
  if (lock_camera) {
    game->camera.pos.xy = e->pos.xy;
  }

  if (game->input.triggered.camera_lock) {
    lock_camera = !lock_camera;
  }
  e->transform = m4translation(v3add(e->pos, (vec3){0, 0.5, 0}));

  ++frame;

  if (inputs & REPLAY) {
    frame = 0;
    index = 0;
    playback = TRUE;
  }
}

// TODO:
// - For reverse playback, bake a node every 60 frames or so with pos/vel data
// - for a reverse frame, track to previous stored node and simulate up to that
//   frame. Store all the intervening nodes in a temporary buffer to reuse.
