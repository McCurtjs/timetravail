#include "wasm.h"
#include "shader.h"
#include "mat.h"
#include "model.h"

#include <stdio.h>
#include <string.h>

#include "GL/gl.h"
#include "SDL/sdl.h"

int export(canary) (int a) {
  print("WASM is connected!");
  return a * a;
}

File file_vert;
File file_frag;
Shader test_vert;
Shader test_frag;
ShaderProgram test_shader;

vec2i window;

mat4 view;
mat4 projection;
int projViewMod_loc = 0;

mat4 model;
mat4 model2;

float cubespin = 0;

Model cube;
Model ccube;
Model grid;

void model_render_cube(Model_Cube* cube);
void model_render_cube_color(Model_CubeColor* cube);

void export(wasm_preload) () {
  file_open_async(&file_vert, "./res/shaders/basic.vert");
  file_open_async(&file_frag, "./res/shaders/basic.frag");

  model = m4identity;
  view = m4rotation(v3y, d2r(-20));
  view = m4mul(view, m4rotation(v3x, d2r(-45)));
  view = m4mul(view, m4translation((vec3){0, 0, 6}));
  view = m4inverse(view);
  projection = m4identity;
}

int export(wasm_load) (int await_count) {
  if (await_count) return 0;

  shader_build_from_file(&test_vert, &file_vert);
  shader_build_from_file(&test_frag, &file_frag);

  file_delete(&file_vert);
  file_delete(&file_frag);

  shader_program_build(&test_shader, &test_vert, &test_frag);
  shader_program_use(&test_shader);
  projViewMod_loc = glGetUniformLocation(test_shader.handle, "projViewMod");

  cube.type = MODEL_CUBE;
  ccube.type = MODEL_CUBE_COLOR;
  grid.type = MODEL_GRID;
  grid.grid.extent = 5;
  model_build(&cube);
  model_build(&ccube);
  model_build(&grid);

  return 1;
}

void handle_events() {
  SDL_Event event;

  while (SDL_PollEvent(&event)) {
    switch(event.type) {
      case SDL_EVENT_WINDOW_RESIZED: {
        window.w = event.window.data1;
        window.h = event.window.data2;
        glViewport(0, 0, window.w, window.h);
        float camera_aspect = window.w / (float)window.h;
        projection = m4perspective(d2r(70), camera_aspect, 0.1, 50);
       } break;
    }
  }
}

void export(wasm_update) (float dt) {
  handle_events();

  model = m4translation((vec3){-1, 0, 0});
  model = m4mul(model, m4rotation(v3norm((vec3){1, 1.5, -.7}), cubespin));
  model = m4mul(model, m4rotation(v3norm((vec3){-4, 1.5, 1}), cubespin/3.6));

  model2 = m4translation((vec3){1, 0, 0});
  model2 = m4mul(model2, m4rotation(v3norm((vec3){-1, 0.2, 1}), cubespin/8.7));

  cubespin += dt / 600;
}

void export(wasm_render) () {
  mat4 projview = m4mul(projection, view);

  shader_program_use(&test_shader);

  vec4 v;

  glUniformMatrix4fv(projViewMod_loc, 1, 0, m4mul(projview, model).f);
  model_render(&ccube);

  glUniformMatrix4fv(projViewMod_loc, 1, 0, m4mul(projview, model2).f);
  model_render(&cube);

  glUniformMatrix4fv(projViewMod_loc, 1, 0, projview.f);
  model_render(&grid);
}

// todo:
// camera class
// load model from file
// textures
// game objects???
