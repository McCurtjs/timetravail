#include "../SDL/sdl.h"
#include "../wasm.h"
#include "../types.h"
#include <stdlib.h>

void export(wasm_push_window_event) (uint event_type, int x, int y) {
  SDL_WindowEvent event;

  event.windowID = 1;
  event.timestamp = 0;
  event.type = event_type;
  event.data1 = x;
  event.data2 = y;

  SDL_PushEvent((SDL_Event*)&event);
}
