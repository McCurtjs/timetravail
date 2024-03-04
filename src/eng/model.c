#include "model.h"

#include "GL/gl.h"

#include <stdlib.h>

// inlined static data declarations for model primitives
#include "data/inline_primitives.h"

// Shared bindings for primitives

static GLuint cube_pos_buffer = 0;
static void prim_bind_cube() {
  if (!cube_pos_buffer) {
    glGenBuffers(1, &cube_pos_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, cube_pos_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_pos), cube_pos, GL_STATIC_DRAW);
  } else {
    glBindBuffer(GL_ARRAY_BUFFER, cube_pos_buffer);
  }
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, sizeof(*cube_pos) / 4, GL_FLOAT, GL_FALSE, 0, 0);
}

static GLuint cube_color_buffer = 0;
static GLuint cube_color_vao = 0;
static void prim_bind_cube_color() {
  if (!cube_color_buffer) {
    GLsizeiptr size = sizeof(cube_color);
    glGenBuffers(1, &cube_color_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, cube_color_buffer);
    glBufferData(GL_ARRAY_BUFFER, size, cube_color, GL_STATIC_DRAW);
  } else {
    glBindBuffer(GL_ARRAY_BUFFER, cube_color_buffer);
  }
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, sizeof(*cube_color) / 4, GL_FLOAT, GL_FALSE, 0, 0);
}

static GLuint cube_vertex_buffer = 0;
static GLuint cube_vao = 0;
static void prim_bind_cube_2() {
  if (!cube_vertex_buffer) {
    GLsizeiptr size = sizeof(cube2_verts);
    glGenBuffers(1, &cube_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, cube_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, size, cube2_verts, GL_STATIC_DRAW);
  } else {
    glBindBuffer(GL_ARRAY_BUFFER, cube_vertex_buffer);
  }
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);

  glEnableVertexAttribArray(1);
  const void* offset = (void*)12;
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), offset);

  glEnableVertexAttribArray(2);
  offset = (void*)24;
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), offset);
}

// Model_Grid

static int model_build_grid(Model_Grid* grid) {
  if (grid->ready) return 1;

  glGenVertexArrays(1, &grid->vao);
  glBindVertexArray(grid->vao);

  int ext = grid->extent;
  int gext = ext;

  if (ext <= 0) {
    grid->points_count = 6;
    gext = -ext > 1 ? -ext : 1;
  } else {
    grid->points_count = 12 + 8 * ext;
  }

  vec3* points = malloc(sizeof(vec3) * grid->points_count);
  color3b* colors = malloc(sizeof(color3b) * grid->points_count);

  uint i = 0;
  const vec3* basis = grid->basis;
  const byte  ga = MIN(grid->primary[0], 2);
  const byte  gb = MIN(grid->primary[1], 2);

  for (int j = 0; i < 6; i += 2, ++j) {
    colors[i] = b3zero;
    points[i] = points[i+1] = v3zero;
    colors[i].i[j] = 255;
    colors[i+1].i[j] = colors[i].i[j];
    points[i] = v3scale(basis[j], gext);
  }

  if (ext > 0) {
    for (uint j = 0; i < 12; i += 2, ++j) {
      colors[i] = colors[i+1] = (color3b){255, 255, 255};
      points[i] = points[i+1] = v3zero;
      points[i] = v3scale(basis[j], -ext);
    }

    for (int j = 1; i < grid->points_count; i += 8, ++j) {
      // (x * ext) + (y * j)
      points[i+0] = v3add(v3scale(basis[ga], ext), v3scale(basis[gb], j));
      points[i+1] = v3add(v3scale(basis[ga],-ext), v3scale(basis[gb], j));
      points[i+2] = v3add(v3scale(basis[ga], ext), v3scale(basis[gb],-j));
      points[i+3] = v3add(v3scale(basis[ga],-ext), v3scale(basis[gb],-j));
      points[i+4] = v3add(v3scale(basis[ga], j), v3scale(basis[gb], ext));
      points[i+5] = v3add(v3scale(basis[ga], j), v3scale(basis[gb],-ext));
      points[i+6] = v3add(v3scale(basis[ga],-j), v3scale(basis[gb], ext));
      points[i+7] = v3add(v3scale(basis[ga],-j), v3scale(basis[gb],-ext));

      byte c = (j % 10 == 0 ? 128 : (j % 5 == 0 ? 0 : 63));
      color3b color = (color3b){c, c, c};
      for (uint k = 0; k < 8; ++k) {
        colors[i + k] = color;
      }
    }
  }

  GLsizeiptr points_size = sizeof(*points) * grid->points_count;
  glGenBuffers(2, grid->buffers);
  glBindBuffer(GL_ARRAY_BUFFER, grid->buffers[0]);
  glBufferData(GL_ARRAY_BUFFER, points_size, points, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);

  GLsizeiptr colors_size = sizeof(*colors) * grid->points_count;
  glBindBuffer(GL_ARRAY_BUFFER, grid->buffers[1]);
  glBufferData(GL_ARRAY_BUFFER, colors_size, colors, GL_STATIC_DRAW);
  glVertexAttribPointer(1, sizeof(*colors), GL_UNSIGNED_BYTE, GL_TRUE, 0, 0);
  glEnableVertexAttribArray(1);

  free(points);
  free(colors);

  glBindVertexArray(0);
  grid->ready = TRUE;
  return 1;
}

