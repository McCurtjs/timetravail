#include "game_behaviors.h"

#include <math.h>

#include "wasm.h"

void behavior_moving_platform(Entity* e, Game* game, float _) {
  Movement* m = &e->movement_params;

  if (!m->line->moving) {
    m->line->moving = m;
    m->origin = m->line->a;
    m->v_plat = v2sub(m->line->b, m->line->a);
    m->v_rail = v2sub(m->target, m->line->a);
  } else {
    print("Moving platform: assigned line already moving. Forgot cleanup?");
  }

  m->line->a = platform_pos_at_frame(m, game->frame);

  print_float(m->line->a.y);

  m->line->b = v2add(m->line->a, m->v_plat);
}

vec2 platform_pos_at_frame(Movement* m, float frame) {
  float t = (frame - m->delay * 60) / (m->duration * 60);
  if (t < 0) t = 0;

  if (m->move_once) {
    if (t > 1) t = 1;
  } else {
    t = (-cosf(t * PI) + 1) / 2;
  }

  return v2add(m->origin, v2scale(m->v_rail, t));
}

void delete_moving_platform(Entity* e) {
  Movement* m = &e->movement_params;

  if (m->line && m->line->moving) {
    m->line->moving = NULL;
    m->line->a = m->origin;
    m->line->b = v2add(m->origin, m->v_plat);
  }
}
