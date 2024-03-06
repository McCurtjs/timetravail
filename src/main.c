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
      .persp = {d2r(20), i2aspect((vec2i){w, h}), 0.1, 100}
      //.ortho = {-6 * i2aspect(windim), 6 * i2aspect(windim), 6, -6, 0.1, 500}
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

  // Set level geometry

  Line colliders[] = {
    { .a = (vec2){-4, 3}, .b = (vec2){-1, 3} },
    { .a = (vec2){ 1, 6}, .b = (vec2){ 4, 6} },
    { .a = (vec2){ 4.0, 6}, .b = (vec2){ 10, 6} },
    { .a = (vec2){ 20.5, 6}, .b = (vec2){ 20.5, 8}, .wall = TRUE },
    { .a = (vec2){-15, 7}, .b = (vec2){-8, 5} },
    { .a = (vec2){ 10, 0}, .b = (vec2){ 17, 7} },
    { .a = (vec2){ 17, 7}, .b = (vec2){ 100, 7}, .droppable = TRUE },
    { .a = (vec2){ -15, 4}, .b = (vec2){ -15, -0.5}, .wall = TRUE },
    { .a = (vec2){ -15, -0.5}, .b = (vec2){ -15, 4}, .wall = TRUE },

    { .a = (vec2){ 5, 13}, .b = (vec2){ 10, 13} },
    { .a = (vec2){ 5, 20}, .b = (vec2){ 10, 20} },
    { .a = (vec2){ 15, 25}, .b = (vec2){ 20, 25} },

    { .a = (vec2){-27, 1}, .b = (vec2){ -23, 1}, .bouncy = TRUE, .wall = TRUE },

    { .a = (vec2){-12, 30}, .b = (vec2){ 12, 30} },   // fd base
    { .a = (vec2){-2.5, 37}, .b = (vec2){ 2.5, 37}, .droppable = TRUE }, // top plat
    { .a = (vec2){-10, 33.5}, .b = (vec2){ -5, 33.5}, .droppable = TRUE }, // left plat
    { .a = (vec2){ 5, 33.5}, .b = (vec2){ 10, 33.5}, .droppable = TRUE }, // right plat
    { .a = (vec2){-8, 25}, .b = (vec2){-12, 30}, .wall = TRUE }, // left wall
    { .a = (vec2){ 12, 30}, .b = (vec2){ 8, 25}, .wall = TRUE }, // right wall
    { .a = (vec2){ 8, 25}, .b = (vec2){-8, 25}, .wall = TRUE }, // bottom
  };

  game.colliders = malloc(sizeof(colliders));
  memcpy(game.colliders, colliders, sizeof(colliders));
  game.collider_count = sizeof(colliders) / sizeof(Line);

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

  // Entity to draw physics lines
  game_add_entity(&game, &(Entity) {
    .behavior = behavior_draw_physics_colliders
  });

  // Time Controller
  game_add_entity(&game, &(Entity) {
    .behavior = behavior_time_controller,
  });

  // Player
  game_add_entity(&game, &(Entity) {
    .shader = &game.shaders.basic,
    .model = &game.models.color_cube,
    .fd = {
      .pos = (vec2){0, 20},
      .vel = v2zero,
      .airborne = TRUE,
      .has_double = TRUE
    },
    .replay = { .data = NULL },
    .replay_temp = { .data = NULL },
    .render = render_basic,
    .behavior = behavior_player,
  });
  vector_init(&game.timeguys, sizeof(PlayerRef));
  vector_push_back(&game.timeguys, &(PlayerRef){
    .start_frame = 0,
    .e = vector_get_back(&game.entities)
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
