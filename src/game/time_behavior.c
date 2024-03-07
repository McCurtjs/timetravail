#include "game_behaviors.h"

#include "wasm.h"
#include "draw.h"

#include "../test_behaviors.h"

static float reverse_speed = 1;
const static float reverse_max = -2.7;
const static float reverse_jolt = 0.0007;
const static float reverse_accel_start = 0.0;
static float reverse_accel = 0.0;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
void behavior_time_controller(Entity* _, Game* game, float dt) {
#pragma clang diagnostic pop

  // Only true on the frame the speed changes from forward to backward
  game->reverse_triggered = FALSE;
  game->reverse_speed = 1;

  // Tick frame for normal playback
  if (!game->reverse_playback) {
    ++game->frame;

  // Tick for reverse playback
  } else if (game->frame > 0) {
    if (game->frame < reverse_speed)
      game->frame = 0;
    else {
      float old_rev_speed = reverse_speed;
      if (reverse_speed < reverse_max) {
        reverse_speed = reverse_max;
      } else {
        reverse_speed -= reverse_accel;
        reverse_accel += reverse_jolt;
      }

      game->frame += reverse_speed;

      if (reverse_speed > 0) {
        game->reverse_speed = reverse_speed;
      }

      if (old_rev_speed > 0 && reverse_speed <= 0) {
        game->reverse_triggered = TRUE;
      }
    }

  // Reset forward playback if we get to the beginning of the recording
  } else {
    game->reverse_playback = FALSE;
    game->frame = 0;
  }

  // Get the currently "active" player entity
  PlayerRef active, temp;
  for (uint index = 0; index < game->timeguys.size; ++index) {
    vector_read(&game->timeguys, index, &temp);

    if (index == 0 || temp.start_frame <= game->frame) {
      active = temp;
    }
  }

  // Active the reverse ability...
  if (game->input.triggered.run_replay) {

    // If we're reverseing, flip back to forward and create a player instance
    if (game->reverse_playback) {
      print("Going to forward playback");

      game_add_entity(game, &(Entity) {
        .shader = &game->shaders.basic,
        .model = &game->models.color_cube,
        .fd = active.e->fd,
        .replay = { .data = NULL },
        .replay_temp = { .data = NULL },
        .render = render_basic,
        .behavior = behavior_player,
      });

    // Otherwise, initialize the playback reversal
    } else {
      print("Entering reverse playback");

      //((PlayerRef*)vector_get_back(&game->timeguys))->e->playback = TRUE;
    }

    game->reverse_playback = !game->reverse_playback;
    reverse_speed = 1;
    reverse_accel = reverse_accel_start;
  }

  // Debug draw attached to the "active" entity only
  //*
  draw_color(v3x);
  draw_offset(v2v3(active.e->fd.pos, 0));
  draw_vector(v2v3(v2scale(active.e->fd.vel, 0.5), 0));
  //*/

//  // Camera control
//  static int lock_camera = FALSE;
//  if (lock_camera) {
//    game->camera.pos.xy = active.e->fd.pos;
//  }
//  if (game->input.triggered.camera_lock) {
//    lock_camera = !lock_camera;
//  }
}