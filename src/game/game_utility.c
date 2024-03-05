#include "game_behaviors.h"

#include "draw.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
void behavior_draw_physics_colliders(Entity* _, Game* game, float dt) {
#pragma clang diagnostic pop

  for (uint i = 0; i < game->collider_count; ++i) {
    Line line = game->colliders[i];

    draw_line_solid(v2v3(line.a, 0), v2v3(line.b, 0), (vec3){0.5, 0.5, 1});

    vec2 v = v2scale(v2sub(line.b, line.a), 0.5);
    vec2 mid = v2add(line.a, v);
    v = v2norm(v2perp(v));

    draw_offset(v2v3(mid, 0));
    draw_color((vec3){1, 1, 0});
    draw_vector(v2v3(v, 0));
  }
}
