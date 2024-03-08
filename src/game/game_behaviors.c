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

void render_sprites(Entity* e, Game* g) {
  vec2 pos = mv4mul(e->transform, p4origin).xy;
  vec2 scale = mv4mul(e->transform, (vec4){1, 1, 0, 0}).xy;

  Animation* a = &e->anim_data.animations[e->fd.animation];
  uint current_frame = g->frame - e->fd.start_frame;
  uint frame_index;

  // if the repeat value is negative, do not repeat - hang on the last frame
  if (a->repeat < 0) {
    frame_index = a->frames[MIN(current_frame / a->rate, a->count - 1)].frame;

  // if the repeat value is zero, play the basic case (repeat whole animation)
  } else if (a->repeat == 0 || current_frame < (uint)a->repeat * a->rate) {
    frame_index = a->frames[(current_frame / a->rate) % a->count].frame;

  // if the repeat value is above zero, play up to that point, repeat the rest
  } else {
    // basically, if we have lead-in frames, we want to play those then repeat
    // the remainder forever. To do that we basically clip the whole animation
    // into just the looping portion and pretend the early frames didn't exist
    frame_index =
      ((current_frame / a->rate) - a->repeat) // set new current frame to 0
      %
      (a->count - a->repeat) // lower the total number of frames
      +
      a->repeat; // add an offset to account for the now missing frames

    frame_index = a->frames[frame_index].frame;
  }

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

