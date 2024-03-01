#include "eng/wasm.h"
#include "eng/shader.h"
#include "eng/mat.h"
#include "eng/model.h"
#include "eng/camera.h"
#include "eng/texture.h"

#include <stdio.h>
#include <string.h>
#include <math.h>

#include "GL/gl.h"
#include "SDL/sdl.h"

#include "game.h"
#include "system_events.h"

int export(canary) (int a) {
  print("WASM is connected!");
  return a * a;
}

Game game;

File file_vert;
File file_frag;
Shader test_vert;
Shader test_frag;
ShaderProgram test_shader;
ShaderProgram basic_shader;

mat4 model;
mat4 model2;
mat4 model3;

float cubespin = 0;

Model cube;
Model ccube;
Model ccube2;
Model grid;
Model gizmo;

Image image_crate;
Image image_tiles;
Texture texture_crate;
Texture texture_tiles;

void export(wasm_preload) (uint w, uint h) {
  game.window.w = w;
  game.window.h = h;

  file_open_async(&file_vert, "./res/shaders/basic.vert");
  file_open_async(&file_frag, "./res/shaders/basic.frag");

  image_open_async(&image_crate, "./res/textures/crate.png");
  image_open_async(&image_tiles, "./res/textures/tiles.png");

  model = m4identity;

  game.camera.pos = (vec4){3, 2, 5, 1};
  game.camera.front = v4front;
  game.camera.up = v4y;
  game.camera.persp = (Camera_PerspectiveParams){d2r(70), (float)w/h, 0.1, 50};
  camera_build_perspective(&game.camera);
  camera_look_at(&game.camera, v3zero);

  game.light_pos = (vec4){4, 3, 5, 1};

  ccube.type = MODEL_CUBE_COLOR;
  model_build(&ccube);

  shader_program_build_basic(&basic_shader);
}

void cheesy_loading_anim(float dt) {
  mat4 projview = camera_projection_view(&game.camera);

  shader_program_use(&basic_shader);
  int projViewMod_loc = basic_shader.uniform.projViewMod;

  model = m4translation((vec3){0, 0, 0});
  model = m4mul(model, m4rotation(v3norm((vec3){1, 1.5, -.7}), cubespin));
  model = m4mul(model, m4rotation(v3norm((vec3){-4, 1.5, 1}), cubespin/3.6));

  glUniformMatrix4fv(projViewMod_loc, 1, 0, m4mul(projview, model).f);
  model_render(&ccube);
  cubespin += 2 * dt;
}

int export(wasm_load) (int await_count, float dt) {
  if (await_count) {
    cheesy_loading_anim(dt);
    return 0;
  };
  cubespin = 0;

  shader_build_from_file(&test_vert, &file_vert);
  shader_build_from_file(&test_frag, &file_frag);

  file_delete(&file_vert);
  file_delete(&file_frag);

  texture_build_from_image(&texture_crate, &image_crate);
  texture_build_from_image(&texture_tiles, &image_tiles);

  image_delete(&image_crate);
  image_delete(&image_tiles);

  shader_program_build(&test_shader, &test_vert, &test_frag);
  shader_program_load_uniforms(&test_shader, UNIFORMS_PHONG);

  cube.type = MODEL_CUBE;
  ccube2.type = MODEL_CUBE_COLOR;
  model_setup_default_grid(&grid, 20);
  model_setup_default_grid(&gizmo, -2);
  model_build(&cube);
  model_build(&ccube2);
  model_build(&grid);
  model_build(&gizmo);

  return 1;
}

void export(wasm_update) (float dt) {
  process_system_events(&game);

  model = m4translation((vec3){-2, 0, 0});
  model = m4mul(model, m4rotation(v3norm((vec3){1, 1.5, -.7}), cubespin));
  model = m4mul(model, m4rotation(v3norm((vec3){-4, 1.5, 1}), cubespin/3.6));

  model2 = m4translation((vec3){2, 0, 0});
  model2 = m4mul(model2, m4uniform(2));
  //model2 = m4mul(model2, m4rotation(v3norm((vec3){-1, 0.2, 1}), cubespin/8.7));

  model3 = m4look((vec3){0, 0, 2}, game.camera.pos.xyz, v3y);

  cubespin += 2 * dt;
}

void export(wasm_render) () {
  mat4 projview = camera_projection_view(&game.camera);

  shader_program_use(&basic_shader);
  int projViewMod_loc = basic_shader.uniform.projViewMod;

  glUniformMatrix4fv(projViewMod_loc, 1, 0, projview.f);
  model_render(&grid);

  glUniformMatrix4fv(projViewMod_loc, 1, 0, m4mul(projview, model).f);
  model_render(&ccube);

  glUniformMatrix4fv(projViewMod_loc, 1, 0, m4mul(projview, model3).f);
  model_render(&ccube);

  if (game.button_down)
    model_render(&gizmo);

  mat4 lightTransform = m4mul(projview, m4translation(game.light_pos.xyz));
  glUniformMatrix4fv(projViewMod_loc, 1, 0, lightTransform.f);
  model_render(&gizmo);

  shader_program_use(&test_shader);
  projViewMod_loc = test_shader.uniform.projViewMod;
  int world_loc = test_shader.uniform.phong.world;
  int lightPos_loc = test_shader.uniform.phong.lightPos;
  int cameraPos_loc = test_shader.uniform.phong.cameraPos;
  int sampler_loc = test_shader.uniform.phong.sampler;
  // should also pass transpose(inverse(model)) to multiply against normal

  glUniformMatrix4fv(projViewMod_loc, 1, 0, m4mul(projview, model2).f);
  glUniform4fv(lightPos_loc, 1, game.light_pos.f);
  glUniform4fv(cameraPos_loc, 1, game.camera.pos.f);

  glActiveTexture(GL_TEXTURE0);
  glUniform1i(sampler_loc, 0);

  glBindTexture(GL_TEXTURE_2D, texture_crate.handle);
  glUniformMatrix4fv(world_loc, 1, 0, model2.f);
  model_render(&cube);

  mat4 lorge_cube = m4translation((vec3){0, -10.5, -5});
  lorge_cube = m4mul(lorge_cube, m4uniform(19));

  glBindTexture(GL_TEXTURE_2D, texture_tiles.handle);

  glUniformMatrix4fv(projViewMod_loc, 1, 0, m4mul(projview, lorge_cube).f);
  glUniformMatrix4fv(world_loc, 1, 0, lorge_cube.f);
  model_render(&cube);

  glBindTexture(GL_TEXTURE_2D, 0);
}

// todo:
// game objects??? (container with function pointers to behavior/render fns)
// key inputs
// make grid generic based on basis vectors
// actually test orthographic mode
// atlas'd and batched 2d sprite animation
// load model from file
// instances
// fix camera rotation on poles in non-local orbit and rotation mode
