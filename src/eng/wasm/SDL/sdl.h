#ifndef _WASM_SDL_H_
#define _WASM_SDL_H_

#include "../types.h"

typedef uint Uint32;
typedef unsigned long Uint64;
typedef int Sint32;
typedef uint SDL_WindowID;
typedef byte Uint8;

typedef int SDL_bool;
#define SDL_TRUE 1
#define SDL_FALSE 0

typedef enum {
  SDL_EVENT_WINDOW_RESIZED = 0x206,

  SDL_EVENT_KEY_DOWN = 0x300,
  SDL_EVENT_KEY_UP,

  SDL_EVENT_MOUSE_MOTION    = 0x400,
  SDL_EVENT_MOUSE_BUTTON_DOWN,
  SDL_EVENT_MOUSE_BUTTON_UP,
  SDL_EVENT_MOUSE_WHEEL,
} SDL_EventType;

#define SDL_RELEASED 0
#define SDL_PRESSED 1

#define SDL_BUTTON(X)       (1 << ((X)-1))
#define SDL_BUTTON_LEFT     1
#define SDL_BUTTON_MIDDLE   3 // <- SDL and javascript
#define SDL_BUTTON_RIGHT    2 // <- swap these values
#define SDL_BUTTON_X1       4
#define SDL_BUTTON_X2       5
#define SDL_BUTTON_LMASK    SDL_BUTTON(SDL_BUTTON_LEFT)
#define SDL_BUTTON_MMASK    SDL_BUTTON(SDL_BUTTON_MIDDLE)
#define SDL_BUTTON_RMASK    SDL_BUTTON(SDL_BUTTON_RIGHT)
#define SDL_BUTTON_X1MASK   SDL_BUTTON(SDL_BUTTON_X1)
#define SDL_BUTTON_X2MASK   SDL_BUTTON(SDL_BUTTON_X2)

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

/**
 *  Mouse motion event structure (event.motion.*)
 */
typedef struct SDL_MouseMotionEvent
{
    Uint32 type;        /**< ::SDL_EVENT_MOUSE_MOTION */
    Uint64 timestamp;   /**< In nanoseconds, populated using SDL_GetTicksNS() */
    Uint32 state;       /**< The current button state */
    float x;            /**< X coordinate, relative to window */
    float y;            /**< Y coordinate, relative to window */
    float xrel;         /**< The relative motion in the X direction */
    float yrel;         /**< The relative motion in the Y direction */
} SDL_MouseMotionEvent;

/**
 *  Mouse button event structure (event.button.*)
 */
typedef struct SDL_MouseButtonEvent
{
    Uint32 type;
    Uint64 timestamp;   /**< In nanoseconds, populated using SDL_GetTicksNS() */
    Uint8 button;       /**< The mouse button index */
    Uint8 state;        /**< ::SDL_PRESSED or ::SDL_RELEASED */
    Uint8 clicks;       /**< 1 for single-click, 2 for double-click, etc. */
    float x;            /**< X coordinate, relative to window */
    float y;            /**< Y coordinate, relative to window */
} SDL_MouseButtonEvent;

typedef union SDL_Event {
  Uint32 type;
  SDL_WindowEvent window;
  SDL_MouseMotionEvent motion;
  SDL_MouseButtonEvent button;

  Uint8 padding[32];
} SDL_Event;

int SDL_PushEvent(SDL_Event* event);
SDL_bool SDL_PollEvent(SDL_Event* event);

#endif
