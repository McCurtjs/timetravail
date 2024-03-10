#include "animations.h"

#define ROW 16

static Frame player_idle[] = {
  { .frame = ROW*0 + 0 },
  { .frame = ROW*0 + 1 },
  { .frame = ROW*0 + 2 },
  { .frame = ROW*0 + 3 },
  { .frame = ROW*0 + 4 },
  { .frame = ROW*0 + 5 },
  { .frame = ROW*0 + 6 },
  { .frame = ROW*0 + 7 },
  { .frame = ROW*0 + 8 },
  { .frame = ROW*0 + 9 },
  { .frame = ROW*0 + 10 },
  { .frame = ROW*0 + 11 },
  { .frame = ROW*0 + 12 },
  { .frame = ROW*0 + 13 },
  { .frame = ROW*0 + 14 },
  { .frame = ROW*0 + 15 },
};

static Frame player_idle2[] = {
  { .frame = ROW*9 + 10 },
  { .frame = ROW*9 + 11 },
  { .frame = ROW*9 + 12 },
  { .frame = ROW*9 + 13 },
  { .frame = ROW*9 + 14 },
  { .frame = ROW*9 + 15 },
  { .frame = ROW*10+ 10 },
  { .frame = ROW*10+ 11 },
  { .frame = ROW*10+ 12 },
  { .frame = ROW*10+ 13 },
  { .frame = ROW*10+ 14 },
  { .frame = ROW*10+ 15 },
  { .frame = ROW*0 + 0 },
  { .frame = ROW*0 + 1 },
  { .frame = ROW*0 + 2 },
  { .frame = ROW*0 + 3 },
  { .frame = ROW*0 + 4 },
  { .frame = ROW*0 + 5 },
  { .frame = ROW*0 + 6 },
  { .frame = ROW*0 + 7 },
  { .frame = ROW*0 + 8 },
  { .frame = ROW*0 + 9 },
  { .frame = ROW*0 + 10 },
  { .frame = ROW*0 + 11 },
  { .frame = ROW*0 + 12 },
  { .frame = ROW*0 + 13 },
  { .frame = ROW*0 + 14 },
  { .frame = ROW*0 + 15 },
};

static Frame player_walk[] = {
  { .frame = ROW*1 + 0 },
  { .frame = ROW*1 + 1 },
  { .frame = ROW*1 + 2 },
  { .frame = ROW*1 + 3 },
  { .frame = ROW*1 + 4 },
  { .frame = ROW*1 + 5 },
  { .frame = ROW*1 + 6 },
  { .frame = ROW*1 + 7 },
  { .frame = ROW*1 + 8 },
  { .frame = ROW*1 + 9 },
  { .frame = ROW*1 + 10 },
  { .frame = ROW*1 + 11 },
  { .frame = ROW*1 + 12 },
  { .frame = ROW*1 + 13 },
  { .frame = ROW*1 + 14 },
  { .frame = ROW*1 + 15 },
};

static Frame player_run[] = {
  { .frame = ROW*2 + 0 },
  { .frame = ROW*2 + 1 },
  { .frame = ROW*2 + 2 },
  { .frame = ROW*2 + 3 },
  { .frame = ROW*2 + 4 },
  { .frame = ROW*2 + 5 },
  { .frame = ROW*2 + 6 },
  { .frame = ROW*2 + 7 },
  { .frame = ROW*2 + 8 },
  { .frame = ROW*2 + 9 },
  { .frame = ROW*2 + 10 },
  { .frame = ROW*2 + 11 },
  { .frame = ROW*2 + 12 },
  { .frame = ROW*2 + 13 },
  { .frame = ROW*2 + 14 },
  { .frame = ROW*2 + 15 },
};

static Frame player_jump[] = {
  { .frame = ROW*3 +  0 },
  { .frame = ROW*3 +  1 },
  { .frame = ROW*3 +  2 },
  { .frame = ROW*3 +  3 },
  { .frame = ROW*3 +  4 }, { .frame = ROW*3 +  4 },
  { .frame = ROW*3 +  5 }, { .frame = ROW*3 +  5 },
  { .frame = ROW*3 +  6 }, { .frame = ROW*3 +  6 },
  { .frame = ROW*3 +  7 }, { .frame = ROW*3 +  7 },
  { .frame = ROW*3 +  8 }, { .frame = ROW*3 +  8 },
  { .frame = ROW*3 +  9 }, { .frame = ROW*3 +  9 },
  { .frame = ROW*3 + 10 }, { .frame = ROW*3 + 10 },
  { .frame = ROW*3 + 11 }, { .frame = ROW*3 + 11 },
  { .frame = ROW*3 + 12 }, { .frame = ROW*3 + 12 },
  { .frame = ROW*3 + 13 }, { .frame = ROW*3 + 13 },
  { .frame = ROW*3 + 14 }, { .frame = ROW*3 + 14 },
  { .frame = ROW*3 + 15 }, { .frame = ROW*3 + 15 },
};

