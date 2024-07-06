#include "game.h"
#include "game/levels.h"

#include <math.h>

#include "test_behaviors.h"
#include "game/game_behaviors.h"
#include "wasm.h"
#include "draw.h"
#include "mat.h"

#define con_type Line
#define con_prefix line
#include "array.h"
#undef con_type
#undef con_prefix

static Array_Line editor_colliders = NULL;
static vec2 player_start = svNzero;

void behavior_editor(Entity* e, Game* game, float dt) {
  PARAM_UNUSED(e);

  if (game->input.pressed.rmb) {
    vec2* pos = &game->camera.pos.xy;
    vec2 move = game->input.mouse.move;
    move.x *= -1;
    *pos = v2add(*pos, v2scale(move, dt));
  }

  static vec2 start_point = svNzero;
  static vec2 end_point = svNzero;

  // Get mouse direction in world space
  vec2 mouse_pos = game->input.mouse.pos;
  vec3 cursor = camera_screen_to_ray(&game->camera, game->window, mouse_pos);

  // project the vector from the camera to intersect the world xy plane
  float t;
  v3ray_plane(game->camera.pos.xyz, cursor, v3origin, v3z, &t);
  cursor = v3add(game->camera.pos.xyz, v3scale(cursor, t));

  // snap to grid
  if (game->input.pressed.shift) {
    cursor.x = floorf(cursor.x + 0.5f);
    cursor.y = floorf(cursor.y + 0.5f);
  }

  // draw the cursor so we know what we're doing
  draw.color = c4green;
  draw_circle(cursor, 0.25);

  if (game->input.pressed.mmb) {
    player_start = cursor.xy;
  }

  // draw the player start location
  draw.color = c4cyan;
  draw_circle(v23(v2add(player_start, v2f(0, 1.5))), 1.5);

  if (game->input.pressed.lmb) {
    end_point = cursor.xy;

    if (game->input.triggered.lmb) {
      start_point = cursor.xy;
    } else {
      draw_line_solid(v23(start_point), v23(end_point), c4magenta);
    }
  }

  if (game->input.released.lmb) {
    bool droppable = game->input.pressed.back;
    bool wall = game->input.pressed.right;
    bool bouncy = game->input.pressed.forward;

    arr_line_push_back(editor_colliders, (Line) {
      .a = start_point, .b = end_point,
      .bouncy = bouncy, .wall = wall, .droppable = droppable
    });

    // update count and reset collider data pointer in case it resized
    game->colliders = editor_colliders->first;
    game->collider_count = editor_colliders->size;
  }

  if (game->input.triggered.run_replay && editor_colliders->size) {
    arr_line_pop_back(editor_colliders);
    game->collider_count = editor_colliders->size;
  }
}

void level_load_editor(Game* game) {

  game->camera.pos = v4f(0, 0, 100, 1);
  game->camera.front = v4front;

  if (!editor_colliders) {
    editor_colliders = arr_line_new();
  }

  game->colliders = editor_colliders->first;
  game->collider_count = editor_colliders->size;

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
  if (!editor_colliders) {
    print("No level geometry to test:");
    print("First make a level in the level editor ('-' key to access)");
    return;
  }

  game->camera.pos = v4f(0, 0, 60, 1);
  game->camera.front = v4front;

  game->colliders = editor_colliders->first;
  game->collider_count = editor_colliders->size;

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
    .replay = NULL,
    .replay_temp = NULL,
    .render = render_sprites,
    .behavior = behavior_player,
    .delete = delete_player,
  });

  game->timeguys = arr_PlayerRef_new();
  arr_PlayerRef_push_back(game->timeguys, (PlayerRef){
    .start_frame = 0,
    .e = arr_ety_get_back_ref(game->entities)
  });

}
