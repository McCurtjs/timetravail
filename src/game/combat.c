#include "game_behaviors.h"

#include "draw.h"
#include "wasm.h"

#define PLAYER_SCALE 1.5

bool handle_player_combat(
  Game* game, const Entity* active, PlayerFrameData* fd
) {
  if (fd->hitstun) --fd->hitstun;

  if (!active) {
    return FALSE;
  }

  //vec4 color = c4cyan;

  vec2 hurtbox = (vec2){fd->pos.x, fd->pos.y + 1.5};
  float h_radius = 1.5;

  for (uint i = 0; i < game->entities.size; ++i) {
    //draw_color(c4gray.rgb);
    Entity* entity = vector_get(&game->entities, i);

    if (entity->type != ENTITY_PLAYER) continue;
    if (anim_is_warp(entity->fd.animation)) continue;

    uint current_frame = (uint)game->frame - entity->fd.start_frame;
    vec2 center = v2add(entity->fd.pos, v2scale(v2y, 1.5));

    //if (entity != active) draw_circle(v2v3(center, 0), h_radius);

    const Hitbox* hbox = anim_hitbox(entity->fd.animation, current_frame);

    if (hbox && !fd->hitstun) {
      vec2 hit_offset = v2scale(hbox->pos, PLAYER_SCALE);
      if (entity->fd.facing == FACING_LEFT) hit_offset.x *= -1;
      vec2 hit_loc = v2add(center, hit_offset);

      //draw_color(c4red.rgb); if (entity == active) draw_color(c4green.rgb);
      //draw_circle(v2v3(hit_loc, 0), hbox->radius * PLAYER_SCALE);

      if (entity == active) continue;

      float distance = v2mag(v2sub(hurtbox, hit_loc));
      //draw_line_solid(v2v3(hit_loc, 0), v2v3(hurtbox, 0), c4green.rgb);

      if (distance <= h_radius + hbox->radius) {
        vec2 knockback = hbox->knockback;
        knockback.x *= (entity->fd.facing == FACING_LEFT ? -1 : 1);

        fd->airborne = TRUE;
        fd->standing = FALSE;
        fd->animation = ANIMATION_FALL_2;
        fd->vel = knockback;
        fd->hitstun = hbox->hitstun;

        //color = c4red;
      }
    }
  }

  //draw_color(color.rgb);
  //draw_circle(v2v3(hurtbox, 0), h_radius);

  return FALSE;
}
