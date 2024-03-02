#include "test_behaviors.h"

#include <GL/gl.h>

#include "draw.h"

void behavior_test_camera(Entity* _, Game* game, float dt) {
  if (game->input.pressed.forward)
    game->camera.pos.xyz = v3add(game->camera.pos.xyz, v3scale(game->camera.front.xyz, dt));
  if (game->input.pressed.back)
    game->camera.pos.xyz = v3add(game->camera.pos.xyz, v3scale(game->camera.front.xyz, -dt));
  if (game->input.pressed.right) {
    vec3 right = v3norm(v3cross(game->camera.front.xyz, game->camera.up.xyz));
    right = v3scale(right, dt);
    game->camera.pos.xyz = v3add(game->camera.pos.xyz, right);
    game->target = v3add(game->target, right);
  }
  if (game->input.pressed.left) {
    vec3 left = v3norm(v3cross(game->camera.front.xyz, game->camera.up.xyz));
    left = v3scale(left, -dt);
    game->camera.pos.xyz = v3add(game->camera.pos.xyz, left);
    game->target = v3add(game->target, left);
  }
}

void behavior_cubespin(Entity* e, Game* _, float dt) {
  e->transform = m4translation(e->pos);
  e->transform = m4mul(e->transform, m4rotation(v3norm((vec3){1, 1.5, -.7}), e->angle));
  e->transform = m4mul(e->transform, m4rotation(v3norm((vec3){-4, 1.5, 1}), e->angle/3.6));
  e->angle += 2 * dt;
}

void behavior_stare(Entity* e, Game* game, float _) {
  e->transform = m4look(e->pos, game->camera.pos.xyz, v3y);
}

void behavior_attach_to_light(Entity* e, Game* game, float _) {
  e->transform = m4translation(game->light_pos.xyz);
}

void behavior_attach_to_camera_target(Entity* e, Game* game, float _) {
  e->transform = m4translation(game->target);
}

void render_basic(Entity* e, Game* game) {
  shader_program_use(e->shader);
  int projViewMod_loc = e->shader->uniform.projViewMod;

  mat4 pvm = m4mul(game->camera.projview, e->transform);
  glUniformMatrix4fv(projViewMod_loc, 1, GL_FALSE, pvm.f);
  model_render(e->model);
}

void render_debug(Entity* e, Game* game) {
  render_basic(e, game);
  draw_render();
}

void render_phong(Entity* e, Game* game) {
  shader_program_use(e->shader);
  UniformLocsPhong uniforms = e->shader->uniform.phong;
  uint pvm = e->shader->uniform.projViewMod;
  // should also pass transpose(inverse(model)) to multiply against normal

  glUniformMatrix4fv(pvm, 1, 0, m4mul(game->camera.projview, e->transform).f);
  glUniform4fv(uniforms.lightPos, 1, game->light_pos.f);
  glUniform4fv(uniforms.cameraPos, 1, game->camera.pos.f);

  glActiveTexture(GL_TEXTURE0);
  glUniform1i(uniforms.sampler, 0);

  glBindTexture(GL_TEXTURE_2D, e->texture->handle);
  glUniformMatrix4fv(uniforms.world, 1, 0, e->transform.f);
  model_render(&game->models.box);

  glBindTexture(GL_TEXTURE_2D, 0);
}
