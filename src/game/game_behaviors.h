#ifndef _GAME_BEHAVIORS_H_
#define _GAME_BEHAVIORS_H_

#include "entity.h"
#include "../game.h"

void behavior_player(Entity* entity, Game* game, float dt);

void behavior_time_controller(Entity* entity, Game* game, float dt);

int  handle_player_collisions(
  Game* game, PlayerFrameData old_fd, PlayerFrameData* new_fd);

void behavior_draw_physics_colliders(Entity* entity, Game* game, float dt);

void behavior_moving_platform(Entity* entity, Game* game, float dt);

vec2 platform_pos_at_frame(Movement* m, float frame);

#endif
