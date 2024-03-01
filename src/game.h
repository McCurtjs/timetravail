#ifndef _GAME_H_
#define _GAME_H_

#include "vec.h"
#include "camera.h"

typedef struct Game {
  vec2i window;
  Camera camera;
  vec3 target;
  int button_down;
  int forward_down;
  int back_down;
  int left_down;
  int right_down;
  vec4 light_pos;
} Game;

#endif
