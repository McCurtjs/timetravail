#ifndef _TYPES_H_
#define _TYPES_H_

#ifdef __WASM__
# ifndef __DEFINED_size_t
#  define __DEFINED_size_t
typedef unsigned long size_t;
# endif


# ifndef __has_builtin
#  define __has_builtin(x) 0
# endif

// Provide assert that works with wasm...
# if __has_builtin(__builtin_trap)
#  define assert(CONDITION) (!(CONDITION) ? __builtin_trap() : 0);
# else
#  define assert(CONDITION)
# endif
#else
# include <corecrt.h>
# include <assert.h>
#endif

typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned short u16;
typedef unsigned char byte;
typedef size_t jshandle;

#ifndef NULL
# define NULL ((void*)0)
#endif

#include <stdbool.h>

#ifndef TRUE
# define TRUE true
# define FALSE false
#endif

#ifndef SQRT2
# define SQRT2 1.41421356237
#endif

#ifndef PI
# define PI 3.14159265358979323846264338f
#endif

#ifndef TAU
# define TAU (2 * PI)
#endif

#ifndef MAX
# define MAX(a, b) ((a) > (b) ? (a) : (b))
# define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

// Converts from degrees into radians
#define d2r(DEG)  ((DEG) * PI / 180.0f)

// True if an integer value is a power of 2
#define isPow2(n) ((n & (n-1)) == 0)

// Allows writing loops in the form:
//    loop {
//      // setup that happens on every iteration but always at least once
//      until (condition);
//      // do stuff each iteration after the conditional check
//    }

#ifndef loop
# define loop while (TRUE)
# define until(condition) if (condition) break;
#endif

#ifndef unless
# define unless(condition) if (!(condition))
#endif

#ifndef MACRO_CONCAT
# define MACRO_CONCAT_RECUR(X, Y) X ## Y
# define MACRO_CONCAT(X, Y) MACRO_CONCAT_RECUR(X, Y)
#endif

#ifndef STR
# define STR_RECUR(S) #S
# define STR(S) STR_RECUR(S)
#endif

// Squelches warnings about unused parameters.
// Ideally, for GCC and Clang this should be __attribute__((unused)) in the
//    function declaration, but there's no equivalent for MSVC that would work
//    there. C++ supports omitting the name, but C does not. Casting the param
//    to void in the function body works on all three with max level warnings.
#define PARAM_UNUSED(PARAM) (void)PARAM;

// Using this in container classes for return values that act as properties
// Is this a bad pattern? Probably, but it's an idea I'm trying out.
#define CV const volatile

#endif
