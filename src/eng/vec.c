#include "vec.h"

#include <math.h>

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

float i2aspect(vec2i v) {
  return (float)v.w / (float)v.h;
}

float v2mag(vec2 v) {
  return sqrt(v2magsq(v));
}

float v2magsq(vec2 v) {
  return v.x * v.x + v.y * v.y;
}

vec2 v2norm(vec2 v) {
  float mag = v2mag(v);
  return (vec2){v.x / mag, v.y / mag};
}

vec2 v2neg(vec2 v) {
  return (vec2){-v.x, -v.y};
}

vec2 v2add(vec2 a, vec2 b) {
  return (vec2){a.x + b.x, a.y + b.y};
}

vec2 v2sub(vec2 a, vec2 b) {
  return (vec2){a.x - b.x, a.y - b.y};
}

vec2 v2scale(vec2 v, float f) {
  return (vec2){v.x * f, v.y * f};
}

float v2dot(vec2 a, vec2 b) {
  return a.x * b.x + a.y * b.y;
}

vec2 v2wedge(vec2 a, vec2 b) {
  return (vec2){a.x * b.x, a.y * b.y};
}

float v2cross(vec2 a, vec2 b) {
  return a.x * b.y - a.y * b.x;
}

vec2 v2perp(vec2 v) {
  return (vec2){-v.y, v.x};
}

float v2angle(vec2 a, vec2 b) {
  return acosf(v2dot(a, b) / (v2mag(a) * v2mag(b)));
}

vec2 v2dir(float theta) {
  float sint, cost;
  //sincosf(theta, &sint, &cost);
  sint = sinf(theta); cost = sinf(theta);
  return (vec2){cost, sint};
};

vec2 v2rot(vec2 v, float theta) {
  float sint, cost;
  //sincosf(theta, &sint, &cost);
  sint = sinf(theta); cost = sinf(theta);
  return (vec2){cost * v.x - sint * v.y, sint * v.x + cost * v.y};
}

vec2 v2lerp(vec2 p1, vec2 p2, float t) {
  vec2 v = v2scale(v2sub(p2, p1), t);
  return v2add(p1, v);
}

// t = (P.x * u.y - P.y * u.x + u.x * Q.y - u.y * Q.x) / (u.x * v.y - u.y * v.x)
// s = (v.x * P.y - v.y * P.x + Q.x * v.y - Q.y * v.x) / (v.x * u.y - v.y * u.x)
bool v2line_line(vec2 P, vec2 v, vec2 Q, vec2 u, float* t_out, float* s_out) {
  float div = v2cross(u, v);
  if (div == 0) return FALSE;
  float t = (v2cross(P, u) + v2cross(u, Q)) / div;
  if (t_out) *t_out = t;
  if (!s_out) return TRUE;
  *s_out = (v2cross(v, P) + v2cross(Q, v)) / -div;
  return TRUE;
}

bool v2ray_line(vec2 P, vec2 v, vec2 Q, vec2 u, float* t_out) {
  float t;
  if (!v2line_line(P, v, Q, u, &t, NULL)) return FALSE;
  if (t_out) *t_out = t;
  if (t < 0) return FALSE;
  return TRUE;
}

bool v2ray_ray(vec2 P, vec2 v, vec2 Q, vec2 u, float* t_out, float* s_out) {
  float t, s;
  if (!v2line_line(P, v, Q, u, &t, &s)) return FALSE;
  if (t_out) *t_out = t;
  if (s_out) *s_out = s;
  if (t < 0) return FALSE;
  if (s < 0) return FALSE;
  return TRUE;
}

bool v2ray_seg(vec2 P, vec2 v, vec2 Q1, vec2 Q2, float* t_out) {
  vec2 u = v2sub(Q2, Q1);
  float t, s; float* t_ptr = t_out ? t_out : &t;
  if (!v2ray_ray(P, v, Q1, u, t_ptr, &s)) return FALSE;
  if (s > 1) return FALSE;
  return TRUE;
}

bool v2seg_seg(vec2 P1, vec2 P2, vec2 Q1, vec2 Q2, vec2* out) {
  vec2 v = v2sub(P2, P1), u = v2sub(Q2, Q1);
  float t, s;
  if (!v2ray_ray(P1, v, Q1, u, &t, &s)) return FALSE;
  if (t > 1) return FALSE;
  if (s > 1) return FALSE;
  if (out) *out = v2add(P1, v2scale(v, t));
  return TRUE;
}

vec3 v2v3(vec2 v, float z) {
  return (vec3){v.x, v.y, z};
}

vec4 v2v4(vec2 v, float z, float w) {
  return (vec4){v.x, v.y, z, w};
}

float v3mag(vec3 v) {
  return sqrt(v3magsq(v));
}

float v3magsq(vec3 v) {
  return v.x * v.x + v.y * v.y + v.z * v.z;
}

vec3 v3norm(vec3 v) {
  float mag = v3mag(v);
  return (vec3) {v.x / mag, v.y / mag, v.z / mag};
}

vec3 v3neg(vec3 v) {
  return (vec3) { -v.x, -v.y, -v.z };
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

float v3dot(vec3 a, vec3 b) {
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

vec3  v3wedge(vec3 a, vec3 b) {
  return (vec3) { a.x * b.x, a.y * b.y, a.z * b.z };
}

vec3 v3cross(vec3 a, vec3 b) {
  return (vec3) {
    a.y * b.z - a.z * b.y,
    a.z * b.x - a.x * b.z,
    a.x * b.y - a.y * b.x
  };
}

// From Ken Whatmough's post on
// https://math.stackexchange.com/questions/137362/how-to-find-perpendicular-vector-to-another-vector
vec3 v3perp(vec3 v) {
  return (vec3) {
    copysign(v.z, v.x),
    copysign(v.z, v.y),
    -copysign(fabs(v.x) + fabs(v.y), v.z)
    // or -copysign(v.x, v.z) - copysign(v.y, v.z)
  };
}

float v3angle(vec3 a, vec3 b) {
  return acosf(v3dot(a, b) / (v3mag(a) * v3mag(b)));
}

vec4 v3v4(vec3 v, float w) {
  return (vec4){v.x, v.y, v.z, w};
}
