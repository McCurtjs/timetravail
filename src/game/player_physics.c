#include "game_behaviors.h"

#include "wasm.h"
#include "draw.h"

int handle_player_collisions(
  Game* game, PlayerFrameData old_fd, PlayerFrameData* new_fd
) {
  const Line* was_standing = new_fd->standing;
  int ret = 0;

  if (new_fd->standing) {
    vec2 extent = v2sub(new_fd->standing->b, new_fd->standing->a);
    float length = v2mag(extent);
    vec2 entity = v2sub(new_fd->pos, new_fd->standing->a);
    float dot = v2dot(v2norm(extent), entity);
    if (dot < 0 || dot > length) {
      new_fd->standing = FALSE;
      new_fd->airborne = TRUE;
    }

    if (game->input.pressed.back && was_standing->droppable) {
      new_fd->standing = NULL;
      new_fd->airborne = TRUE;
      new_fd->vel.y -= 10;
    }
  }

  for (uint i = 0; i < game->collider_count; ++i) {

    Line line = game->colliders[i];
    vec2 p;

    if (v2seg_seg(old_fd.pos, new_fd->pos, line.a, line.b, &p)) {

      if (was_standing == &game->colliders[i]) {
        continue;
      }

      vec2 dir = v2sub(new_fd->pos, old_fd.pos);
      vec2 v = v2sub(line.b, line.a);
      vec2 v_unit = v2norm(v);
      vec2 n = v2perp(v_unit);

      if (v2dot(dir, n) < 0 && !(line.droppable && game->input.pressed.back)) {
        if (!line.wall && !line.bouncy) {
          new_fd->standing = &game->colliders[i];
          new_fd->airborne = FALSE;
          new_fd->has_double = TRUE;
        }

        new_fd->pos = v2add(p, v2scale(n, 0.01));

        if (line.bouncy) {
          print("Bouncy?");
          print_float(new_fd->vel.y);
          new_fd->vel = v2reflect(new_fd->vel, v_unit);
          print_float(new_fd->vel.y);
        } else {
          new_fd->vel = v2scale(v_unit, v2dot(v_unit, new_fd->vel));
        }

        ret = 1;
      }
    }
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
