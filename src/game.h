#ifndef _GAME_H_
#define _GAME_H_

#include "vec.h"
#include "vector.h"
#include "camera.h"
#include "entity.h"
#include "shader.h"

typedef struct Game_Shaders {
  ShaderProgram basic_shader;
  ShaderProgram light_shader;
} Game_Shaders;

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

  Game_Shaders shaders;

  Vector entities;
} Game;

void game_add_entity(Entity* entity);

void game_update(Game* game, float dt);
void game_render(Game* game);

#endif
