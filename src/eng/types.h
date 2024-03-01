#ifndef _TYPES_H_
#define _TYPES_H_

typedef unsigned int uint;
typedef unsigned int jshandle;
typedef unsigned char byte;

#ifndef NULL
#define NULL 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef SQRT2
#define SQRT2 1.41421356237
#endif

#ifndef PI
#define PI 3.14159265358979323846264338
#endif
#define TAU (2 * PI)

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#define d2r(DEG)  ((DEG) * PI / 180.0)
#define isPow2(n) ((n & (n-1)) == 0)

#endif
