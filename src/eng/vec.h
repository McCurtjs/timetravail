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
      union { float x; float w; float u; float s; float p; float i; };
      union { float y; float h; float v; float t; float q; float r; };
    };
  };
} vec2;
typedef vec2 inum;

typedef struct vec3 {
  union {
    float f[3];
    struct {
      union {          float r; float u; };
      union { float y; float g; float v; };
      union { float z; float b; float w; float t; };
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
      union {          float r; float i; float u; float p; };
      union { float y; float g; float j; float v; float q; };
      union { float z; float b; float k; float s; };
      union { float w; float a;          float t; };
    };
    struct {
      float x;
      union {
        vec3 yzw;
        vec2 yz;
      };
    };
    struct {
      union { vec2 xy; vec2 uv; vec2 pq; };
      union { vec2 zw; vec2 st; };
    };
    vec3 xyz;
    vec3 rgb;
    vec3 ijk;
  };
} vec4;
typedef vec4 quat;
typedef vec4 color4;

  //     c d e     h       l   n o

// Size constants (generally for OpenGL functions)
#define v2floats  2
#define v3floats  3
#define v4floats  4
#define i2ints    2
#define i3ints    3
#define v2bytes   (sizeof(vec2))
#define v3bytes   (sizeof(vec3))
#define v4bytes   (sizeof(vec4))
#define i2bytes   (sizeof(vec2i))
#define i3bytes   (sizeof(vec3i))
#define b3bytes   (sizeof(color3b))
#define b4bytes   (sizeof(color4b))

// Vector constants
#define b3zero    ((vec3b){ 0, 0, 0 })
#define i3zero    ((vec3i){ 0, 0, 0 })
#define v2ones    ((vec2){ 1, 1 })
#define v2zero    ((vec2){ 0, 0 })
#define v2x       ((vec2){ 1, 0 })
#define v2y       ((vec2){ 0, 1 })
#define v2right   ((vec2){ 1, 0 })
#define v2up      ((vec2){ 0, 1 })
#define v2left    ((vec2){-1, 0 })
#define v2down    ((vec2){ 0,-1 })
#define v3ones    ((vec3){ 1, 1, 1 })
#define v3zero    ((vec3){ 0, 0, 0 })
#define v3origin  ((vec3){ 0, 0, 0 })
#define v3x       ((vec3){ 1, 0, 0 })
#define v3y       ((vec3){ 0, 1, 0 })
#define v3z       ((vec3){ 0, 0, 1 })
#define v3right   ((vec3){ 1, 0, 0 })
#define v3up      ((vec3){ 0, 1, 0 })
#define v3back    ((vec3){ 0, 0, 1 })
#define v3left    ((vec3){-1, 0, 0 })
#define v3down    ((vec3){ 0,-1, 0 })
#define v3front   ((vec3){ 0, 0,-1 })
#define v4ones    ((vec4){ 1, 1, 1, 0 })
#define v4zero    ((vec4){ 0, 0, 0, 0 })
#define v4x       ((vec4){ 1, 0, 0, 0 })
#define v4y       ((vec4){ 0, 1, 0, 0 })
#define v4z       ((vec4){ 0, 0, 1, 0 })
#define v4w       ((vec4){ 0, 0, 0, 1 })
#define v4right   ((vec4){ 1, 0, 0, 0 })
#define v4up      ((vec4){ 0, 1, 0, 0 })
#define v4back    ((vec4){ 0, 0, 1, 0 })
#define v4left    ((vec4){-1, 0, 0, 0 })
#define v4down    ((vec4){ 0,-1, 0, 0 })
#define v4front   ((vec4){ 0, 0,-1, 0 })
#define p4ones    ((vec4){ 1, 1, 1, 1 })
#define p4origin  ((vec4){ 0, 0, 0, 1 })
#define c4black   ((vec4){ 0, 0, 0, 1 })
#define c4red     ((vec4){ 1, 0, 0, 1 })
#define c4green   ((vec4){ 0, 1, 0, 1 })
#define c4blue    ((vec4){ 0, 0, 1, 1 })
#define c4yellow  ((vec4){ 1, 1, 0, 1 })
#define c4cyan    ((vec4){ 0, 1, 1, 1 })
#define c4magenta ((vec4){ 1, 0, 1, 1 })
#define c4white   ((vec4){ 1, 1, 1, 1 })
#define c4gray    ((vec4){.5,.5,.5, 1 })
#define b4black   ((vec4b){   0,   0,   0, 255 })
#define b4red     ((vec4b){ 255,   0,   0, 255 })
#define b4green   ((vec4b){   0, 255,   0, 255 })
#define b4blue    ((vec4b){   0,   0, 255, 255 })
#define b4yellow  ((vec4b){ 255, 255,   0, 255 })
#define b4cyan    ((vec4b){   0, 255, 255, 255 })
#define b4magenta ((vec4b){ 255,   0, 255, 255 })
#define b4white   ((vec4b){ 255, 255, 255, 255 })
#define b4gray    ((vec4b){ 128, 128, 128, 255 })

