#ifndef _MATRIX_H_
#define _MATRIX_H_

#include "vec.h"

typedef struct {
  union {
    float f[16];
    float m[4][4];
    vec4  col[4];
  };
} mat4;

const extern mat4 m4identity;
const extern mat4 m4zero;

#define m4identity ((mat4) {  \
  1, 0, 0, 0,                 \
  0, 1, 0, 0,                 \
  0, 0, 1, 0,                 \
  0, 0, 0, 1                  \
})                           //

#define m4zero ((mat4) {  \
  0, 0, 0, 0,             \
  0, 0, 0, 0,             \
  0, 0, 0, 0,             \
  0, 0, 0, 0              \
})                       //

mat4 m4ortho(
  float left, float right, float top, float bottom, float near, float far);
mat4 m4perspective(float fov_rads, float aspect, float near, float far);
mat4 m4basis(vec3 x, vec3 y, vec3 z, vec3 origin);
mat4 m4look(vec3 pos, vec3 target, vec3 up);
mat4 m4translation(vec3 vec);
mat4 m4rotation(vec3 axis, float angle);
mat4 m4scalar(vec3 scalar);
mat4 m4uniform(float scalar);
mat4 m4mul(mat4 a, mat4 b);
vec4 mv4mul(mat4 m, vec4 v);
mat4 m4inverse(mat4 mat);

#endif
