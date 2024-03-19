#include "game_behaviors.h"

#include "wasm.h"
#include "draw.h"

#include "../test_behaviors.h"
#include "vector.h"
#include "entity.h"

#include <math.h>
#include <stdlib.h>

uint get_input_mask(Game* game) {
  uint inputs = 0;
  inputs |= game->input.pressed.forward   << (SHIFT_JUMP  + SHIFT_PRESSED);
  inputs |= game->input.pressed.right     << (SHIFT_RIGHT + SHIFT_PRESSED);
  inputs |= game->input.pressed.left      << (SHIFT_LEFT  + SHIFT_PRESSED);
  inputs |= game->input.pressed.back      << (SHIFT_DROP  + SHIFT_PRESSED);
  inputs |= game->input.pressed.kick      << (SHIFT_KICK  + SHIFT_PRESSED);
  inputs |= game->input.triggered.forward << (SHIFT_JUMP  + SHIFT_TRIGGER);
  inputs |= game->input.triggered.right   << (SHIFT_RIGHT + SHIFT_TRIGGER);
  inputs |= game->input.triggered.left    << (SHIFT_LEFT  + SHIFT_TRIGGER);
  inputs |= game->input.triggered.back    << (SHIFT_DROP  + SHIFT_TRIGGER);
  inputs |= game->input.triggered.kick    << (SHIFT_KICK  + SHIFT_TRIGGER);
  inputs |= game->input.triggered.run_replay << (SHIFT_REPLAY);
  return inputs;
}

typedef struct ReplayNode {
  uint frame;
  uint frame_until;
  uint buttons;
  PlayerFrameData data;
} ReplayNode;

void behavior_player(Entity* e, Game* game, float _) {
  float dt = 0.016;

  // Convert inputs from source game booleans to bitmask
  uint inputs = get_input_mask(game);

  // Stuipd stuipd stupid
  uint first_frame = FALSE;

  // Initialize the replay data storage on first update
  if (e->replay.data == NULL) {
    vector_init_reserve(&e->replay, sizeof(ReplayNode), 200);
    vector_push_back(&e->replay, &(ReplayNode) {
      .frame = game->frame,
      .frame_until = game->frame,
      .buttons = 0,
      .data = e->fd,
    });

    first_frame = TRUE;

    // Store a a pointer to this player entity along with the current frame
    // to mark it as the "active" player for its frame set
    ((PlayerRef*)vector_get_back(&game->timeguys))->end_frame = game->frame;
    vector_push_back(&game->timeguys, &(PlayerRef) {
      .start_frame = game->frame,
      .e = e,
    });

    //print("New player!");
    //print_int((int)e);
  }

  /* debug points
  draw_color(v3x);
  for (uint i = 0; i < e->replay.size; ++i) {
    ReplayNode node;
    vector_read(&e->replay, i, &node);
    draw_point(v23f(node.data.pos, 0));
  } //*/


  // Handle input event recording
  if (!e->playback) {
    ReplayNode* prev_node = vector_get_back(&e->replay);
    bool hit_max_node_time = game->frame - prev_node->frame >= max_replay_temp;

    if (hit_max_node_time
    ||  game->reverse_triggered
    ||  prev_node->buttons != inputs
    ||  prev_node->data.airborne != e->fd.airborne
    ||  prev_node->data.has_double != e->fd.has_double
    ||  prev_node->data.standing != e->fd.standing
    ||  prev_node->data.animation != e->fd.animation
    ||  prev_node->data.facing != e->fd.facing
    ) {
      // Because fractional frames, make sure we aren't double-counting frames
      if ((uint)game->frame != prev_node->frame) {
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
    dt = dt * game->reverse_speed;
    bool cancel = 0;
    cancel |= handle_player_combat(game, e, &updates);
    handle_movement(&updates, dt, inputs, (uint)game->frame);
    cancel |= handle_player_collisions(game, e->fd, &updates, inputs);
    handle_abilities(e->fd, &updates, inputs, game->frame, first_frame, cancel);
    e->fd = updates;

    // special cases with animations...
    uint prev_anim = prev_node->data.animation;
    uint next_anim = e->fd.animation;

    if (hit_max_node_time
    && anim_is_idle(prev_anim) && next_anim == prev_anim
    ) {
      srand((uint)game->frame);
      uint r = rand();
      if (r % 7 < 4) {
        e->fd.animation = ANIMATION_IDLE_2;
      }
    }

    // If we started a new animation on this frame, update the anim start frame
    if (prev_anim != e->fd.animation) {
      // walk and run map onto each other, so don't update the start frame
      unless ((anim_is_ground(prev_anim) && anim_is_ground(next_anim))) {
        e->fd.start_frame = game->frame;
      }
    }

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
        handle_movement(&updates, dt, node.buttons, node.frame);
        bool cancel = FALSE;
        cancel |= handle_player_combat(game, NULL, &node.data);
        cancel |= handle_player_collisions(game, node.data, &updates, node.buttons);
        handle_abilities(node.data, &updates, node.buttons, node.frame, TRUE, cancel);
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

    // And change the warp animations for the "ghosts"
    if (anim_is_warp(e->fd.animation)) {
      if (e != vector_get_back(&game->entities)) {
        e->fd.animation += 1;
      }
    }

    /* Draw debug trails showing history nodes in the temp buffer
    if (!game->reverse_playback) {
      draw_color(c4green.rgb);
    } else {
      draw_color(c4blue.rgb);
    }

    vec3 _dbg_prev;
    for (uint i = 0; i < e->replay_temp.size; ++i) {
      ReplayNode node;
      vector_read(&e->replay_temp, i, &node);
      draw_point(v23f(node.data.pos, 0));
      if (i > 0) draw_line(_dbg_prev, v23f(node.data.pos, 0));
      _dbg_prev = v23f(node.data.pos, 0);
    } //*/
  }

  // Update rendering
  e->transform = m4uniform(3);
  e->transform = m4mul(m4translation(v23f(v2add(e->fd.pos, (vec2){0, 1.5}), 0)), e->transform);

  // Camera control
  // (you want to guarantee the camera control is at the end to avoid stuttering
  // when the player moves after the camera in update sequence)
  if (e == game_get_active_player(game)) {
    //static int lock_camera = FALSE;
    //if (lock_camera) {
      game->camera.pos.xy = v2add(e->fd.pos, v2scale(v2y, 3));
    //}
    //if (game->input.triggered.camera_lock) {
    //  lock_camera = !lock_camera;
    //}
  }
}

// TODO:
//  - level object for loading different kinds of levels
//    - will also need an entity delete function to clean up temp things
//  - "end of level" entity, probably just a "touch and you win" for now
//  - debug time nodes to see if any are being missed causing the desyncs on hit
//  - genericize entity replay a bit, or copy it to work with oths (like keys)
//  - stopwatch model and effect (maybe just do the hand spin first)
//    - yeah, we're gonna need model loading most likely...
//    - CRT static and film-roll rewind effect when playing backwards
//  - differentiate platform types/reactions based on their angle
//    - steep slopes (walk up slower)
//    - give Line its own "behavior" setup to react when collided with?
//  - make b-air even stronger
