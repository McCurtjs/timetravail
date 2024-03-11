#ifndef _GAME_BEHAVIORS_H_
#define _GAME_BEHAVIORS_H_

#include "entity.h"
#include "../game.h"

void handle_movement(PlayerFrameData* d, float dt, uint inputs, uint frame);
void behavior_player(Entity* entity, Game* game, float dt);
void behavior_time_controller(Entity* entity, Game* game, float dt);
void behavior_draw_physics_colliders(Entity* entity, Game* game, float dt);
void behavior_moving_platform(Entity* entity, Game* game, float dt);

void render_sprites(Entity* entity, Game* game);

void finish_rendering_sprites(
  Game* game, const Model_Sprites* sprites, const Texture* texture);

vec2 platform_pos_at_frame(Movement* m, float frame);

bool handle_player_collisions(
  Game* game, PlayerFrameData old_fd, PlayerFrameData* new_fd, uint inputs);

bool handle_player_combat(Game* game, const Entity* e, PlayerFrameData* fd);

void handle_abilities(
  PlayerFrameData old_fd, PlayerFrameData* new_fd, uint inputs,
  uint frame, bool block_warp, bool move_cancel);

#define accel ((float[2]){45, 16}) // [0] = ground, [1] = air
#define max_vel ((float[2]){20, 23})
#define min_roll_velocity 8.0
#define run_anim_threshold_diff 2.0
#define roll_anim_threshold_diff 4.0
#define walk_multiplier 0.5
#define lift 17.0
#define skid 9.0
#define drop 10.0
#define gravity 9.8 * 5.0
#define jump_str 16.0
#define jump_reverse_factor 0.2
#define max_replay_temp 120

#define jump_accel_frame 4
#define jump_double_accel_frame 6

// Masks for reading the replay-packed input value
#define SHIFT_JUMP    0
#define SHIFT_RIGHT   1
#define SHIFT_LEFT    2
#define SHIFT_DROP    3
#define SHIFT_KICK    4
#define SHIFT_REPLAY  16
#define SHIFT_TRIGGER 0
#define SHIFT_PRESSED 8

//                                                        triggered -> |=======|
//                                                 pressed-> |=======| |       |
#define JUMP    (1 << SHIFT_JUMP)     // 0000 0000 0000 0000 0000 0001 0000 000t
#define RIGHT   (1 << SHIFT_RIGHT)    // 0000 0000 0000 0000 0000 0010 0000 00t0
#define LEFT    (1 << SHIFT_LEFT)     // 0000 0000 0000 0000 0000 0100 0000 0t00
#define DROP    (1 << SHIFT_DROP)     // 0000 0000 0000 0000 0000 1000 0000 t000
#define KICK    (1 << SHIFT_KICK)     // 0000 0000 0000 0000 0001 0000 000t 0000
#define REPLAY  (1 << SHIFT_REPLAY)   // 0000 0000 0000 000t 0000 0000 0000 0000

#define PRESSED(mask) (inputs & ((mask) << SHIFT_PRESSED))
#define TRIGGERED(mask) (inputs & ((mask) << SHIFT_TRIGGER))

#endif
