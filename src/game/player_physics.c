#include "game_behaviors.h"

#include "wasm.h"
#include "draw.h"

int handle_player_collisions(
  Game* game, PlayerFrameData old_fd, PlayerFrameData* new_fd
) {
  if (new_fd->standing) {
    vec2 extent = v2sub(new_fd->standing->b, new_fd->standing->a);
    float length = v2mag(extent);
    vec2 entity = v2sub(new_fd->pos, new_fd->standing->a);
    float dot = v2dot(v2norm(extent), entity);
    if (dot < 0 || dot > length) {
      new_fd->standing = FALSE;
    }
  }

  for (uint i = 0; i < game->collider_count; ++i) {
    Line line = game->colliders[i];
    vec2 p;

    if (v2seg_seg(old_fd.pos, new_fd->pos, line.a, line.b, &p)) {
      vec2 dir = v2sub(new_fd->pos, old_fd.pos);
      vec2 n = v2perp(v2sub(line.b, line.a));
      if (v2dot(dir, n) < 0) {
        float epsilon = 0.025;
        if (new_fd->pos.x > 20) {
          epsilon = 0.01 + new_fd->pos.x * 0.001;
          print_float(epsilon);
        }

        new_fd->standing = &game->colliders[i];
        new_fd->pos = v2add(p, v2scale(v2norm(n), epsilon));
        new_fd->vel.y = 0;
        new_fd->airborne = FALSE;
        new_fd->has_double = TRUE;
        return 1;
      }
    }
  }

  if (new_fd->pos.y <= 0) {
    new_fd->pos.y = 0;
    new_fd->vel.y = 0;
    new_fd->airborne = FALSE;
    new_fd->has_double = TRUE;
    return 1;
  }
  return 0;
}
