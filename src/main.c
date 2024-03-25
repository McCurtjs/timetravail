#include "eng/wasm.h"
#include "eng/shader.h"
#include "eng/mat.h"
#include "eng/model.h"
#include "eng/camera.h"
#include "eng/texture.h"
#include "eng/draw.h"
#include "eng/str.h"
#include "utility.h"

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#include "gl.h"
#include <SDL3/SDL.h>

#include "game.h"
#include "system_events.h"
#include "test_behaviors.h"
#include "game/game_behaviors.h"
#include "animations.h"

#include "game/levels.h"

static Game game;

#define GAME_ON 1

#if GAME_ON == 1
// Async loaders
static File file_vert;
static File file_frag;
static File file_model_test;
//static File file_model_level_1;
static File file_model_gear;
static Image image_crate;
//static Image image_level;
static Image image_tiles;
static Image image_brass;
static Image image_anim_test;
#endif

str_static(str_test_1, "|Hello, this is a static string|");
str_static(str_test_2, "|This is another test!|");

void string_tests() {
  print("Type Sizes");
  str_print(str_test_1);
  print_int(str_test_1.length);
  str_print(str_test_2);
  print_int(str_test_2.length);
  str_print(str_literal("This is an inline string literal"));
  str_print(R("This is an inline literal with short macro"));

  String tmp = str_concat(str_test_1, str_test_2);
  str_print(tmp->range);
  print_int(tmp->size);
  print_ptr(tmp);
  print_ptr(tmp->begin);
  str_delete(&tmp);
  print_ptr(tmp);

  Array ranges = array_new(StringRange);
  array_push_back(ranges, &R("First string"));
  array_push_back(ranges, &R("Second range"));
  array_push_back(ranges, &R("Another one!"));
  array_push_back(ranges, &R("And another!"));
  print("Joining strings...");
  tmp = str_join(R("--POTATO--"), ranges);
  str_print(tmp->range);
  print_int(tmp->size);
  str_delete(&tmp);
  array_delete(&ranges);
  print_ptr(ranges);
  print_ptr(tmp);

  String empty = str_new(NULL);
  String tmp1 = str_new("Copying as an actual string");
  String tmp2 = str_copy(R("Copied string"));
  String tmp3 = str_new("third string");
  StringRange* tmp4 = &R("Sneaky range pointer");

  print_ptr(tmp1);

  ranges = array_new(String);
  array_push_back(ranges, &tmp1);
  array_push_back(ranges, &tmp2);
  array_push_back(ranges, &empty);
  array_push_back(ranges, &tmp3);
  array_push_back(ranges, &tmp4);
  print("Joining strings...");
  tmp = str_join(R(" +++ "), ranges);
  str_print(tmp->range);
  print_int(tmp->size);
  str_delete(&tmp);
  str_delete(&tmp1);
  str_delete(&tmp2);
  str_delete(&tmp3);
  print_ptr(empty);
  str_delete(&empty);
  print_ptr(empty);
  array_delete(&ranges);

  str_print(str_substring(str_test_1, 1, 6));
  str_print(str_substring(R("xHELLOyz"), 1, 6));
  str_print(str_substring(R("xNO"), 1, 6));

  str_static(str_test_sub, "Static string for substr tests");
  str_print(str_substring(str_test_sub, 7));
  str_print(str_substring(str_test_sub, 7, -6));
  str_print(str_substring(str_test_sub, -5));
  str_print(str_substring(str_test_sub, 0, 13));
  str_print(str_substring(str_test_sub, str_test_sub.size, 0)); // empty

  print(str_eq(R("Is eq?"), R("Not eq")) ? "True" : "False");
  print(str_eq(R("Is eq?"), R("Is eq?")) ? "True" : "False");

  tmp = str_concat(R("Starts with 'Static': "), str_starts_with(str_test_sub, R("Static")) ? R("True") : R("False"));
  str_print(tmp->range);
  str_delete(&tmp);

  tmp = str_concat(R("Starts with 'Statics': "), str_starts_with(str_test_sub, R("Statics")) ? R("True") : R("False"));
  str_print(tmp->range);
  str_delete(&tmp);

  tmp = str_concat(R("Ends with 'Static': "), str_ends_with(str_test_sub, R("Static")) ? R("True") : R("False"));
  str_print(tmp->range);
  str_delete(&tmp);

  tmp = str_concat(R("Ends with 'tests': "), str_ends_with(str_test_sub, R("tests")) ? R("True") : R("False"));
  str_print(tmp->range);
  str_delete(&tmp);

  str_print(str_substring(str_test_sub, str_find(str_test_sub, R("for"))));
  str_print(str_substring(str_test_sub, str_find(str_test_sub, R("tests"))));
  str_print(str_substring(str_test_sub, str_find(str_test_sub, R("nonsense"))));
  str_print(str_substring(str_test_sub, str_find(str_test_sub, R(""))));

  print("Finding all substrs starting with 'st'");
  size_t tracker = 0;
  loop {
    tracker = str_index_of(str_test_sub, R("st"), tracker);
    until (tracker >= str_test_sub.size);
    str_print(str_substring(str_test_sub, tracker++, 0));
  }
  print("End of index checks");

  print("Split strings from array: (space)");
  Array substr_splits = str_split(str_test_sub, R(" "));
  for (uint i = 0; i < substr_splits->size; ++i) {
    StringRange* range = array_get(substr_splits, i);
    str_print(*range);
  }
  array_delete(&substr_splits);
  print("End of array");

  print("Split strings from array: (st)");
  substr_splits = str_split(str_test_sub, R("st"));
  for (uint i = 0; i < substr_splits->size; ++i) {
    StringRange* range = array_get(substr_splits, i);
    str_print(*range);
  }
  array_delete(&substr_splits);
  print("End of array");

  ranges = array_new(StringRange);
  StringRange trimmed_both = str_trim(R("  \t   String   needing trim    "));
  StringRange trimmed_front = str_trim_start(R("  \t   String   needing trim    "));
  StringRange trimmed_back = str_trim_end(R("  \t   String   needing trim    "));
  array_push_back(ranges, &R(""));
  array_push_back(ranges, &trimmed_front);
  array_push_back(ranges, &trimmed_back);
  array_push_back(ranges, &trimmed_both);
  array_push_back(ranges, &R(""));
  tmp = str_join(R("-"), ranges);
  str_print(tmp->range);
  str_delete(&tmp);
  array_delete(&ranges);
  str_print(str_trim(R("     This string needs trimmed!   ")));

  str_print(str_substring(R("Overload?"), 2));
  str_print(str_substring(R("Overload?"), 2, 0));

  tmp = str_from_bool(1);
  str_print(tmp->range);
  print_ptr(tmp);
  str_delete(&tmp);
  tmp = str_from_bool(0);
  str_print(tmp->range);
  print_ptr(tmp);
  str_delete(&tmp);
  tmp = str_from_bool(2);
  str_print(tmp->range);
  print_ptr(tmp);
  str_delete(&tmp);

  String num = str_from_int(132);
  tmp = str_concat(R("String converted from int: "), num->range);
  str_print(tmp->range);
  str_delete(&tmp);
  str_delete(&num);

  num = str_from_float(34.29f);
  tmp = str_concat(R("String converted from float: "), num->range);
  str_print(tmp->range);
  str_delete(&tmp);
  str_delete(&num);
}