static Frame player_fall[] = {
  { .frame = ROW*3 + 6 },
  { .frame = ROW*3 + 7 },
  { .frame = ROW*3 + 8 },
  { .frame = ROW*3 + 9 },
  { .frame = ROW*3 + 10 },
  { .frame = ROW*3 + 11 },
  { .frame = ROW*3 + 12 },
  { .frame = ROW*3 + 13 },
  { .frame = ROW*3 + 14 },
  { .frame = ROW*3 + 15 },
};

static Frame player_jump_double[] = {
  { .frame = ROW*4 + 0 },
  { .frame = ROW*4 + 1 },
  { .frame = ROW*4 + 2 },
  { .frame = ROW*4 + 3 }, { .frame = ROW*4 + 3 },
  { .frame = ROW*4 + 4 }, { .frame = ROW*4 + 4 },
  { .frame = ROW*4 + 5 }, { .frame = ROW*4 + 5 },
  { .frame = ROW*4 + 6 }, { .frame = ROW*4 + 6 },
  { .frame = ROW*4 + 7 }, { .frame = ROW*4 + 7 },
  { .frame = ROW*4 + 8 }, { .frame = ROW*4 + 8 },
  { .frame = ROW*4 + 9 }, { .frame = ROW*4 + 9 },
  { .frame = ROW*5 + 0 }, { .frame = ROW*5 + 0 },
  { .frame = ROW*5 + 1 }, { .frame = ROW*5 + 1 },
  { .frame = ROW*5 + 2 }, { .frame = ROW*5 + 2 },
  { .frame = ROW*5 + 3 }, { .frame = ROW*5 + 3 },
  { .frame = ROW*5 + 4 }, { .frame = ROW*5 + 4 },
  { .frame = ROW*5 + 5 }, { .frame = ROW*5 + 5 },
  { .frame = ROW*5 + 6 }, { .frame = ROW*5 + 6 },
  { .frame = ROW*5 + 7 }, { .frame = ROW*5 + 7 },
  { .frame = ROW*5 + 8 }, { .frame = ROW*5 + 8 },
  { .frame = ROW*5 + 9 }, { .frame = ROW*5 + 9 },
};

static Frame player_jump_double_reverse[] = {
  { .frame = ROW*6 + 9 },
  { .frame = ROW*6 + 8 },
  { .frame = ROW*6 + 7 },
  { .frame = ROW*6 + 6 }, { .frame = ROW*6 + 6 },
  { .frame = ROW*6 + 5 }, { .frame = ROW*6 + 5 },
  { .frame = ROW*6 + 4 }, { .frame = ROW*6 + 4 },
  { .frame = ROW*6 + 3 }, { .frame = ROW*6 + 3 },
  { .frame = ROW*6 + 2 }, { .frame = ROW*6 + 2 },
  { .frame = ROW*6 + 1 }, { .frame = ROW*6 + 1 },
  { .frame = ROW*6 + 0 }, { .frame = ROW*6 + 0 },
  { .frame = ROW*5 + 0 }, { .frame = ROW*5 + 0 },
  { .frame = ROW*5 + 1 }, { .frame = ROW*5 + 1 },
  { .frame = ROW*5 + 2 }, { .frame = ROW*5 + 2 },
  { .frame = ROW*5 + 3 }, { .frame = ROW*5 + 3 },
  { .frame = ROW*5 + 4 }, { .frame = ROW*5 + 4 },
  { .frame = ROW*5 + 5 }, { .frame = ROW*5 + 5 },
  { .frame = ROW*5 + 6 }, { .frame = ROW*5 + 6 },
  { .frame = ROW*5 + 7 }, { .frame = ROW*5 + 7 },
  { .frame = ROW*5 + 8 }, { .frame = ROW*5 + 8 },
  { .frame = ROW*5 + 9 }, { .frame = ROW*5 + 9 },
};

