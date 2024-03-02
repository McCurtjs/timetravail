#include "draw.h"

#include <stdlib.h>
#include <memory.h>
#include <GL/gl.h>

typedef struct DrawState {
  color3 color1;
  color3 color2;
  vec3 offset;
  float scale;
} DrawState;
static DrawState draw_state;

typedef struct Vert {
  vec3    pos;
  color3  color;
} Vert;

static Vert* geometry = NULL;
static uint geom_size = 0;
static uint geom_tail = 0;
static const uint page_size = 32; // room for 16 lines

static uint gl_vao = 0;
static uint gl_buffer = 0;

static void draw_init() {
  if (geometry) return;

  draw_state = (DrawState){v3x, v3x, v3zero, 0.1};

  geom_size = page_size;
  geom_tail = 0;
  geometry = malloc(sizeof(Vert) * geom_size);
}

static void draw_grow() {
  uint old_size = geom_size;
  geom_size += page_size;

  Vert* old_geom = geometry;
  geometry = malloc(sizeof(Vert) * geom_size);
  memcpy(geometry, old_geom, old_size);
  free(old_geom);
}

static void draw_push(vec3 pos, color3 col) {
  draw_init();

  if (geom_tail >= geom_size) {
    draw_grow();
  }

  geometry[geom_tail++] = (Vert){pos, col};
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
  draw_state.offset = offset;
}

void draw_color(color3 color) {
  draw_colors(color, color);
}

void draw_colors(color3 color1, color3 color2) {
  draw_state.color1 = color1;
  draw_state.color2 = color2;
}

void draw_color_start(color3 color) {
  draw_state.color1 = color;
}

void draw_color_end(color3 color) {
  draw_state.color2 = color;
}

void draw_scale(float s) {
  draw_state.scale = s;
}

// Draw functions

void draw_point(vec3 p) {
  vec3 a, b;
  float s = draw_state.scale;
  vec3 x = v3scale(v3x, s), y = v3scale(v3y, s), z = v3scale(v3z, s);
  a = v3sub(p, x); b = v3add(p, x);
  draw_line_solid(a, b, draw_state.color2);
  a = v3sub(p, y); b = v3add(p, y);
  draw_line_solid(a, b, draw_state.color2);
  a = v3sub(p, z); b = v3add(p, z);
  draw_line_solid(a, b, draw_state.color2);
}

void draw_line_solid(vec3 a, vec3 b, color3 color_override) {
  draw_push(a, color_override);
  draw_push(b, color_override);
}

void draw_line(vec3 a, vec3 b) {
  draw_push(a, draw_state.color1);
  draw_push(b, draw_state.color2);
}

void draw_vector(vec3 v) {
  vec3 target = v3add(draw_state.offset, v);
  draw_line(draw_state.offset, target);
  vec3 base = v3add(draw_state.offset, v3scale(v, 1.0 - draw_state.scale));
  float base_scale = v3mag(v) * draw_state.scale * 0.75;
  vec3 perp = v3scale(v3norm(v3perp(v)), base_scale);
  vec3 pcrs = v3scale(v3norm(v3cross(v, perp)), base_scale);
  draw_line_solid(target, v3add(base, perp), draw_state.color2);
  draw_line_solid(base,   v3add(base, perp), draw_state.color2);
  draw_line_solid(target, v3sub(base, perp), draw_state.color2);
  draw_line_solid(base,   v3sub(base, perp), draw_state.color2);
  draw_line_solid(target, v3add(base, pcrs), draw_state.color2);
  draw_line_solid(base,   v3add(base, pcrs), draw_state.color2);
  draw_line_solid(target, v3sub(base, pcrs), draw_state.color2);
  draw_line_solid(base,   v3sub(base, pcrs), draw_state.color2);

  draw_line_solid(v3add(base, pcrs), v3add(base, perp), draw_state.color2);
  draw_line_solid(v3sub(base, pcrs), v3sub(base, perp), draw_state.color2);
  draw_line_solid(v3sub(base, pcrs), v3add(base, perp), draw_state.color2);
  draw_line_solid(v3add(base, pcrs), v3sub(base, perp), draw_state.color2);
}

void draw_dir(vec3 v) {
  draw_vector(v3norm(v));
}

void draw_rect(vec3 pos, vec3 a, vec3 b) {
  vec3 end = v3add(v3add(pos, a), b);
  draw_line(pos, v3add(pos, a));
  draw_line_solid(pos, v3add(pos, b), draw_state.color1);
  draw_line(v3add(pos, b), end);
  draw_line_solid(v3add(pos, a), end, draw_state.color2);
}

// Rendering and cleanup

void draw_render() {
  draw_init_gl();
  uint size_bytes = geom_size * sizeof(Vert);

  glBindVertexArray(gl_vao);
  glBindBuffer(GL_ARRAY_BUFFER, gl_buffer);
  glBufferData(GL_ARRAY_BUFFER, size_bytes, geometry, GL_STATIC_DRAW);

  glDrawArrays(GL_LINES, 0, geom_tail);

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  draw_state = (DrawState){v3x, v3x, v3zero, 0.1};
  geom_tail = 0;
}

void draw_cleanup() {
  glDeleteBuffers(1, &gl_buffer);
  glDeleteVertexArrays(1, &gl_vao);
  free(geometry);
  geometry = 0;
  geom_size = 0;
  geom_tail = 0;
}
