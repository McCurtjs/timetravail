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
        game->camera.persp.aspect = game->window.w / (float)game->window.h;
        camera_build_perspective(&game->camera);
      } break;

      case SDL_EVENT_MOUSE_BUTTON_DOWN: {
        game->button_down = event.button.button;
      } break;

      case SDL_EVENT_MOUSE_BUTTON_UP: {
        game->button_down = event.button.button;
      } break;

      case SDL_EVENT_MOUSE_MOTION: {
        float xrot = d2r(-event.motion.yrel * 180 / (float)game->window.h);
        float yrot = d2r(-event.motion.xrel * 180 / (float)game->window.x);

        if (game->button_down & SDL_BUTTON_LMASK) {
          vec3 angles = (vec3){xrot, yrot, 0};
          camera_orbit(&game->camera, game->target, angles.xy);
        }

        if (game->button_down & SDL_BUTTON_RMASK) {
          mat4 light_rotation = m4rotation(v3y, yrot);
          game->light_pos = mv4mul(light_rotation, game->light_pos);
        }
      } break;

      case SDL_EVENT_KEY_DOWN: {
        if (event.key.repeat) break;
        else if (event.key.keysym.sym == 'w') game->forward_down = TRUE;
        else if (event.key.keysym.sym == 'a') game->left_down = TRUE;
        else if (event.key.keysym.sym == 's') game->back_down = TRUE;
        else if (event.key.keysym.sym == 'd') game->right_down = TRUE;
      }; break;

      case SDL_EVENT_KEY_UP: {
        if      (event.key.keysym.sym == 'w') game->forward_down = FALSE;
        else if (event.key.keysym.sym == 'a') game->left_down = FALSE;
        else if (event.key.keysym.sym == 's') game->back_down = FALSE;
        else if (event.key.keysym.sym == 'd') game->right_down = FALSE;
      }; break;
    }
  }
}
