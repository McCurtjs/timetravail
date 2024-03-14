#include "game.h"
#include "game/levels.h"

#include <math.h>

#include "test_behaviors.h"
#include "game/game_behaviors.h"
#include "wasm.h"
#include "draw.h"
#include "mat.h"

static Vector editor_colliders;
static vec2 player_start = v2zero;

void behavior_editor(Entity* _, Game* game, float dt) {

  if (game->input.pressed.rmb) {
    vec2* pos = &game->camera.pos.xy;
    vec2 move = game->input.mouse.move;
    move.x *= -1;
    *pos = v2add(*pos, v2scale(move, dt));
  }

  static vec2 start_point = v2zero;
  static vec2 end_point = v2zero;

  // Get mouse position in world space
  vec3 cursor = v3front;

  // convert screen space from [0, wh] to [-1, 1]
  cursor.xy = game->input.mouse.pos;
  cursor.x =      (cursor.x / (float)game->window.x - 0.5) * 2;
  cursor.y = (1 - (cursor.y / (float)game->window.y) - 0.5) * 2;

  // find a point 1 unit away on a plane defined by our field of view
  float half_field_of_view = d2r(20) / 2;
  float screen_plane_halfwidth = tanf(half_field_of_view);
  cursor.x = screen_plane_halfwidth * i2aspect(game->window) * cursor.x;
  cursor.y = screen_plane_halfwidth * cursor.y;

  // project the vector from the camera to that point onto the world xy plane
  float t;
  v3line_plane(game->camera.pos.xyz, cursor, v3origin, v3z, &t);
  cursor = v3add(game->camera.pos.xyz, v3scale(cursor, t));

  // snap to grid
  if (game->input.pressed.shift) {
    cursor.x = floorf(cursor.x + 0.5);
    cursor.y = floorf(cursor.y + 0.5);
  }

  // draw the cursor so we know what we're doing
  draw_color(c4green.rgb);
  draw_circle(cursor, 0.25);

  if (game->input.triggered.mmb) {
    player_start = cursor.xy;
  }

  // draw the player start location
  draw_color(c4cyan.rgb);
  draw_circle(v2v3(v2add(player_start, (vec2){0, 1.5}), 0), 1.5);

  if (game->input.pressed.lmb) {
    end_point = cursor.xy;


    if (game->input.triggered.lmb) {
      start_point = cursor.xy;
      print_floats(start_point.f, 2);
    } else {
      draw_line_solid(v2v3(start_point, 0), v2v3(end_point, 0), c4magenta.rgb);
    }
  }

  if (game->input.released.lmb) {
    bool droppable = game->input.pressed.back;
    bool wall = game->input.pressed.right;
    bool bouncy = game->input.pressed.forward;

    vector_push_back(&editor_colliders, &(Line) {
      .a = start_point, .b = end_point,
      .bouncy = bouncy, .wall = wall, .droppable = droppable
    });

    // update count and reset collider data pointer in case it resized
    game->colliders = editor_colliders.data;
    game->collider_count = editor_colliders.size;
  }

  if (game->input.triggered.run_replay && editor_colliders.size) {
    vector_pop_back(&editor_colliders, NULL);
    game->collider_count = editor_colliders.size;
  }
}

void level_load_editor(Game* game) {

  game->camera.pos = (vec4){0, 0, 100, 1};
  game->camera.front = v4front;

  if (!editor_colliders.data) {
    vector_init(&editor_colliders, sizeof(Line));
  }

  game->colliders = editor_colliders.data;
  game->collider_count = editor_colliders.size;

  // Debug Renderer
  game_add_entity(game, &(Entity) {
    .shader = &game->shaders.basic,
    .model = &game->models.grid,
    .transform = m4identity,
    .render = render_debug,
  });

  // Entity to draw physics lines
  game_add_entity(game, &(Entity) {
    .behavior = behavior_draw_physics_colliders
  });

  // Editor managing entity
  game_add_entity(game, &(Entity) {
    .behavior = behavior_editor
  });

}

void level_load_editor_test(Game* game) {

  game->camera.pos = (vec4){0, 0, 60, 1};
  game->camera.front = v4front;

  game->colliders = editor_colliders.data;
  game->collider_count = editor_colliders.size;

  // Debug Renderer
  game_add_entity(game, &(Entity) {
    .shader = &game->shaders.basic,
    .model = &game->models.grid,
    .transform = m4identity,
    .render = render_debug,
  });

  // Entity to draw physics lines
  game_add_entity(game, &(Entity) {
    .behavior = behavior_draw_physics_colliders
  });

  // Time Controller
  game_add_entity(game, &(Entity) {
    .behavior = behavior_time_controller,
  });

  // Player
  game_add_entity(game, &(Entity) {
    .type = ENTITY_PLAYER,
    .shader = &game->shaders.light,
    .model = &game->models.player,
    .fd = {
      .pos = player_start,
      .vel = v2zero,
      .airborne = TRUE,
      .has_double = TRUE
    },
    .anim_data = {
      .animations = player_animations,
      .hitboxes = player_hitboxes,
      .anim_count = ANIMATION_COUNT,
    },
    .replay = { .data = NULL },
    .replay_temp = { .data = NULL },
    .render = render_sprites,
    .behavior = behavior_player,
    .delete = delete_player,
  });

  vector_init(&game->timeguys, sizeof(PlayerRef));
  vector_push_back(&game->timeguys, &(PlayerRef){
    .start_frame = 0,
    .e = vector_get_back(&game->entities)
  });

}
