#include "wasm.h"
#include "shader.h"
#include "mat.h"
#include "model.h"
#include "camera.h"

#include <stdio.h>
#include <string.h>
#include <math.h>

#include "GL/gl.h"
#include "SDL/sdl.h"

int export(canary) (int a) {
  print("WASM is connected!");
  return a * a;
}

typedef struct Game {
  vec2i window;
  Camera camera;
} Game;

Game game;
int projViewMod_loc; // put in shader

File file_vert;
File file_frag;
Shader test_vert;
Shader test_frag;
ShaderProgram test_shader;

mat4 model;
mat4 model2;
mat4 model3;

float cubespin = 0;

Model cube;
Model ccube;
Model ccube2;
Model grid;
Model gizmo;

void model_render_cube(Model_Cube* cube);
void model_render_cube_color(Model_CubeColor* cube);

void export(wasm_preload) () {
  file_open_async(&file_vert, "./res/shaders/basic.vert");
  file_open_async(&file_frag, "./res/shaders/basic.frag");

  model = m4identity;

  game.camera = (Camera){
    (vec4){3, 2, 5, 1}, v4front, v4y, d2r(70), 1, 0.1, 50
  };
  camera_build_perspective(&game.camera);
  camera_look_at(&game.camera, v3zero);
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
  ccube2.type = MODEL_CUBE_COLOR;
  grid.type = MODEL_GRID;
  grid.grid.extent = 20;
  gizmo.type = MODEL_GRID;
  gizmo.grid.extent = -2;
  model_build(&cube);
  model_build(&ccube);
  model_build(&ccube2);
  model_build(&grid);
  model_build(&gizmo);

  return 1;
}

int button_down = 0;

void handle_events() {
  SDL_Event event;

  while (SDL_PollEvent(&event)) {
    switch(event.type) {
      case SDL_EVENT_WINDOW_RESIZED: {
        game.window.w = event.window.data1;
        game.window.h = event.window.data2;
        glViewport(0, 0, game.window.w, game.window.h);
        game.camera.persp.aspect = game.window.w / (float)game.window.h;
        camera_build_perspective(&game.camera);
       } break;

       case SDL_EVENT_MOUSE_BUTTON_DOWN: {
        button_down = event.button.button;
       } break;

       case SDL_EVENT_MOUSE_BUTTON_UP: {
        button_down = 0;
       } break;

       case SDL_EVENT_MOUSE_MOTION: {
        if (button_down == 1) {
          float xrot = d2r(-event.motion.yrel * 180 / (float)game.window.h);
          float yrot = d2r(-event.motion.xrel * 180 / (float)game.window.x);
          vec3 angles = (vec3){xrot, yrot, 0};
          camera_orbit(&game.camera, v3zero, angles.xy);
        }
       } break;
    }
  }
}

void export(wasm_update) (float dt) {
  handle_events();

  model = m4translation(v4left.xyz);
  model = m4mul(model, m4rotation(v3norm((vec3){1, 1.5, -.7}), cubespin));
  model = m4mul(model, m4rotation(v3norm((vec3){-4, 1.5, 1}), cubespin/3.6));

  model2 = m4translation(v3x);
  model2 = m4mul(model2, m4rotation(v3norm((vec3){-1, 0.2, 1}), cubespin/8.7));

  model3 = m4look((vec3){0, 0, 2}, game.camera.pos.xyz, v3y);

  cubespin += 2 * dt;
}

void export(wasm_render) () {
  mat4 projview = camera_projection_view(&game.camera);

  shader_program_use(&test_shader);

  vec4 v;

  glUniformMatrix4fv(projViewMod_loc, 1, 0, m4mul(projview, model).f);
  model_render(&ccube);

  glUniformMatrix4fv(projViewMod_loc, 1, 0, m4mul(projview, model3).f);
  model_render(&ccube);

  if (button_down)
    model_render(&gizmo);

  glUniformMatrix4fv(projViewMod_loc, 1, 0, m4mul(projview, model2).f);
  model_render(&cube);

  glUniformMatrix4fv(projViewMod_loc, 1, 0, projview.f);
  model_render(&grid);
}

// todo:
// key inputs
// load model from file
// textures
// game objects???
// instances
// fix camera rotation on poles in non-local orbit and rotation mode
