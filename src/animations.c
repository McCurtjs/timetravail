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
  { FRAMES(player_walk), .rate = 4 },
  { FRAMES(player_run), .rate = 4 },
  { FRAMES(player_jump), .rate = 2, .repeat = 8 },
  { FRAMES(player_fall), .rate = 4 }
};
