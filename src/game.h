#ifndef _GAME_H_
#define _GAME_H_

#include "vec.h"
#include "camera.h"

typedef struct Game {
  vec2i window;
  Camera camera;
  int button_down;
  vec4 light_pos;
} Game;

#endif
