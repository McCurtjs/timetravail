#include "../SDL/sdl.h"
#include "../types.h"
#include "../wasm.h"
#include <stdlib.h>

void export(wasm_push_window_event) (uint event_type, int x, int y) {
  SDL_WindowEvent event;

  event.type = event_type;
  event.windowID = 1;
  event.timestamp = 0;
  event.data1 = x;
  event.data2 = y;

  SDL_PushEvent((SDL_Event*)&event);
}

void export(wasm_push_mouse_button_event) (
  uint event_type, byte button, float x, float y
) {
  SDL_MouseButtonEvent event;

  byte state =
    event_type == SDL_EVENT_MOUSE_BUTTON_DOWN ? SDL_PRESSED : SDL_RELEASED;

  event.type = event_type;
  event.button = button;
  event.state = state;
  event.x = x;
  event.y = y;

  SDL_PushEvent((SDL_Event*)&event);
}

void export(wasm_push_mouse_motion_event) (
  float x, float y, float xrel, float yrel
) {
  SDL_MouseMotionEvent event;

  event.type = SDL_EVENT_MOUSE_MOTION;
  event.state = 0; // will figure this out later
  event.x = x;
  event.y = y;
  event.xrel = xrel;
  event.yrel = yrel;

  SDL_PushEvent((SDL_Event*)&event);
}
