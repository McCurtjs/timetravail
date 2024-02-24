#ifndef _WASM_SDL_H_
#define _WASM_SDL_H_

#include "../types.h"

typedef uint Uint32;
typedef unsigned long Uint64;
typedef int Sint32;
typedef uint SDL_WindowID;

typedef int SDL_bool;
#define SDL_TRUE 1
#define SDL_FALSE 0

typedef enum {
  SDL_EVENT_WINDOW_RESIZED = 0x206
} SDL_EventType;

/**
 *  Window state change event data (event.window.*)
 */
typedef struct SDL_WindowEvent
{
    Uint32 type;
    Uint64 timestamp;
    SDL_WindowID windowID;
    Sint32 data1;
    Sint32 data2;
} SDL_WindowEvent;

typedef union SDL_Event {
  Uint32 type;
  SDL_WindowEvent window;
} SDL_Event;

int SDL_PushEvent(SDL_Event* event);
SDL_bool SDL_PollEvent(SDL_Event* event);

#endif
