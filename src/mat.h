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

mat4 m4ortho(
  float left, float right, float top, float bottom, float near, float far);
mat4 m4perspective(float fov_rads, float aspect, float near, float far);
mat4 m4translation(vec3 vec);
mat4 m4rotation(vec3 axis, float angle);
mat4 m4scalar(mat4 mat, vec3 scaler);
mat4 m4uniform(mat4 mat, float scalar);
mat4 m4mul(mat4 a, mat4 b);
vec4 mv4mul(mat4 m, vec4 v);
mat4 m4inverse(mat4 mat);

#endif
