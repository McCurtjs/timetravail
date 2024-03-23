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

#define CV const volatile

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

#define d2r(DEG)  ((DEG) * PI / 180.0f)
#define isPow2(n) ((n & (n-1)) == 0)

#define loop while (TRUE)
#define until(condition) if (condition) break;
#define unless(condition) if (!(condition))

#define MACRO_CONCAT_RECUR(X, Y) X ## Y
#define MACRO_CONCAT(X, Y) MACRO_CONCAT_RECUR(X, Y)

#endif