void export(wasm_preload) (uint w, uint h) {
  #if GAME_ON == 1
  file_open_async(&file_vert, "./res/shaders/basic.vert");
  file_open_async(&file_frag, "./res/shaders/basic.frag");
  file_open_async(&file_model_test, "./res/models/test.obj");
  //file_open_async(&file_model_level_1, "./res/models/level_1.obj");
  file_open_async(&file_model_gear, "./res/models/gear.obj");

  image_open_async(&image_crate, "./res/textures/crate.png");
  //image_open_async(&image_level, "./res/textures/levels.jpg");
  image_open_async(&image_brass, "./res/textures/brass.jpg");
  image_open_async(&image_tiles, "./res/textures/tiles.png");
  image_open_async(&image_anim_test, "./res/textures/spritesheet.png");
  #endif

  vec2i windim = {w, h};
  game = (Game){
    .window = windim,
    .camera = {
      .pos = (vec4){0, 0, 60, 1},
      .front = v4front,
      .up = v4y,
      .persp = {d2r(20), i2aspect((vec2i){w, h}), 0.1f, 500}
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

  Array array = array_new(uint);
  print("Array tests");
  print_int(array->element_size);
  print_int(array->size);
  array_push_back(array, &(uint){5});
  print_int(array->size);
  uint i = *(uint*)array_get(array, 0);
  print_int(i);
  array_delete(&array);
}

static void cheesy_loading_animation(float dt) {
  static float cubespin = 0;
  mat4 projview = camera_projection_view(&game.camera);

  shader_program_use(&game.shaders.basic);
  int projViewMod_loc = game.shaders.basic.uniform.projViewMod;

  mat4 model = m4translation((vec3){0, 0, 0});
  model = m4mul(model, m4rotation(v3norm((vec3){1.f, 1.5f, -.7f}), cubespin));
  model = m4mul(model, m4rotation(v3norm((vec3){-4.f, 1.5f, 1.f}), cubespin/3.6f));

  glUniformMatrix4fv(projViewMod_loc, 1, 0, m4mul(projview, model).f);
  model_render(&game.models.color_cube);
  cubespin += 2 * dt;
}

int export(wasm_load) (int await_count, float dt) {
  if (await_count || !GAME_ON) {
    cheesy_loading_animation(dt);
    return 0;
  };

  #if GAME_ON == 1

  // Build shaders from async data
  Shader light_vert, light_frag;
  shader_build_from_file(&light_vert, &file_vert);
  shader_build_from_file(&light_frag, &file_frag);

  model_load_obj(&game.models.level_test, &file_model_test);
  model_load_obj(&game.models.gear, &file_model_gear);
  //model_load_obj(&game.models.level_1, &file_model_level_1);

  shader_program_build(&game.shaders.light, &light_vert, &light_frag);
  shader_program_load_uniforms(&game.shaders.light, UNIFORMS_PHONG);

  // Build textures from async data
  texture_build_from_image(&game.textures.crate, &image_crate);
  //texture_build_from_image(&game.textures.level, &image_level);
  texture_build_from_image(&game.textures.brass, &image_brass);
  texture_build_from_image(&game.textures.tiles, &image_tiles);
  texture_build_from_image(&game.textures.player, &image_anim_test);

  // Delete async loaded resources
  file_delete(&file_vert);
  file_delete(&file_frag);
  file_delete(&file_model_test);
  file_delete(&file_model_gear);
  //file_delete(&file_model_level_1);
  //image_delete(&image_level);
  image_delete(&image_crate);
  image_delete(&image_brass);
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
  model_build(&game.models.level_test);
  //model_build(&game.models.level_1);
  model_build(&game.models.gear);
  model_grid_set_default(&game.models.gizmo, -2);
  game.models.box.type = MODEL_CUBE;
  model_build(&game.models.box);
  model_build(&game.models.grid);
  model_build(&game.models.gizmo);

  // Load the first game level
  level_switch(&game, game.level);

  #endif

  string_tests();

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


#ifndef __WASM__

int main(int argc, char* argv[]) {
  print("Hello, world!");

  string_tests();

  return 0;
}

#endif

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
