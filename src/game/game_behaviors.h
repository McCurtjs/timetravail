#ifndef _GAME_BEHAVIORS_H_
#define _GAME_BEHAVIORS_H_

#include "entity.h"
#include "../game.h"

// Player behaviors and helpers
void behavior_player(Entity* entity, Game* game, float dt);
void render_sprites(Entity* entity, Game* game);
void delete_player(Entity* entity);

bool handle_player_combat(Game* game, const Entity* e, PlayerFrameData* fd);
void handle_movement(PlayerFrameData* d, float dt, uint inputs, uint frame);
bool handle_player_collisions(Game* game, PlayerFrameData old_fd,
                              PlayerFrameData* new_fd, uint inputs);
void handle_abilities(PlayerFrameData old_fd, PlayerFrameData* new_fd,
                      uint inputs, uint frame, bool block_warp, bool cancel);

// Other entity controllers
void behavior_time_controller(Entity* entity, Game* game, float dt);
void behavior_goal(Entity* e, Game* _, float dt);
void behavior_draw_physics_colliders(Entity* entity, Game* game, float dt);
void behavior_gearspin(Entity* e, Game* game, float dt);

void behavior_moving_platform(Entity* entity, Game* game, float dt);
void delete_moving_platform(Entity* entity);
vec2 platform_pos_at_frame(Movement* m, float frame);

// Not actually a controller...
void finish_rendering_sprites(
  Game* game, const Model_Sprites* sprites, const Texture* texture);

// Player stats for movement and such
#define accel   ((float[2]){ 40.f, 12.f }) // [0] = ground, [1] = air
#define max_vel ((float[2]){ 13.f, 15.f })
#define min_roll_velocity (max_vel[0] * 2.f/5.f)
#define run_anim_threshold_diff 2.f
#define roll_anim_threshold_diff 4.f
#define walk_multiplier 0.5f
#define lift 17.f
#define skid 9.f
#define drop 8.f
#define gravity 9.8f * 5.f
#define jump_str 16.f
#define jump_reverse_factor 0.2f
#define max_replay_temp 120

#define jump_accel_frame 4
#define jump_double_accel_frame 6
#define physics_epsilon 0.01f
#define physics_bump_threshold 1

// Masks for reading the replay-packed input value
#define SHIFT_JUMP    0
#define SHIFT_RIGHT   1
#define SHIFT_LEFT    2
#define SHIFT_DROP    3
#define SHIFT_KICK    4
#define SHIFT_REPLAY  16
#define OFFSET_TRIGGER 0 // mask offset for key triggered values
#define OFFSET_PRESSED 8 // mask offset for key pressed values

//                                                        triggered -> |=======|
//                                                 pressed-> |=======| |       |
#define JUMP    (1 << SHIFT_JUMP)     // 0000 0000 0000 0000 0000 0001 0000 000t
#define RIGHT   (1 << SHIFT_RIGHT)    // 0000 0000 0000 0000 0000 0010 0000 00t0
#define LEFT    (1 << SHIFT_LEFT)     // 0000 0000 0000 0000 0000 0100 0000 0t00
#define DROP    (1 << SHIFT_DROP)     // 0000 0000 0000 0000 0000 1000 0000 t000
#define KICK    (1 << SHIFT_KICK)     // 0000 0000 0000 0000 0001 0000 000t 0000
#define REPLAY  (1 << SHIFT_REPLAY)   // 0000 0000 0000 000t 0000 0000 0000 0000

#define PRESSED(mask) (inputs & ((mask) << OFFSET_PRESSED))
#define TRIGGERED(mask) (inputs & ((mask) << OFFSET_TRIGGER))

#endif
