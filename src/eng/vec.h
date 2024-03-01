#ifndef _MATH_VECTOR_H_
#define _MATH_VECTOR_H_

#include "types.h"

typedef struct vec2i {
  union {
    int i[2];
    struct {
      union { int x; int w; };
      union { int y; int h; };
    };
  };
} vec2i;

typedef struct vec3i {
  union {
    int i[3];
    struct {
      union {        int r; int u; };
      union { int y; int g; int v; };
      union { int z; int b; int w; };
    };
    struct {
      int x;
      vec2i yz;
    };
    vec2i xy;
    vec2i uv;
  };
} vec3i;
typedef vec3i color3i;

typedef struct vec3b {
  union {
    byte i[3];
    struct {
      union { byte x; byte r; };
      union { byte y; byte g; };
      union { byte z; byte b; };
    };
  };
} vec3b;
typedef vec3b color3b;

typedef struct vec4b {
  union {
    byte i[4];
    struct {
      union { byte x; byte r; };
      union { byte y; byte g; };
      union { byte z; byte b; };
      union { byte w; byte a; };
    };
    vec3b xyz;
    vec3b rgb;
  };
} vec4b;
typedef vec4b color4b;

typedef struct vec2 {
  union {
    float f[2];
    struct {
      union { float x; float u; float w; };
      union { float y; float v; float h; };
    };
  };
} vec2;

typedef struct vec3 {
  union {
    float f[3];
    struct {
      union {          float r; float u; };
      union { float y; float g; float v; };
      union { float z; float b; float w; };
    };
    struct {
      float x;
      vec2 yz;
    };
    vec2 xy;
    vec2 uv;
  };
} vec3;
typedef vec3 color3;

typedef struct vec4 {
  union {
    float f[4];
    float row[4];
    struct {
      union {          float r; float i; };
      union { float y; float g; float j; };
      union { float z; float b; float k; };
      union { float w; float a; };
    };
    struct {
      float x;
      union {
        vec3 yzw;
        vec2 yz;
      };
    };
    struct {
      vec2 xy;
      vec2 zw;
    };
    vec3 xyz;
    vec3 rgb;
    vec3 ijk;
  };
} vec4;
typedef vec4 quat;
typedef vec4 color4;

  //     c d e f   h       l   n o p q   s t

extern const vec3b v3bzero;
extern const vec3i v3izero;
extern const vec3 v3zero;
extern const vec3 v3x;
extern const vec3 v3y;
extern const vec3 v3z;
extern const vec4 v4zero;
extern const vec4 v4x;
extern const vec4 v4y;
extern const vec4 v4z;
extern const vec4 v4origin;
extern const vec4 v4front;
extern const vec4 v4left;
extern const vec4 v4down;

//float v2dot(vec2 a, vec2 b);

vec3 qtransform(quat q, vec3 v);

float v2iaspect(vec2i v);
float v3mag(vec3 v);
float v3magsq(vec3 v);
vec3  v3norm(vec3 v);
vec3  v3add(vec3 a, vec3 b);
vec3  v3sub(vec3 a, vec3 b);
vec3  v3neg(vec3 v);
vec3  v3scale(vec3 a, float f);
vec3  v3wedge(vec3 a, vec3 b);
float v3dot(vec3 a, vec3 b);
vec3  v3cross(vec3 a, vec3 b);
//quat  v3mul(vec3 a, vec3 b);
float v3angle(vec3 a, vec3 b);

#endif
