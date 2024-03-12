#include "game_behaviors.h"

#include "GL/gl.h"

#include "draw.h"
#include "wasm.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
void behavior_draw_physics_colliders(Entity* _, Game* game, float dt) {
#pragma clang diagnostic pop

  float z = 0.01;

  for (uint i = 0; i < game->collider_count; ++i) {
    Line line = game->colliders[i];
    vec3 color = (vec3){0.5, 0.5, 1};

    if (line.droppable) color = (vec3) {76/255.0, 229/255.0, 209/255.0};
    if (line.wall) color = (vec3) {250/255.0, 128/255.0, 114/255.0};
    if (line.moving) color = (vec3) {173/255.0, 229/255.0, 76/255.0};
    if (line.bouncy) color = (vec3) {202/255.0, 193/255.0, 150/255.0};

    draw_line_solid(v2v3(line.a, z), v2v3(line.b, z), color);

    vec2 v = v2scale(v2sub(line.b, line.a), 0.5);
    vec2 mid = v2add(line.a, v);
    v = v2norm(v2perp(v));

    draw_offset(v2v3(mid, z * 2));
    draw_color(c4yellow.rgb);
    draw_vector(v2v3(v, 0));
  }
}

void render_sprites(Entity* e, Game* g) {
  vec2 pos = mv4mul(e->transform, p4origin).xy;
  vec2 scale = mv4mul(e->transform, (vec4){1, 1, 0, 0}).xy;

  uint current_frame = g->frame - e->fd.start_frame;
  uint frame_index = anim_frame(e->fd.animation, current_frame)->frame;

  model_sprites_draw(&e->model->sprites, pos, scale, frame_index, e->fd.facing);
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

// Delete fns
void delete_player(Entity* entity) {
  if (entity->replay.data) {
    vector_delete(&entity->replay);
  }

  if (entity->replay_temp.data) {
    vector_delete(&entity->replay_temp);
  }
}

