#ifndef _GAME_H_
#define _GAME_H_

#include "vec.h"
#include "vector.h"
#include "camera.h"
#include "entity.h"
#include "shader.h"
#include "texture.h"

#include "game/levels.h"

typedef struct Game_Shaders {
  ShaderProgram basic;
  ShaderProgram light;
} Game_Shaders;

typedef struct Game_Models {
  Model color_cube;
  Model gizmo;
  Model grid;
  Model box;
  Model player;
} Game_Models;

typedef struct Game_Textures {
  Texture crate;
  Texture tiles;
  Texture player;
} Game_Textures;

#define game_key_count (8 + LEVEL_COUNT)
#define game_mouse_button_count 3
#define game_button_input_count (game_key_count + game_mouse_button_count)

// TODO: should have a generic buttons poller separate from the keymapping
typedef struct Game_Buttons {
  union {
    int buttons[game_button_input_count];
    struct {
      // keyboard
      union {
        int keys[game_key_count];
        struct {
          int forward;
          int back;
          int left;
          int right;
          int camera_lock;
          int run_replay;
          int kick;

          // level skip buttons
          int level_reload;
          int levels[LEVEL_COUNT];
        };
      };
      // mouse
      union {
        int mouse[game_mouse_button_count];
        struct {
          int lmb;
          int mmb;
          int rmb;
        };
      };
    };
  };
} Game_Buttons;

typedef struct Game_Mouse {
  vec2 pos;
  vec2 move;
} Game_Mouse;

typedef struct Game_Inputs {
  Game_Buttons mapping;
  Game_Buttons pressed;
  Game_Buttons triggered;
  Game_Buttons released;
  Game_Mouse   mouse;
} Game_Inputs;

typedef struct PlayerRef {
  uint start_frame;
  uint end_frame;
  Entity* e;
} PlayerRef;

typedef struct Line {
  vec2 a, b;

  bool droppable;
  bool wall;
  bool bouncy;

  Movement* moving;
} Line;

typedef struct Game {
  vec2i window;
  Camera camera;

  vec3 target;
  vec4 light_pos;

  Game_Inputs input;

  Game_Shaders shaders;
  Game_Models models;
  Game_Textures textures;

  // Vector levels; // list of levels with hot-swapping?
  Vector entities;

  // Game specific shenanigans
  // Probably move all of these to a level class?
  float frame;
  bool reverse_playback;
  bool reverse_triggered;
  float reverse_speed;
  Vector timeguys;
  uint level;

  Line* colliders;
  uint collider_count;

} Game;

void game_init(Game* game);
void game_add_entity(Game* game, const Entity* entity);

void game_update(Game* game, float dt);
void game_render(Game* game);
void game_cleanup(Game* game);

#endif
