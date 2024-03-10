#include "system_events.h"

#include <SDL/sdl.h>
#include <GL/gl.h>

#include "wasm.h"

void process_system_events(Game* game) {
  SDL_Event event;

  while (SDL_PollEvent(&event)) {
    switch(event.type) {
      case SDL_EVENT_WINDOW_RESIZED: {
        game->window.w = event.window.data1;
        game->window.h = event.window.data2;
        glViewport(0, 0, game->window.w, game->window.h);
        float aspect = i2aspect(game->window);
        game->camera.persp.aspect = aspect;
        camera_build_perspective(&game->camera);
        //game->camera.ortho.left = -game->camera.ortho.top * aspect;
        //game->camera.ortho.right = -game->camera.ortho.bottom * aspect;
        //camera_build_orthographic(&game->camera);
      } break;

      case SDL_EVENT_MOUSE_BUTTON_DOWN: {
        if (event.button.button & SDL_BUTTON_LMASK) {
          game->input.pressed.lmb = TRUE;
          game->input.triggered.lmb = TRUE;
        }
        if (event.button.button & SDL_BUTTON_RMASK) {
          game->input.pressed.rmb = TRUE;
          game->input.triggered.rmb = TRUE;
        }
      } break;

      case SDL_EVENT_MOUSE_BUTTON_UP: {
        if (!(event.button.button & SDL_BUTTON_LMASK)) {
          game->input.pressed.lmb = FALSE;
          game->input.released.lmb = TRUE;
        }
        if (!(event.button.button & SDL_BUTTON_RMASK)) {
          game->input.pressed.rmb = FALSE;
          game->input.released.rmb = TRUE;
        }
      } break;

      case SDL_EVENT_MOUSE_MOTION: {
        float xrot = d2r(-event.motion.yrel * 180 / (float)game->window.h);
        float yrot = d2r(-event.motion.xrel * 180 / (float)game->window.x);

        if (game->input.pressed.lmb) {
          vec3 angles = (vec3){xrot, yrot, 0};
          camera_orbit(&game->camera, game->target, angles.xy);
        }

        if (game->input.pressed.rmb) {
          mat4 light_rotation = m4rotation(v3y, yrot);
          game->light_pos = mv4mul(light_rotation, game->light_pos);
        }
      } break;

      case SDL_EVENT_KEY_DOWN: {
        if (event.key.repeat) break;
        for (uint i = 0; i < game_key_count; ++i) {
          if (event.key.keysym.sym == game->input.mapping.keys[i]) {
            if (game->input.pressed.keys[i]) return;
            game->input.pressed.keys[i] = TRUE;
            game->input.triggered.keys[i] = TRUE;
          }
        }
      }; break;

      case SDL_EVENT_KEY_UP: {
        for (uint i = 0; i < game_key_count; ++i) {
          if (event.key.keysym.sym == game->input.mapping.keys[i]) {
            game->input.pressed.keys[i] = FALSE;
            game->input.released.keys[i] = TRUE;
          }
        }
      }; break;
    }
  }
}
