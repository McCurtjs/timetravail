#include "model.h"

#include "GL/gl.h"

// inlined static data declarations for model primitives
#include "data/inline_primitives.h"

static GLuint cube_pos_buffer = 0;
static void prim_bind_cube() {
  if (!cube_pos_buffer) {
    GLsizeiptr size = 14 * sizeof(*cube_pos);
    glGenBuffers(1, &cube_pos_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, cube_pos_buffer);
    glBufferData(GL_ARRAY_BUFFER, size, cube_pos, GL_STATIC_DRAW);
    glVertexAttribPointer(0, sizeof(*cube_pos) / 4, GL_FLOAT, GL_FALSE, 0, 0);
  }
}

static GLuint cube_color_buffer = 0;
static void prim_bind_cube_color() {
  if (!cube_color_buffer) {
    GLsizeiptr size = 14 * sizeof(*cube_color);
    glGenBuffers(1, &cube_color_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, cube_color_buffer);
    glBufferData(GL_ARRAY_BUFFER, size, cube_color, GL_STATIC_DRAW);
    glVertexAttribPointer(1, sizeof(*cube_color) / 4, GL_FLOAT, GL_FALSE, 0, 0);
  }
}



typedef int (*model_build_pfn)(void* model);



static int model_build_cube(Model_Cube* cube) {
  prim_bind_cube();
  cube->ready = TRUE;
  return 1;
}

void model_render_cube(Model_Cube* cube) {
  glBindBuffer(GL_ARRAY_BUFFER, cube_pos_buffer);
  glEnableVertexAttribArray(0);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 14);
  glDisableVertexAttribArray(0);
}



static int model_build_cube_color(Model_CubeColor* cube) {
  prim_bind_cube();
  prim_bind_cube_color();
  cube->ready = TRUE;
  return 1;
}

void model_render_cube_color(Model_CubeColor* cube) {
  glBindBuffer(GL_ARRAY_BUFFER, cube_pos_buffer);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, cube_color_buffer);
  glEnableVertexAttribArray(1);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 14);
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
}

static model_build_pfn model_build_fns[MODEL_TYPES_COUNT] = {
  NULL,
  (model_build_pfn)model_build_cube,
  (model_build_pfn)model_build_cube_color
};

int model_build(Model* model) {
  if (!model || !model->type) return 0;

  return model_build_fns[model->type](model);
}
