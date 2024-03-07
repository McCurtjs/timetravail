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

static Animation player_animations[] = {
  { FRAMES(player_idle), .rate = 4 },
  { FRAMES(player_run), .rate = 4 },
  { FRAMES(player_jump), .rate = 4 },
  { FRAMES(player_air), .rate = 4 },
  { FRAMES(player_punch), .rate = 4 },
};

AnimationData animations_for_player() {
  return (AnimationData) {
    .anim_count = ANIMATION_COUNT,
    .animations = player_animations,
  };
}
