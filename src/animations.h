#ifndef _ANIMATIONS_H_
#define _ANIMATIONS_H_

#include "types.h"

#include "vec.h"

typedef struct Entity Entity;

#define FACING_RIGHT 0
#define FACING_LEFT 1

typedef enum Anims {
  ANIMATION_IDLE,
  ANIMATION_IDLE_2,
  ANIMATION_WALK,
  ANIMATION_RUN,
  ANIMATION_JUMP,
  ANIMATION_FALL, // uses same hangtime as jump, but a slip when you go off platforms?
  ANIMATION_FALL_2, // uses the hangtime 2 animation (the one that rolls)
  ANIMATION_DOUBLE_JUMP, // includes different hangtime (arm flaps?)
  ANIMATION_DOUBLE_JUMP_REVERSE,
  ANIMATION_DOUBLE_JUMP_REVERSE_2,
  ANIMATION_ROLL_INTO_RUN,
  ANIMATION_BUMP_INTO_WALL,
  ANIMATION_WARP_STANDING,
  ANIMATION_WARP_STANDING_FADE, // for duplicates going back in time
  ANIMATION_WARP_AIR,
  ANIMATION_WARP_AIR_FADE,
  ANIMATION_LAND, // land into idle

  ANIMATION_N_AIR,
  ANIMATION_F_AIR,
  ANIMATION_B_AIR,
  ANIMATION_KICK,
  ANIMATION_KICK_RUN,

  ANIMATION_TURN, // slow and fast for walk and run?
  ANIMATION_SKID, // skid to stop/idle

  ANIMATION_THROW,

  ANIMATION_COUNT // end of list
} Anims;

typedef enum Hitboxes {
  HITBOX_NONE,
  HITBOX_KICK,
  HITBOX_DASH,
  HITBOX_NAIR,
  HITBOX_FAIR,
  HITBOX_BAIR,
  HITBOX_BAIR2,
} Hitboxes;

//typedef enum HitboxType {
//  HITBOX_CIRCLE,
//  HITBOX_AABB,
//} HitboxType;

typedef struct Hitbox {
  vec2 pos;
  float radius;
  vec2 knockback;
  uint hitstun;
} Hitbox;

typedef struct Frame {
  byte frame;
  byte hitbox;
} Frame;

typedef struct Animation {
  Frame* frames;
  uint count;
  uint rate;
  int repeat;
} Animation;

typedef struct AnimationData {
  Animation* animations;
  uint anim_count;

  Hitbox* hitboxes;
  //uint hitbox_count;

} AnimationData;

extern Animation player_animations[];
extern Hitbox player_hitboxes[];

bool anim_is_idle(uint animation);
bool anim_is_ground(uint animation);
bool anim_is_warp(uint animation);
bool anim_is_double_jump(uint animation);
bool anim_is_attack(uint animation);
bool anim_is_hangtime_2(uint animation);
bool anim_finished(uint animation, uint frame);
uint anim_frame_index(uint animation, uint time_playing);
const Frame* anim_frame(uint animation, uint time_playing);
const Hitbox* anim_hitbox(uint animation, uint time_playing);
const Animation* anim_current(const Entity* e);

#endif