static void model_render_grid(const Model_Grid* grid) {
  glBindVertexArray(grid->vao);
  glDrawArrays(GL_LINES, 0, grid->points_count);
  glBindVertexArray(0);
}

// Model_Cube

static int model_build_cube(Model_Cube* cube) {
  if (cube_vao) return 1;
  glGenVertexArrays(1, &cube_vao);
  glBindVertexArray(cube_vao);
  prim_bind_cube_2();
  glBindVertexArray(0);
  cube->ready = TRUE;
  return 1;
}

static void model_render_cube(const Model_Cube* _) {
  glBindVertexArray(cube_vao);
  glDrawArrays(GL_TRIANGLES, 0, 36);
  glBindVertexArray(0);
}

// Model_CubeColor

static int model_build_cube_color(Model_CubeColor* cube) {
  if (cube_color_vao) return 1;
  glGenVertexArrays(1, &cube_color_vao);
  glBindVertexArray(cube_color_vao);
  prim_bind_cube();
  prim_bind_cube_color();
  glBindVertexArray(0);
  cube->ready = TRUE;
  return 1;
}

static void model_render_cube_color(const Model_CubeColor* _) {
  glBindVertexArray(cube_color_vao);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 14);
  glBindVertexArray(0);
}

// Exported functions

typedef int  (*model_build_pfn)(void* model);
typedef void (*model_render_pfn)(const void* model);

static model_build_pfn model_build_fns[MODEL_TYPES_COUNT] = {
  (model_build_pfn)model_build_grid,
  (model_build_pfn)model_build_cube,
  (model_build_pfn)model_build_cube_color
};

int model_build(Model* model) {
  if (!model || !model->type) return 0;

  return model_build_fns[model->type - 1](model);
}

static model_render_pfn model_render_fns[MODEL_TYPES_COUNT] = {
  (model_render_pfn)model_render_grid,
  (model_render_pfn)model_render_cube,
  (model_render_pfn)model_render_cube_color
};

void model_render(const Model* model) {
  if (!model || !model->type || !model->ready) return;

  model_render_fns[model->type - 1](model);
}

void model_setup_default_grid(Model* model, int extent) {
  model->grid = (Model_Grid) {
    .type = MODEL_GRID, .ready = FALSE, .extent = extent,
    .basis = {v3x, v3y, v3z}, .primary = {0, 2}
  };
}
