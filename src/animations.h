#ifndef _ANIMATION_H_
#define _ANIMATION_H_

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
} Animation;

typedef struct AnimationData {
  Animation* animations;
  uint anim_count;

  //Hitbox* hitboxes;
  //uint hitbox_count;

} AnimationData;

AnimationData animations_for_player();

#endif