static Frame player_jump_double_reverse_2[] = {
  { .frame = ROW*6 +  9 },
  { .frame = ROW*6 +  8 },
  { .frame = ROW*6 +  7 },
  { .frame = ROW*6 +  6 }, { .frame = ROW*6 +  6 },
  { .frame = ROW*6 +  5 }, { .frame = ROW*6 +  5 },
  { .frame = ROW*6 +  4 }, { .frame = ROW*6 +  4 },
  { .frame = ROW*6 +  3 }, { .frame = ROW*6 +  3 },
  { .frame = ROW*7 +  2 }, { .frame = ROW*7 +  2 },
  { .frame = ROW*7 +  1 }, { .frame = ROW*7 +  1 },
  { .frame = ROW*7 +  0 }, { .frame = ROW*7 +  0 },
  { .frame = ROW*3 +  6 }, { .frame = ROW*3 +  6 },
  { .frame = ROW*3 +  7 }, { .frame = ROW*3 +  7 },
  { .frame = ROW*3 +  8 }, { .frame = ROW*3 +  8 },
  { .frame = ROW*3 +  9 }, { .frame = ROW*3 +  9 },
  { .frame = ROW*3 + 10 }, { .frame = ROW*3 + 10 },
  { .frame = ROW*3 + 11 }, { .frame = ROW*3 + 11 },
  { .frame = ROW*3 + 12 }, { .frame = ROW*3 + 12 },
  { .frame = ROW*3 + 13 }, { .frame = ROW*3 + 13 },
  { .frame = ROW*3 + 14 }, { .frame = ROW*3 + 14 },
  { .frame = ROW*3 + 15 }, { .frame = ROW*3 + 15 },
};

static Frame player_roll_into_run[] = {
  { .frame = ROW*8 +  0 },
  { .frame = ROW*8 +  1 },
  { .frame = ROW*8 +  2 },
  { .frame = ROW*8 +  3 },
  { .frame = ROW*8 +  4 },
  { .frame = ROW*8 +  5 },
  { .frame = ROW*8 +  6 },
  { .frame = ROW*8 +  7 },
  { .frame = ROW*8 +  8 },
  { .frame = ROW*8 +  9 },
  { .frame = ROW*8 + 10 },
  { .frame = ROW*8 + 11 },
  { .frame = ROW*8 + 12 },
  { .frame = ROW*8 + 13 },
  { .frame = ROW*8 + 14 },
  { .frame = ROW*8 + 15 },
  { .frame = ROW*2 +  0 },
  { .frame = ROW*2 +  1 },
  { .frame = ROW*2 +  2 },
  { .frame = ROW*2 +  3 },
  { .frame = ROW*2 +  4 },
  { .frame = ROW*2 +  5 },
  { .frame = ROW*2 +  6 },
  { .frame = ROW*2 +  7 },
  { .frame = ROW*2 +  8 },
  { .frame = ROW*2 +  9 },
  { .frame = ROW*2 + 10 },
  { .frame = ROW*2 + 11 },
  { .frame = ROW*2 + 12 },
  { .frame = ROW*2 + 13 },
  { .frame = ROW*2 + 14 },
  { .frame = ROW*2 + 15 },
};

static Frame player_bump_into_wall[] = {
  { .frame = ROW*4 + 10 },
  { .frame = ROW*4 + 11 },
  { .frame = ROW*4 + 12 },
  { .frame = ROW*4 + 13 },
  { .frame = ROW*4 + 14 },
  { .frame = ROW*4 + 15 },
  { .frame = ROW*0 + 0 },
  { .frame = ROW*0 + 1 },
  { .frame = ROW*0 + 2 },
  { .frame = ROW*0 + 3 },
  { .frame = ROW*0 + 4 },
  { .frame = ROW*0 + 5 },
  { .frame = ROW*0 + 6 },
  { .frame = ROW*0 + 7 },
  { .frame = ROW*0 + 8 },
  { .frame = ROW*0 + 9 },
  { .frame = ROW*0 + 10 },
  { .frame = ROW*0 + 11 },
  { .frame = ROW*0 + 12 },
  { .frame = ROW*0 + 13 },
  { .frame = ROW*0 + 14 },
  { .frame = ROW*0 + 15 },
};

