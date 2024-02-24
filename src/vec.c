#include "vec.h"

#include <math.h>

const vec4 v4zero = {0, 0, 0, 0};
const vec4 v4origin = {0, 0, 0, 1};

// shamelessly lifted from
// https://community.khronos.org/t/quaternion-functions-for-glsl/50140/2
vec3 qtransform(quat q, vec3 v) {
  return v3add(
    v, v3scale(
      v3cross(
        v3add(v3cross(v, q.ijk), v3scale(v, q.w)),
        q.ijk
      )
    , 2)
  );
  //return v + 2.0*cross(cross(v, q.xyz ) + q.w*v, q.xyz);
}

float v3magsq(vec3 v) {
  return v.x * v.x + v.y * v.y + v.z * v.z;
}

float v3mag(vec3 v) {
  return sqrt(v3magsq(v));
}

vec3 v3norm(vec3 v) {
  float mag = v3mag(v);
  return (vec3) {v.x / mag, v.y / mag, v.z / mag};
}

vec3 v3add(vec3 a, vec3 b) {
  return (vec3) { a.x + b.x, a.y + b.y, a.z + b.z };
}

vec3 v3sub(vec3 a, vec3 b) {
  return (vec3) { a.x - b.x, a.y - b.y, a.z - b.z };
}

vec3 v3scale(vec3 a, float f) {
  return (vec3) { a.x * f, a.y * f, a.z * f };
}

vec3  v3wedge(vec3 a, vec3 b) {
  return (vec3) { a.x * b.x, a.y * b.y, a.z * b.z };
}

float v3dot(vec3 a, vec3 b) {
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

vec3 v3cross(vec3 a, vec3 b) {
  return (vec3) {
    a.y * b.z - a.z * b.y,
    a.z * b.x - a.x * b.z,
    a.x * b.y - a.y * b.x
  };
}
