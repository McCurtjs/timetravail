#include "draw.h"

#include <GL/gl.h>

#include "array.h"

typedef struct DrawState {
  color3 color1;
  color3 color2;
  vec3 offset;
  float scale;
} DrawState;
static DrawState dstate;

typedef struct Vert {
  vec3    pos;
  color3  color;
} Vert;

static Array geometry = NULL;

static uint gl_vao = 0;
static uint gl_buffer = 0;

static void draw_init() {
  if (geometry) return;
  dstate = (DrawState){v3x, v3x, v3zero, 0.1};
  geometry = array_new_reserve(sizeof(Vert), 128);
}

static void draw_push(vec3 pos, color3 col) {
  draw_init();

  array_push_back(geometry, &(Vert){pos, col});
}

static void draw_init_gl() {
  if (gl_buffer) return;

  const void* color_offset = (void*)sizeof(vec3);

  glGenVertexArrays(1, &gl_vao);
  glBindVertexArray(gl_vao);
  glGenBuffers(1, &gl_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, gl_buffer);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vert), 0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vert), color_offset);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
}

// State functions

void draw_offset(vec3 offset) {
  dstate.offset = offset;
}

void draw_color(color3 color) {
  draw_colors(color, color);
}

void draw_colors(color3 color1, color3 color2) {
  dstate.color1 = color1;
  dstate.color2 = color2;
}

void draw_color_start(color3 color) {
  dstate.color1 = color;
}

void draw_color_end(color3 color) {
  dstate.color2 = color;
}

void draw_scale(float s) {
  dstate.scale = s;
}

// Draw functions

void draw_point(vec3 p) {
  vec3 a, b;
  float s = dstate.scale;
  vec3 x = v3scale(v3x, s), y = v3scale(v3y, s), z = v3scale(v3z, s);
  a = v3sub(p, x); b = v3add(p, x);
  draw_line_solid(a, b, dstate.color2);
  a = v3sub(p, y); b = v3add(p, y);
  draw_line_solid(a, b, dstate.color2);
  a = v3sub(p, z); b = v3add(p, z);
  draw_line_solid(a, b, dstate.color2);
}

void draw_circle(vec3 center, float radius) {
  vec3 prev = v3scale(v3x, radius);

  for (uint i = 0; i < 32; ++i) {
    vec3 next = v23f(v2rot(prev.xy, TAU / 32), 0);
    draw_line_solid(v3add(center, prev), v3add(center, next), dstate.color1);
    prev = next;
  }
}

void draw_line_solid(vec3 a, vec3 b, color3 color_override) {
  draw_push(a, color_override);
  draw_push(b, color_override);
}

void draw_line(vec3 a, vec3 b) {
  draw_push(a, dstate.color1);
  draw_push(b, dstate.color2);
}

void draw_vector(vec3 v) {
  vec3 target = v3add(dstate.offset, v);
  draw_line(dstate.offset, target);
  vec3 base = v3add(dstate.offset, v3scale(v, 1.0 - dstate.scale));
  float base_scale = v3mag(v) * dstate.scale * 0.75;
  vec3 perp = v3scale(v3norm(v3perp(v)), base_scale);
  vec3 pcrs = v3scale(v3norm(v3cross(v, perp)), base_scale);
  draw_line_solid(target, v3add(base, perp), dstate.color2);
  draw_line_solid(base,   v3add(base, perp), dstate.color2);
  draw_line_solid(target, v3sub(base, perp), dstate.color2);
  draw_line_solid(base,   v3sub(base, perp), dstate.color2);
  draw_line_solid(target, v3add(base, pcrs), dstate.color2);
  draw_line_solid(base,   v3add(base, pcrs), dstate.color2);
  draw_line_solid(target, v3sub(base, pcrs), dstate.color2);
  draw_line_solid(base,   v3sub(base, pcrs), dstate.color2);

  draw_line_solid(v3add(base, pcrs), v3add(base, perp), dstate.color2);
  draw_line_solid(v3sub(base, pcrs), v3sub(base, perp), dstate.color2);
  draw_line_solid(v3sub(base, pcrs), v3add(base, perp), dstate.color2);
  draw_line_solid(v3add(base, pcrs), v3sub(base, perp), dstate.color2);
}

void draw_dir(vec3 v) {
  draw_vector(v3norm(v));
}

void draw_rect(vec3 pos, vec3 a, vec3 b) {
  vec3 end = v3add(v3add(pos, a), b);
  draw_line(pos, v3add(pos, a));
  draw_line_solid(pos, v3add(pos, b), dstate.color1);
  draw_line(v3add(pos, b), end);
  draw_line_solid(v3add(pos, a), end, dstate.color2);
}

// Rendering and cleanup

void draw_render() {
  draw_init_gl();

  uint size_bytes = geometry->size_bytes;
  uint size_lines = geometry->size;
  Vert* draw_buffer = array_get_front(geometry);

  glBindVertexArray(gl_vao);
  glBindBuffer(GL_ARRAY_BUFFER, gl_buffer);
  glBufferData(GL_ARRAY_BUFFER, size_bytes, draw_buffer, GL_STATIC_DRAW);

  glDrawArrays(GL_LINES, 0, size_lines);

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  dstate = (DrawState){v3x, v3x, v3zero, 0.1};

  array_clear(geometry);
}

void draw_cleanup() {
  array_delete(&geometry);
  glDeleteBuffers(1, &gl_buffer);
  glDeleteVertexArrays(1, &gl_vao);
  gl_buffer = 0; gl_vao = 0;
}
