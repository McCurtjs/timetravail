#ifndef _ANIMATIONS_H_
#define _ANIMATIONS_H_

#include "types.h"

#include "vec.h"

typedef enum Anims {
  ANIMATION_IDLE,
  ANIMATION_RUN,
  ANIMATION_JUMP,
  ANIMATION_AIR,
  ANIMATION_PUNCH,
  ANIMATION_COUNT
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
