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

#include "GL/gl.h"
#include "SDL/sdl.h"

#include "game.h"
#include "system_events.h"
#include "test_behaviors.h"
#include "game/player_behavior.h"

static Game game;

// Async loaders
static File file_vert;
static File file_frag;
static Image image_crate;
static Image image_tiles;

void export(wasm_preload) (uint w, uint h) {
  file_open_async(&file_vert, "./res/shaders/basic.vert");
  file_open_async(&file_frag, "./res/shaders/basic.frag");

  image_open_async(&image_crate, "./res/textures/crate.png");
  image_open_async(&image_tiles, "./res/textures/tiles.png");

  vec2i windim = {w, h};
  game = (Game){
    .window = windim,
    .camera = {
      .pos = (vec4){0, 0, 60, 1},
      .front = v4front,
      .up = v4y,
      .persp = {d2r(20), v2iaspect((vec2i){w, h}), 0.1, 100}
      //.ortho = {-6 * v2iaspect(windim), 6 * v2iaspect(windim), 6, -6, 0.1, 500}
    },
    .target = v3zero,
    .light_pos = (vec4){4, 3, 5, 1},
    .input.mapping.keys = {'w', 's', 'a', 'd', 'c', 'r'},
    .entities = {
      .data = NULL
    }
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

  // Delete async loaded resources
  file_delete(&file_vert);
  file_delete(&file_frag);
  image_delete(&image_crate);
  image_delete(&image_tiles);

  // Set up game models
  game.models.grid.grid = (Model_Grid) {
    .type = MODEL_GRID,
    .basis = {v3x, v3y, v3z},
    .primary = {0, 1},
    .extent = 100
  };
  model_setup_default_grid(&game.models.gizmo, -2);
  game.models.box.type = MODEL_CUBE;
  model_build(&game.models.box);
  model_build(&game.models.grid);
  model_build(&game.models.gizmo);

  // Establish game entities

  // Debug Renderer
  game_add_entity(&game, &(Entity) {
    .shader = &game.shaders.basic,
    .model = &game.models.grid,
    .transform = m4identity,
    .render = render_debug,
  });

  //// Camera Controller
  //game_add_entity(&game, &(Entity) {
  //  .behavior = behavior_test_camera,
  //});

  // Spinny Cube
  game_add_entity(&game, &(Entity) {
    .shader = &game.shaders.basic,
    .model = &game.models.color_cube,
    .pos = (vec3){0, 20, 0},
    .render = render_basic,
    .behavior = behavior_player,
  });

  return 1;
}

void export(wasm_update) (float dt) {
  process_system_events(&game);
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