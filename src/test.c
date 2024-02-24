#include "wasm.h"
#include "shader.h"
#include "mat.h"

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
float camera_aspect;

mat4 model;
mat4 view;
mat4 projection;
mat4 PVM;

float cubespin = 0;

vec3 positions[14] = {
  {-0.5, -0.5,  0.5},
  { 0.5, -0.5,  0.5},
  {-0.5,  0.5,  0.5},
  { 0.5,  0.5,  0.5},
  { 0.5,  0.5, -0.5},
  { 0.5, -0.5,  0.5},
  { 0.5, -0.5, -0.5},
  {-0.5, -0.5,  0.5},
  {-0.5, -0.5, -0.5},
  {-0.5,  0.5,  0.5},
  {-0.5,  0.5, -0.5},
  { 0.5,  0.5, -0.5},
  {-0.5, -0.5, -0.5},
  { 0.5, -0.5, -0.5},
};

vec4 colors[14] = {
  {1.0, 1.0, 1.0, 1.0}, // white    111
  {1.0, 0.0, 0.0, 1.0}, // red      100
  {0.0, 1.0, 0.0, 1.0}, // green    010
  {0.0, 0.0, 1.0, 1.0}, // blue     001
  {1.0, 1.0, 0.0, 1.0}, // yellow   110
  {1.0, 0.0, 0.0, 1.0}, // red
  {0.0, 1.0, 1.0, 1.0}, // A        011
  {1.0, 1.0, 1.0, 1.0}, // white
  {1.0, 0.0, 1.0, 1.0}, // magenta  101
  {0.0, 1.0, 0.0, 1.0}, // green
  {0.0, 0.0, 0.0, 1.0}, // B        000
  {1.0, 1.0, 0.0, 1.0}, // yellow
  {1.0, 0.0, 1.0, 1.0}, // magenta
  {0.0, 1.0, 1.0, 1.0}, // A
};

GLuint pos_buffer;
GLuint color_buffer;

void export(wasm_preload) () {
  file_open_async(&file_vert, "./res/shaders/basic.vert");
  file_open_async(&file_frag, "./res/shaders/basic.frag");

  model = m4identity;

  view = m4translation((vec3){0, 0, 2});
  view = m4inverse(view);

  projection = m4identity;
}

int projViewMod_loc = 0;

int export(wasm_load) (int await_count) {
  if (await_count) return 0;

  shader_build_from_file(&test_vert, &file_vert);
  shader_build_from_file(&test_frag, &file_frag);

  file_delete(&file_vert);
  file_delete(&file_frag);

  shader_program_build(&test_shader, &test_vert, &test_frag);
  shader_program_use(&test_shader);
  projViewMod_loc = glGetUniformLocation(test_shader.handle, "projViewMod");

  glGenBuffers(1, &pos_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, pos_buffer);
  glBufferData(GL_ARRAY_BUFFER, 14 * sizeof(*positions), positions, GL_STATIC_DRAW);
  glVertexAttribPointer(0, sizeof(*positions) / 4, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);

  glGenBuffers(1, &color_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, color_buffer);
  glBufferData(GL_ARRAY_BUFFER, 14 * sizeof(vec4), colors, GL_STATIC_DRAW);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(1);

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
        camera_aspect = window.w / (float)window.h;
        projection = m4perspective(TAU / 4.0, camera_aspect, 0, 999);
       } break;
    }
  }
}

void export(wasm_update) (float dt) {
  handle_events();

  model = m4rotation(v3norm((vec3){1, 1.5, -.7}), cubespin);
  model = m4mul(model, m4rotation(v3norm((vec3){-4, 1.5, 1}), cubespin/3.6));
  cubespin += dt / 600;
}

void export(wasm_render) () {
  PVM = m4mul(projection, view);
  PVM = m4mul(PVM, model);

  shader_program_use(&test_shader);
  glUniformMatrix4fv(projViewMod_loc, 1, 0, PVM.f);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 14);
}

// todo:
// make model class
// load model from file
// textures
// camera class
// game objects???
