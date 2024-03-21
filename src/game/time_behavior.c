#include "game_behaviors.h"

#include "wasm.h"
#include "draw.h"

#include "../test_behaviors.h"

static float reverse_speed = 1;
const static float reverse_max = -2.7;
const static float reverse_jolt = 0.0007;
const static float reverse_accel_start = 0.0;
static float reverse_accel = 0.0;

static void create_new_player(Game* game, Entity* e) {
  //print("Going to forward playback");
  PlayerFrameData new_fd = e->fd;
  new_fd.warp_triggered = 0;

  game_add_entity(game, &(Entity) {
    .type = ENTITY_PLAYER,
    .shader = &game->shaders.light,
    .model = &game->models.player,
    .fd = new_fd,
    .anim_data = {
      .anim_count = ANIMATION_COUNT,
      .animations = player_animations,
      .hitboxes = player_hitboxes,
    },
    .replay = NULL,
    .replay_temp = NULL,
    .render = render_sprites,
    .behavior = behavior_player,
    .delete = delete_player,
  });
}

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
    game->frame = 0;
  }

  // Find the active player - the last player instance in the list whose start
  // time was before the current frame (most recently created player)
  // TODO: this still breaks sometimes, not sure why
  PlayerRef* active = NULL;
  PlayerRef* array_foreach_index(ref, i, game->timeguys) {
    if (i == 0 || ref->start_frame <= game->frame) {
      active = ref;
    }
  }

  // (Actually do the forward playback reset here so we can use the active)
  if (game->reverse_playback && game->frame <= 0) {
    game->reverse_playback = FALSE;
    game->frame = 0;
    create_new_player(game, active->e);
  }

  // Active the reverse ability...
  if (game->input.triggered.run_replay && !active->e->fd.warp_triggered) {

    // If we're reverseing, flip back to forward and create a player instance
    if (game->reverse_playback) {

      create_new_player(game, active->e);

    // Otherwise, initialize the playback reversal
    } else {
      //print("Entering reverse playback");

      //((PlayerRef*)vector_get_back(&game->timeguys))->e->playback = TRUE;
    }

    game->reverse_playback = !game->reverse_playback;
    reverse_speed = 1;
    reverse_accel = reverse_accel_start;
  }

  // Debug draw attached to the "active" entity only
  //*
  draw.color = c4red;
  draw.vector_offset = v23(active->e->fd.pos);
  draw_vector(v23(v2scale(active->e->fd.vel, 0.5)));
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
