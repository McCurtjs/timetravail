#include "animations.h"

#include "entity.h"

#define ROW 16

static Frame player_idle[] = {
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

static Frame player_idle2[] = {
  // beleaguered sigh
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
  // regular idle anim
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

static Frame player_walk[] = {
  { .frame = ROW*1 +  0 },
  { .frame = ROW*1 +  1 },
  { .frame = ROW*1 +  2 },
  { .frame = ROW*1 +  3 },
  { .frame = ROW*1 +  4 },
  { .frame = ROW*1 +  5 },
  { .frame = ROW*1 +  6 },
  { .frame = ROW*1 +  7 },
  { .frame = ROW*1 +  8 },
  { .frame = ROW*1 +  9 },
  { .frame = ROW*1 + 10 },
  { .frame = ROW*1 + 11 },
  { .frame = ROW*1 + 12 },
  { .frame = ROW*1 + 13 },
  { .frame = ROW*1 + 14 },
  { .frame = ROW*1 + 15 },
};

static Frame player_run[] = {
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

static Frame player_jump[] = {
  // lifdoff
  { .frame = ROW*3 +  0 },
  { .frame = ROW*3 +  1 },
  { .frame = ROW*3 +  2 },
  { .frame = ROW*3 +  3 },
  { .frame = ROW*3 +  4 }, { .frame = ROW*3 +  4 },
  { .frame = ROW*3 +  5 }, { .frame = ROW*3 +  5 },
  // hangtime 1 (leaning slightly forward)
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
  // hangtime 1 (leaning slightly forward)
  { .frame = ROW*3 +  6 },
  { .frame = ROW*3 +  7 },
  { .frame = ROW*3 +  8 },
  { .frame = ROW*3 +  9 },
  { .frame = ROW*3 + 10 },
  { .frame = ROW*3 + 11 },
  { .frame = ROW*3 + 12 },
  { .frame = ROW*3 + 13 },
  { .frame = ROW*3 + 14 },
  { .frame = ROW*3 + 15 },
};

static Frame player_fall_2[] = {
  // hangtime 2 (leaning slightly back)
  { .frame = ROW*5 + 0 },
  { .frame = ROW*5 + 1 },
  { .frame = ROW*5 + 2 },
  { .frame = ROW*5 + 3 },
  { .frame = ROW*5 + 4 },
  { .frame = ROW*5 + 5 },
  { .frame = ROW*5 + 6 },
  { .frame = ROW*5 + 7 },
  { .frame = ROW*5 + 8 },
  { .frame = ROW*5 + 9 },
};

static Frame player_jump_double[] = {
  // jellyfish arm flail
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
  // hangtime 2 (leaning slightly back)
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
  // do a little pirouette (it's backwards because reasons)
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
  // hangtime 2 (leaning slightly back)
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
  // do a little pirouette (it's backwards because reasons)
  { .frame = ROW*6 +  9 },
  { .frame = ROW*6 +  8 },
  { .frame = ROW*6 +  7 },
  { .frame = ROW*6 +  6 }, { .frame = ROW*6 +  6 },
  { .frame = ROW*6 +  5 }, { .frame = ROW*6 +  5 },
  { .frame = ROW*6 +  4 }, { .frame = ROW*6 +  4 },
  { .frame = ROW*6 +  3 }, { .frame = ROW*6 +  3 },
  // last 3 frames set up for other hangtime anim
  { .frame = ROW*7 +  2 }, { .frame = ROW*7 +  2 },
  { .frame = ROW*7 +  1 }, { .frame = ROW*7 +  1 },
  { .frame = ROW*7 +  0 }, { .frame = ROW*7 +  0 },
  // hangtime 1 (leaning slightly forward)
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
  // roll
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
  // run
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
  // bomp
  { .frame = ROW*4 + 10 },
  { .frame = ROW*4 + 11 },
  { .frame = ROW*4 + 12 },
  { .frame = ROW*4 + 13 },
  { .frame = ROW*4 + 14 },
  { .frame = ROW*4 + 15 },
  // idle
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
  // countdown to warp from standing
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
  // warp mid
  { .frame = ROW*7 +  8 },
  { .frame = ROW*7 +  9 },
  { .frame = ROW*7 + 10 },
  { .frame = ROW*7 + 11 },
  { .frame = ROW*7 + 12 },
  { .frame = ROW*7 + 13 },
  { .frame = ROW*7 + 14 },
  { .frame = ROW*7 + 15 },
  // warp tail (active)
  { .frame = ROW*6 + 10 },
  { .frame = ROW*6 + 11 },
  { .frame = ROW*6 + 12 },
  { .frame = ROW*6 + 13 },
  { .frame = ROW*6 + 14 },
  { .frame = ROW*6 + 15 },
};

static Frame player_warp_standing_fade[] = {
  // countdown to warp from standing
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
  // warp mid
  { .frame = ROW*7 +  8 },
  { .frame = ROW*7 +  9 },
  { .frame = ROW*7 + 10 },
  { .frame = ROW*7 + 11 },
  { .frame = ROW*7 + 12 },
  { .frame = ROW*7 + 13 },
  { .frame = ROW*7 + 14 },
  { .frame = ROW*7 + 15 },
  // warp tail (fade)
  { .frame = ROW*5 + 10 },
  { .frame = ROW*5 + 11 },
  { .frame = ROW*5 + 12 },
  { .frame = ROW*5 + 13 },
  { .frame = ROW*5 + 14 },
  { .frame = ROW*5 + 15 },
};

static Frame player_warp_air[] = {
  // countdown to warp while in air
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
  // warp mid
  { .frame = ROW*7 +  8 },
  { .frame = ROW*7 +  9 },
  { .frame = ROW*7 + 10 },
  { .frame = ROW*7 + 11 },
  { .frame = ROW*7 + 12 },
  { .frame = ROW*7 + 13 },
  { .frame = ROW*7 + 14 },
  { .frame = ROW*7 + 15 },
  // warp tail (active)
  { .frame = ROW*6 + 10 },
  { .frame = ROW*6 + 11 },
  { .frame = ROW*6 + 12 },
  { .frame = ROW*6 + 13 },
  { .frame = ROW*6 + 14 },
  { .frame = ROW*6 + 15 },
};

static Frame player_warp_air_fade[] = {
  // countdown to warp while in air
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
  // warp mid
  { .frame = ROW*7 +  8 },
  { .frame = ROW*7 +  9 },
  { .frame = ROW*7 + 10 },
  { .frame = ROW*7 + 11 },
  { .frame = ROW*7 + 12 },
  { .frame = ROW*7 + 13 },
  { .frame = ROW*7 + 14 },
  { .frame = ROW*7 + 15 },
  // warp tail (fading)
  { .frame = ROW*5 + 10 },
  { .frame = ROW*5 + 11 },
  { .frame = ROW*5 + 12 },
  { .frame = ROW*5 + 13 },
  { .frame = ROW*5 + 14 },
  { .frame = ROW*5 + 15 },
};

static Frame player_landing[] = {
  // cronch
  { .frame = ROW*7 +  3 },
  { .frame = ROW*7 +  4 },
  { .frame = ROW*7 +  5 },
  { .frame = ROW*7 +  6 },
  { .frame = ROW*7 +  7 },
  // walk animation
  { .frame = ROW*1 +  0 },
  // idle
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

static Frame player_b_air[] = {
  // woprek! (it's backwards because reasons)
  { .frame = ROW*11+ 13 },
  { .frame = ROW*11+ 12 },
  { .frame = ROW*11+ 11 },
  { .frame = ROW*11+ 10 },
  { .frame = ROW*11+  9 },
  { .frame = ROW*11+  8 },
  { .frame = ROW*11+  7 },
  { .frame = ROW*11+  6, .hitbox = HITBOX_BAIR },
  { .frame = ROW*11+  5, .hitbox = HITBOX_BAIR2 },
  { .frame = ROW*11+  4 },
  { .frame = ROW*11+  3 },
  { .frame = ROW*11+  2 },
  { .frame = ROW*11+  1 },
  { .frame = ROW*11+  0 },
};

static Frame player_f_air[] = {
  // kerpow!
  { .frame = ROW*12+  0 },
  { .frame = ROW*12+  1 },
  { .frame = ROW*12+  2 },
  { .frame = ROW*12+  3 },
  { .frame = ROW*12+  4, .hitbox = HITBOX_FAIR },
  { .frame = ROW*12+  5, .hitbox = HITBOX_FAIR },
  { .frame = ROW*12+  6, .hitbox = HITBOX_FAIR },
  { .frame = ROW*12+  7 },
  { .frame = ROW*12+  8 },
  { .frame = ROW*12+  9 },
  { .frame = ROW*12+ 10 },
};

static Frame player_n_air[] = {
  // klonk!
  { .frame = ROW*13+  0 },
  { .frame = ROW*13+  1 },
  { .frame = ROW*13+  2 },
  { .frame = ROW*13+  3, .hitbox = HITBOX_NAIR },
  { .frame = ROW*13+  4, .hitbox = HITBOX_NAIR },
  { .frame = ROW*13+  5, .hitbox = HITBOX_NAIR },
  { .frame = ROW*13+  6 },
  { .frame = ROW*13+  7 },
  { .frame = ROW*13+  8 },
  { .frame = ROW*13+  9 },
  { .frame = ROW*13+ 10 },
};

static Frame player_kick[] = {
  // bonk!
  { .frame = ROW*14+  0 },
  { .frame = ROW*14+  1 },
  { .frame = ROW*14+  2 },
  { .frame = ROW*14+  3, .hitbox = HITBOX_KICK },
  { .frame = ROW*14+  4, .hitbox = HITBOX_KICK },
  { .frame = ROW*14+  5, .hitbox = HITBOX_KICK },
  { .frame = ROW*14+  6 },
  { .frame = ROW*14+  7 },
  { .frame = ROW*14+  8 },
  { .frame = ROW*14+  9 },
  { .frame = ROW*14+ 10 },
};

static Frame player_kick_run[] = {
  // donk!
  { .frame = ROW*12+ 11 },
  { .frame = ROW*12+ 12 },
  { .frame = ROW*12+ 13 },
  { .frame = ROW*12+ 14, .hitbox = HITBOX_DASH },
  { .frame = ROW*12+ 15, .hitbox = HITBOX_DASH },
  { .frame = ROW*11+ 14, .hitbox = HITBOX_DASH },
  { .frame = ROW*11+ 15 },
};

#define ANIM_SIZE(FRAMES) (sizeof(FRAMES) / sizeof(Frame))
#define FRAMES(F) .frames = F, .count = ANIM_SIZE(F)

Animation player_animations[] = {
  { FRAMES(player_idle), .rate = 6 },
  { FRAMES(player_idle2), .rate = 6, .repeat = 12 },
  { FRAMES(player_walk), .rate = 4 },
  { FRAMES(player_run), .rate = 4 },
  { FRAMES(player_jump), .rate = 2, .repeat = 8 },
  { FRAMES(player_fall), .rate = 4 },
  { FRAMES(player_fall_2), .rate = 4 },
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
  { FRAMES(player_n_air), .rate = 3, .repeat = -1},
  { FRAMES(player_f_air), .rate = 3, .repeat = -1},
  { FRAMES(player_b_air), .rate = 3, .repeat = -1},
  { FRAMES(player_kick), .rate = 3, .repeat = -1},
  { FRAMES(player_kick_run), .rate = 3, .repeat = -1},
};

Hitbox player_hitboxes[7] = {
  { }, // placeholder - index 0 is no hitbox
  //   center          radius   knockback   hitstun
  { .pos = { 0.75f, 0.2f }, .radius = .5f, .knockback = { 25.f, 12.f }, .hitstun = 12 }, // KICK
  { .pos = { 0.75f, 0.2f }, .radius = .5f, .knockback = { 15.f, 38.f }, .hitstun = 25 }, // DASH
  { .pos = { 0.75f, 0.2f }, .radius = .5f, .knockback = {  5.f, 30.f }, .hitstun = 20 }, // NAIR
  { .pos = { 0.90f, 0.0f }, .radius = .7f, .knockback = { 40.f, 20.f }, .hitstun = 35 }, // FAIR
  { .pos = { 0.90f,-0.1f }, .radius = .3f, .knockback = { 60.f, 25.f }, .hitstun = 120}, // BAIR sweetspot
  { .pos = { 0.90f, 0.3f }, .radius = .8f, .knockback = {  8.f, 20.f }, .hitstun = 30 }, // BAIR sourspot
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
  ||     animation == ANIMATION_WARP_AIR_FADE
  ||     animation == ANIMATION_WARP_STANDING_FADE
  ;
}

// Double jump animations
bool anim_is_double_jump(uint animation) {
  return animation == ANIMATION_DOUBLE_JUMP
  ||     animation == ANIMATION_DOUBLE_JUMP_REVERSE
  ||     animation == ANIMATION_DOUBLE_JUMP_REVERSE_2
  ;
}

// These are attacks!
bool anim_is_attack(uint animation) {
  return animation == ANIMATION_N_AIR
  ||     animation == ANIMATION_F_AIR
  ||     animation == ANIMATION_B_AIR
  ||     animation == ANIMATION_KICK
  ||     animation == ANIMATION_KICK_RUN
  ;
}

// Hangtime 2 is the one where you lean back slightly, and leads into
// the roll animation on landing into run
bool anim_is_hangtime_2(uint animation) {
  return animation == ANIMATION_DOUBLE_JUMP
  ||     animation == ANIMATION_DOUBLE_JUMP_REVERSE
  ||     animation == ANIMATION_N_AIR
  ||     animation == ANIMATION_F_AIR
  ||     animation == ANIMATION_B_AIR
  ||     animation == ANIMATION_FALL_2
  ;
}

// Returns true if the given animation is finished before this frame
// If the animation repeats, returns true if it's passed the intro
// The frame passed in is real-time frame (ie, 2 images at rate 3 is 6 frames)
bool anim_finished(uint animation, uint frame) {
  Animation* anim = &player_animations[animation];
  if (anim->repeat > 0)
    return frame / anim->rate >= (uint)anim->repeat;
  return frame / anim->rate >= anim->count - anim->repeat;
}

uint anim_frame_index(uint animation, uint time_playing) {
  Animation* a = &player_animations[animation];

  // non-repeating frame, hangs on last frame.
  if (a->repeat < 0)
    return MIN(time_playing / a->rate, a->count - 1);

  // regular animation where whole animation loops, or where we haven't gotten
  // to the loop point on a delayed loop
  if (a->repeat == 0 || time_playing < (uint)a->repeat * a->rate)
    return (time_playing / a->rate) % a->count;

  // case where repeat counter is positive, plays the beginning, loops the rest.
  // basically, pretend the intro didn't exist, and adjust the numbers to clip
  // it out so we're really only using the looping section
  return ((time_playing / a->rate) - a->repeat) // conv to anim time, drop intro
  %      (a->count - a->repeat) // effectively lower the number of frames
  +      a->repeat // add the offset to account for the missing frames
  ;
}

const Frame* anim_frame(uint animation, uint time_playing) {
  Animation* a = &player_animations[animation];
  return &a->frames[anim_frame_index(animation, time_playing)];
}

const Hitbox* anim_hitbox(uint animation, uint time_playing) {
  const Frame* f = anim_frame(animation, time_playing);
  if (f->hitbox == 0) return NULL;
  return &player_hitboxes[f->hitbox];
}

const Animation* anim_current(const Entity* e) {
  return &e->anim_data.animations[e->fd.animation];
}
