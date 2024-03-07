#include "game_behaviors.h"

#include "wasm.h"
#include "draw.h"

int handle_player_collisions(
  Game* game, PlayerFrameData old_fd, PlayerFrameData* new_fd
) {
  const Line* was_standing = new_fd->standing;
  int ret = 0;

  if (new_fd->standing) {
    // If you're on the platform and it's moving, apply that movement
    if (new_fd->standing && new_fd->standing->moving) {
      vec2 curr = platform_pos_at_frame(new_fd->standing->moving, game->frame);
      vec2 prev = platform_pos_at_frame(new_fd->standing->moving, game->frame - 1);
      new_fd->pos = v2add(new_fd->pos, v2sub(curr, prev));
    }

    vec2 extent = v2sub(new_fd->standing->b, new_fd->standing->a);
    float length = v2mag(extent);
    vec2 entity = v2sub(new_fd->pos, new_fd->standing->a);
    float dot = v2dot(v2norm(extent), entity);

    // Fall off the edge of the platform if you run past its extents
    if (dot < 0 || dot > length) {
      new_fd->standing = NULL;
      new_fd->airborne = TRUE;
      new_fd->animation = ANIMATION_AIR;
    }

    // Drop through the platform if it's droppable and you press down
    if (game->input.pressed.back && was_standing->droppable) {
      new_fd->standing = NULL;
      new_fd->airborne = TRUE;
      new_fd->animation = ANIMATION_AIR;
      new_fd->vel.y -= 10;
    }
  }

  float closest = 99999;
  vec2 closest_p;
  vec2 closest_n;
  Line* closest_line = NULL;

  for (uint i = 0; i < game->collider_count; ++i) {

    Line line = game->colliders[i];
    vec2 p;

    if (v2seg_seg(old_fd.pos, new_fd->pos, line.a, line.b, &p)) {

      if (was_standing == &game->colliders[i]) {
        continue;
      }

      vec2 dir = v2sub(new_fd->pos, old_fd.pos);
      vec2 n = v2perp(v2norm(v2sub(line.b, line.a)));

      if (v2dot(dir, n) < 0 && !(line.droppable && game->input.pressed.back)) {

        float dist = v2dist(new_fd->pos, p);
        if (closest_line == NULL || dist < closest) {
          closest = dist;
          closest_p = p;
          closest_n = n;
          closest_line = &game->colliders[i];
        }
      }
    }
  }

  if (closest_line) {
    if (!closest_line->wall && !closest_line->bouncy) {
      new_fd->standing = closest_line;
      new_fd->airborne = FALSE;
      new_fd->has_double = TRUE;
    }

    new_fd->pos = v2add(closest_p, v2scale(closest_n, 0.01));

    vec2 v = v2sub(closest_line->b, closest_line->a);
    vec2 v_unit = v2norm(v);

    if (closest_line->bouncy) {
      new_fd->vel = v2reflect(new_fd->vel, v_unit);
    } else {
      new_fd->vel = v2scale(v_unit, v2dot(v_unit, new_fd->vel));
    }

    ret = 1;
  }

  if (new_fd->pos.y <= 0) {
    new_fd->pos.y = 0;
    new_fd->vel.y = 0;
    new_fd->airborne = FALSE;
    new_fd->has_double = TRUE;
    ret = 1;
  }

  return ret;
}