vec3 qtransform(quat q, vec3 v);

float i2aspect(vec2i v);

float v2mag(vec2 v);
float v2magsq(vec2 v);
float v2dist(vec2 P, vec2 Q);
float v2distsq(vec2 P, vec2 Q);
vec2  v2norm(vec2 v);
vec2  v2neg(vec2 v);
vec2  v2add(vec2 a, vec2 b);
vec2  v2sub(vec2 a, vec2 b);
vec2  v2scale(vec2 v, float f);
float v2dot(vec2 a, vec2 b);
vec2  v2had(vec2 a, vec2 b);
float v2cross(vec2 a, vec2 b);
vec2  v2perp(vec2 v);
vec2  v2reflect(vec2 a, vec2 b);
float v2angle(vec2 a, vec2 b);
vec2  v2dir(float theta);
vec2  v2rot(vec2 v, float theta);
vec2  v2lerp(vec2 P, vec2 Q, float t);
float v2line_dist(vec2 P, vec2 v, vec2 Q);
float v2line_closest(vec2 P, vec2 v, vec2 Q, vec2* R_out);
bool  v2line_line(vec2 P, vec2 v, vec2 Q, vec2 u, float* t_out, float* s_out);
bool  v2ray_line(vec2 P, vec2 v, vec2 Q, vec2 u, float* t_out);
bool  v2ray_ray(vec2 P, vec2 v, vec2 Q, vec2 u, float* t_out, float* s_out);
bool  v2ray_seg(vec2 P, vec2 v, vec2 Q1, vec2 Q2, float* t_out);
bool  v2seg_seg(vec2 P1, vec2 P2, vec2 Q1, vec2 Q2, vec2* out);
vec3  v23(vec2 xy);
vec4  v24(vec2 xy);
vec3  v23f(vec2 xy, float z);
vec4  v24f(vec2 xy, float z, float w);

float v3mag(vec3 v);
float v3magsq(vec3 v);
vec3  v3norm(vec3 v);
vec3  v3neg(vec3 v);
vec3  v3add(vec3 a, vec3 b);
vec3  v3sub(vec3 a, vec3 b);
vec3  v3scale(vec3 a, float f);
float v3dot(vec3 a, vec3 b);
vec3  v3had(vec3 a, vec3 b);
vec3  v3cross(vec3 a, vec3 b);
vec3  v3perp(vec3 v);
float v3angle(vec3 a, vec3 b);
bool  v3line_plane(vec3 P, vec3 v, vec3 R, vec3 n, float* t_out);
bool  v3ray_plane(vec3 P, vec3 v, vec3 R, vec3 n, float* t_out);
vec4  v34(vec3 xyz);
vec4  v34f(vec3 xyz, float w);

//vec2  v2orbit(vec2 a, vec2 center, float theta);
//vec3  v3reflect(vec3 v, vec3 axis);
//vec3  v3rot(vec3 v, vec3 axis, float theta);

#endif
