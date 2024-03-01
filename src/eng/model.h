#ifndef _MODEL_H_
#define _MODEL_H_

#include "types.h"
#include "mat.h"

typedef enum ModelType {
  MODEL_NONE = 0,
  MODEL_GRID,
  MODEL_CUBE,
  MODEL_CUBE_COLOR,
  MODEL_TYPES_COUNT
} ModelType;

// A grid defined by the basis axes provided, along the index primary axes
// if extent is 0, render only a unit axis gizmo
// use a negative extent to scale the gizmo by the absolute value
typedef struct Model_Grid {
  uint type;
  uint ready;
  int  extent;
  vec3 basis[3];
  byte primary[2];
  uint vao;
  uint points_count;
  uint buffers[2];
} Model_Grid;

typedef struct Model_Cube {
  uint type;
  uint ready;
} Model_Cube;

typedef struct Model_CubeColor {
  uint type;
  uint ready;
} Model_CubeColor;

typedef union Model {
  struct {
    uint type;
    uint ready;
  };
  Model_Grid grid;
  Model_Cube cube;
  Model_CubeColor cube_color;
} Model;

int  model_build(Model* model);
void model_render(Model* model);

void model_setup_default_grid(Model* model, int extent);

//void dbdraw_offset(vec3 offset);
//void dbdraw_vector(vec3 v);
//void dbdraw_line(vec3 a, vec3 b);

#endif
