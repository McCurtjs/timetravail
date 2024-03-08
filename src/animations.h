#ifndef _ANIMATIONS_H_
#define _ANIMATIONS_H_

#include "types.h"

#include "vec.h"

typedef enum Anims {
  ANIMATION_IDLE,
  ANIMATION_WALK,
  ANIMATION_RUN,
  ANIMATION_JUMP,

  ANIMATION_FALL, // uses same hangtime as jump, but a slip when you go off platforms?

  ANIMATION_WARP,
  ANIMATION_WARP_FADE, // for duplicates going back in time

  ANIMATION_DOUBLE_JUMP, // includes different hangtime (arm flaps?)

  ANIMATION_KICK,
  ANIMATION_KICK_AIR,

  ANIMATION_TURN, // slow and fast for walk and run?
  ANIMATION_SKID, // skid to stop/idle

  ANIMATION_LAND, // land into idle
  ANIMATION_LAND_HARD, // land at high velocity
  ANIMATION_LAND_ROLL, // land at high horizontal velocity

  ANIMATION_CRASH, // brace with hands when you run into wall, ends with idle
  ANIMATION_THROW,

  ANIMATION_COUNT // end of list
} Anims;

typedef struct Hitbox {
  vec2 pos, ext;
} Hitbox;

typedef struct Frame {
  byte frame;
  Hitbox* hitbox;
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

  //Hitbox* hitboxes;
  //uint hitbox_count;

} AnimationData;

extern Animation player_animations[];

#endif