static Frame player_warp_standing[] = {
  { .frame = ROW*9 +  0 },
  { .frame = ROW*9 +  1 }, { .frame = ROW*9 +  1 },
  { .frame = ROW*9 +  2 }, { .frame = ROW*9 +  2 }, { .frame = ROW*9 +  2 },
  { .frame = ROW*9 +  3 }, { .frame = ROW*9 +  3 }, { .frame = ROW*9 +  3 },
  { .frame = ROW*9 +  4 }, { .frame = ROW*9 +  4 }, { .frame = ROW*9 +  4 },
  { .frame = ROW*9 +  5 }, { .frame = ROW*9 +  5 }, { .frame = ROW*9 +  5 },
  { .frame = ROW*9 +  6 }, { .frame = ROW*9 +  6 },
  { .frame = ROW*9 +  7 }, { .frame = ROW*9 +  7 },
  { .frame = ROW*9 +  8 },
  { .frame = ROW*9 +  9 },

  { .frame = ROW*7 +  8 },
  { .frame = ROW*7 +  9 },
  { .frame = ROW*7 + 10 },
  { .frame = ROW*7 + 11 },
  { .frame = ROW*7 + 12 },
  { .frame = ROW*7 + 13 },
  { .frame = ROW*7 + 14 },
  { .frame = ROW*7 + 15 },
  { .frame = ROW*6 + 10 },
  { .frame = ROW*6 + 11 },
  { .frame = ROW*6 + 12 },
  { .frame = ROW*6 + 13 },
  { .frame = ROW*6 + 14 },
  { .frame = ROW*6 + 15 },
};

static Frame player_warp_standing_fade[] = {
  { .frame = ROW*9 +  0 },
  { .frame = ROW*9 +  1 },
  { .frame = ROW*9 +  2 },
  { .frame = ROW*9 +  3 }, { .frame = ROW*9 +  3 },
  { .frame = ROW*9 +  4 }, { .frame = ROW*9 +  4 },
  { .frame = ROW*9 +  5 },
  { .frame = ROW*9 +  6 },
  { .frame = ROW*9 +  7 },
  { .frame = ROW*9 +  8 },
  { .frame = ROW*9 +  9 },

  { .frame = ROW*7 +  8 },
  { .frame = ROW*7 +  9 },
  { .frame = ROW*7 + 10 },
  { .frame = ROW*7 + 11 },
  { .frame = ROW*7 + 12 },
  { .frame = ROW*7 + 13 },
  { .frame = ROW*7 + 14 },
  { .frame = ROW*7 + 15 },
  { .frame = ROW*5 + 10 },
  { .frame = ROW*5 + 11 },
  { .frame = ROW*5 + 12 },
  { .frame = ROW*5 + 13 },
  { .frame = ROW*5 + 14 },
  { .frame = ROW*5 + 15 },
};

static Frame player_warp_air[] = {
  { .frame = ROW*10+  0 },
  { .frame = ROW*10+  1 },
  { .frame = ROW*10+  2 }, { .frame = ROW*10+  2 },
  { .frame = ROW*10+  3 }, { .frame = ROW*10+  3 }, { .frame = ROW*10+  3 },
  { .frame = ROW*10+  4 }, { .frame = ROW*10+  4 }, { .frame = ROW*10+  4 },
  { .frame = ROW*10+  5 }, { .frame = ROW*10+  5 }, { .frame = ROW*10+  5 },
  { .frame = ROW*10+  6 }, { .frame = ROW*10+  6 }, { .frame = ROW*10+  6 },
  { .frame = ROW*10+  7 }, { .frame = ROW*10+  7 },
  { .frame = ROW*10+  8 }, { .frame = ROW*10+  8 },
  { .frame = ROW*10+  9 },

  { .frame = ROW*7 +  8 },
  { .frame = ROW*7 +  9 },
  { .frame = ROW*7 + 10 },
  { .frame = ROW*7 + 11 },
  { .frame = ROW*7 + 12 },
  { .frame = ROW*7 + 13 },
  { .frame = ROW*7 + 14 },
  { .frame = ROW*7 + 15 },
  { .frame = ROW*6 + 10 },
  { .frame = ROW*6 + 11 },
  { .frame = ROW*6 + 12 },
  { .frame = ROW*6 + 13 },
  { .frame = ROW*6 + 14 },
  { .frame = ROW*6 + 15 },
};

