#include "eng/wasm.h"
#include "eng/shader.h"
#include "eng/mat.h"
#include "eng/model.h"
#include "eng/camera.h"
#include "eng/texture.h"
#include "eng/draw.h"

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#include "GL/gl.h"
#include "SDL/sdl.h"

#include "game.h"
#include "system_events.h"
#include "test_behaviors.h"
#include "game/game_behaviors.h"
#include "animations.h"

#include "game/levels.h"

static Game game;

// Async loaders
static File file_vert;
static File file_frag;
static Image image_crate;
static Image image_tiles;
static Image image_anim_test;

void export(wasm_preload) (uint w, uint h) {
  file_open_async(&file_vert, "./res/shaders/basic.vert");
  file_open_async(&file_frag, "./res/shaders/basic.frag");

  image_open_async(&image_crate, "./res/textures/crate.png");
  image_open_async(&image_tiles, "./res/textures/tiles.png");
  image_open_async(&image_anim_test, "./res/textures/spritesheet.png");

  vec2i windim = {w, h};
  game = (Game){
    .window = windim,
    .camera = {
      .pos = (vec4){0, 0, 60, 1},
      .front = v4front,
      .up = v4y,
      .persp = {d2r(20), i2aspect((vec2i){w, h}), 0.1, 100}
      //.ortho = {-6 * i2aspect(windim), 6 * i2aspect(windim), 6, -6, 0.1, 500}
    },
    .target = v3zero,
    .light_pos = (vec4){4, 3, 5, 1},
    .input.mapping.keys = {'w', 's', 'a', 'd', 'c', 'r',
      /* // Attack button, useful on F for testing
      'f',
      /*/
      SDLK_LEFT, // kick button
      //*/
      16, // shift keky, for editor
      'p', // restart level
      '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', // level select
      SDLK_MINUS, SDLK_EQUALS, // editor levels
      SDLK_HOME // go home
    },
    .level = 0,
  };
  game_init(&game);
  camera_build_perspective(&game.camera);
  //camera_build_orthographic(&game.camera);

  // load this first since it's the loading screen spinner
  game.models.color_cube.type = MODEL_CUBE_COLOR;
  model_build(&game.models.color_cube);

  shader_program_build_basic(&game.shaders.basic);
}

static void cheesy_loading_animation(float dt) {
  static float cubespin = 0;
  mat4 projview = camera_projection_view(&game.camera);

  shader_program_use(&game.shaders.basic);
  int projViewMod_loc = game.shaders.basic.uniform.projViewMod;

  mat4 model = m4translation((vec3){0, 0, 0});
  model = m4mul(model, m4rotation(v3norm((vec3){1, 1.5, -.7}), cubespin));
  model = m4mul(model, m4rotation(v3norm((vec3){-4, 1.5, 1}), cubespin/3.6));

  glUniformMatrix4fv(projViewMod_loc, 1, 0, m4mul(projview, model).f);
  model_render(&game.models.color_cube);
  cubespin += 2 * dt;
}

int export(wasm_load) (int await_count, float dt) {
  if (await_count) {
    cheesy_loading_animation(dt);
    return 0;
  };

  // Build shaders from async data
  Shader light_vert, light_frag;
  shader_build_from_file(&light_vert, &file_vert);
  shader_build_from_file(&light_frag, &file_frag);

  shader_program_build(&game.shaders.light, &light_vert, &light_frag);
  shader_program_load_uniforms(&game.shaders.light, UNIFORMS_PHONG);

  // Build textures from async data
  texture_build_from_image(&game.textures.crate, &image_crate);
  texture_build_from_image(&game.textures.tiles, &image_tiles);
  texture_build_from_image(&game.textures.player, &image_anim_test);

  // Delete async loaded resources
  file_delete(&file_vert);
  file_delete(&file_frag);
  image_delete(&image_crate);
  image_delete(&image_tiles);
  image_delete(&image_anim_test);

  // Set up game models
  game.models.grid.grid = (Model_Grid) {
    .type = MODEL_GRID,
    .basis = {v3x, v3y, v3z},
    .primary = {0, 1},
    .extent = 100
  };

  game.models.player.sprites = (Model_Sprites) {
    .type = MODEL_SPRITES,
    .grid = {.w = 16, .h = 16},
  };

  model_build(&game.models.player);
  model_grid_set_default(&game.models.gizmo, -2);
  game.models.box.type = MODEL_CUBE;
  model_build(&game.models.box);
  model_build(&game.models.grid);
  model_build(&game.models.gizmo);

  // Load the first game level
  level_switch(&game, game.level);

  return 1;
}

void export(wasm_update) (float dt) {
  process_system_events(&game);
  level_switch_check(&game);
  game_update(&game, dt);
}

void export(wasm_render) () {
  game_render(&game);
}

// todo:
/*
instance organizing?
- for each shader { for each model { for each texture { draw instances } } }
- can an entity have multiple registered transforms
Game: {
  Shaders {
    a: {
      Models: {
        a: {
          Textures: {
            a: {
              Transforms: {
                a, b, c...
              }
            },
            b: { ... }
          }
        },
        b: { ... }
      }
    },
    b: { ... }
  }
}

*/
// make spritemaster to batch together sprites
// atlas'd and batched 2d sprite animation
// line intersections, 2d physics for player movement
// - point-based player position? use line segment to detect collisions
//   with other lines. Attach to line segment when "at rest", connect segments
//   together like a linked list
// load model from file
// instances
// fix camera rotation on poles in non-local orbit and rotation mode
