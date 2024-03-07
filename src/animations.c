#include "animations.h"

static Frame player_idle[] = {
  { .frame = 0 },
  { .frame = 1 },
  { .frame = 2 },
  { .frame = 3 },
  { .frame = 4 },
  { .frame = 5 },
};

static Frame player_run[] = {
  { .frame = 6  },
  { .frame = 7  },
  { .frame = 8  },
  { .frame = 9  },
  { .frame = 10 },
};

static Frame player_jump[] = {
  { .frame = 11 },
  { .frame = 12 },
  { .frame = 13 },
  { .frame = 14 },
};

static Frame player_air[] = {
  { .frame = 13 },
  { .frame = 14 },
};

static Hitbox hitbox_player_punch = {
  .pos = (vec2){ 0.5, 0.5 },
  .ext = (vec2){ 0.5, 1.0 },
};

static Frame player_punch[] = {
  { .frame = 15, .hitbox = &hitbox_player_punch },
};

#define ANIM_SIZE(FRAMES) (sizeof(FRAMES) / sizeof(Frame))
#define FRAMES(F) .frames = F, .count = ANIM_SIZE(F)

Animation player_animations[] = {
  { FRAMES(player_idle), .rate = 12 },
  { FRAMES(player_run), .rate = 8 },
  { FRAMES(player_jump), .rate = 8, .repeat = 2 },
  { FRAMES(player_air), .rate = 6 },
  { FRAMES(player_punch), .rate = 4 },
};
