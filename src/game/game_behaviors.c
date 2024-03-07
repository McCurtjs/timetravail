#include "game_behaviors.h"

#include "GL/gl.h"

#include "draw.h"
#include "wasm.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
void behavior_draw_physics_colliders(Entity* _, Game* game, float dt) {
#pragma clang diagnostic pop

  float z = 0.1;

  for (uint i = 0; i < game->collider_count; ++i) {
    Line line = game->colliders[i];

    draw_line_solid(v2v3(line.a, z), v2v3(line.b, z), (vec3){0.5, 0.5, 1});

    vec2 v = v2scale(v2sub(line.b, line.a), 0.5);
    vec2 mid = v2add(line.a, v);
    v = v2norm(v2perp(v));

    draw_offset(v2v3(mid, z));
    draw_color(c4yellow.rgb);
    draw_vector(v2v3(v, z));
  }
}

void render_sprites(Entity* entity, Game* g) {
  model_sprites_draw(&entity->model->sprites, entity->fd.pos, 10, (uint)g->frame/4*0);
}

void finish_rendering_sprites(
  Game* game, const Model_Sprites* sprites, const Texture* texture
) {
  shader_program_use(&game->shaders.light);
  UniformLocsPhong uniforms = game->shaders.light.uniform.phong;
  uint pvm = game->shaders.light.uniform.projViewMod;

  glUniformMatrix4fv(pvm, 1, 0, game->camera.projview.f);
  glUniform4fv(uniforms.lightPos, 1, game->light_pos.f);
  glUniform4fv(uniforms.cameraPos, 1, game->camera.pos.f);

  glActiveTexture(GL_TEXTURE0);
  glUniform1i(uniforms.sampler, 0);

  glBindTexture(GL_TEXTURE_2D, texture->handle);
  glUniformMatrix4fv(uniforms.world, 1, 0, m4identity.f);

  model_render((Model*)sprites);

  glBindTexture(GL_TEXTURE_2D, 0);
}

