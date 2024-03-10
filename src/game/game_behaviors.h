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
int  handle_player_collisions(
  Game* game, PlayerFrameData old_fd, PlayerFrameData* new_fd);

void handle_abilities(
  Game* game, PlayerFrameData old_fd, PlayerFrameData* new_fd, bool block_warp);

#define accel ((float[2]){45, 16}) // [0] = ground, [1] = air
#define max_vel ((float[2]){20, 23})
#define min_roll_velocity 8.0
#define run_anim_threshold_diff 2.0
#define roll_anim_threshold_diff 6.0
#define walk_multiplier 0.5
#define lift 17.0
#define skid 9.0
#define drop 10.0
#define gravity 9.8 * 5.0
#define jump_str 16.0
#define jump_reverse_factor 0.2

#define jump_accel_frame 4
#define jump_double_accel_frame 6

#endif