static Frame player_warp_air_fade[] = {
  { .frame = ROW*10+  0 },
  { .frame = ROW*10+  1 },
  { .frame = ROW*10+  2 },
  { .frame = ROW*10+  3 },
  { .frame = ROW*10+  4 }, { .frame = ROW*10+  4 },
  { .frame = ROW*10+  5 }, { .frame = ROW*10+  5 },
  { .frame = ROW*10+  6 },
  { .frame = ROW*10+  7 },
  { .frame = ROW*10+  8 },
  { .frame = ROW*10+  9 },

  { .frame = ROW*7 +  8 },
  { .frame = ROW*7 +  9 },
  { .frame = ROW*7 + 10 },
  { .frame = ROW*7 + 11 },
  { .frame = ROW*7 + 12 },
  { .frame = ROW*7 + 13 },
  { .frame = ROW*7 + 14 },
  { .frame = ROW*7 + 15 },
  { .frame = ROW*5 + 10 },
  { .frame = ROW*5 + 11 },
  { .frame = ROW*5 + 12 },
  { .frame = ROW*5 + 13 },
  { .frame = ROW*5 + 14 },
  { .frame = ROW*5 + 15 },
};

static Frame player_landing[] = {
  { .frame = ROW*7 +  3 },
  { .frame = ROW*7 +  4 },
  { .frame = ROW*7 +  5 },
  { .frame = ROW*7 +  6 },
  { .frame = ROW*7 +  7 },
  { .frame = ROW*1 +  0 },
  { .frame = ROW*0 +  0 },
  { .frame = ROW*0 +  1 },
  { .frame = ROW*0 +  2 },
  { .frame = ROW*0 +  3 },
  { .frame = ROW*0 +  4 },
  { .frame = ROW*0 +  5 },
  { .frame = ROW*0 +  6 },
  { .frame = ROW*0 +  7 },
  { .frame = ROW*0 +  8 },
  { .frame = ROW*0 +  9 },
  { .frame = ROW*0 + 10 },
  { .frame = ROW*0 + 11 },
  { .frame = ROW*0 + 12 },
  { .frame = ROW*0 + 13 },
  { .frame = ROW*0 + 14 },
  { .frame = ROW*0 + 15 },
};

//static Hitbox hitbox_player_punch = {
//  .pos = (vec2){ 0.5, 0.5 },
//  .ext = (vec2){ 0.5, 1.0 },
//};
//
//static Frame player_punch[] = {
//  { .frame = 15, .hitbox = &hitbox_player_punch },
//};

#define ANIM_SIZE(FRAMES) (sizeof(FRAMES) / sizeof(Frame))
#define FRAMES(F) .frames = F, .count = ANIM_SIZE(F)

Animation player_animations[] = {
  { FRAMES(player_idle), .rate = 6 },
  { FRAMES(player_idle2), .rate = 6, .repeat = 13 },
  { FRAMES(player_walk), .rate = 4 },
  { FRAMES(player_run), .rate = 4 },
  { FRAMES(player_jump), .rate = 2, .repeat = 8 },
  { FRAMES(player_fall), .rate = 4 },
  { FRAMES(player_jump_double), .rate = 2, .repeat = 19 },
  { FRAMES(player_jump_double_reverse), .rate = 2, .repeat = 19 },
  { FRAMES(player_jump_double_reverse_2), .rate = 2, .repeat = 19 },
  { FRAMES(player_roll_into_run), .rate = 4, .repeat = 16 },
  { FRAMES(player_bump_into_wall), .rate = 6, .repeat = 6 },
  { FRAMES(player_warp_standing), .rate = 1, .repeat = -1 },
  { FRAMES(player_warp_standing_fade), .rate = 2, .repeat = -1 },
  { FRAMES(player_warp_air), .rate = 1, .repeat = -1 },
  { FRAMES(player_warp_air_fade), .rate = 2, .repeat = -1 },
  { FRAMES(player_landing), .rate = 4, .repeat = 6},
};

// Used to tell if an animation contains an idle sequence
bool anim_is_idle(uint animation) {
  return animation == ANIMATION_IDLE
  ||     animation == ANIMATION_IDLE_2
  ||     animation == ANIMATION_LAND
  ||     animation == ANIMATION_BUMP_INTO_WALL
  ;
}

// Used to track these animations whose frames map onto each other and
// don't need to have the start frame updated as a result
bool anim_is_ground(uint animation) {
  return animation == ANIMATION_WALK
  ||     animation == ANIMATION_RUN
  ||     animation == ANIMATION_ROLL_INTO_RUN
  ;
}

// Similarly used to determine if the player is in a warp animation
bool anim_is_warp(uint animation) {
  return animation == ANIMATION_WARP_AIR
  ||     animation == ANIMATION_WARP_STANDING
  ;
}
