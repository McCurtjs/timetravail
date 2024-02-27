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

// A grid along the X/Z plane with color gizmo for primary axes
// if extent is 0, render only a unit axis gizmo
// use a negative extent to scale the gizmo by the absolute value
typedef struct Model_Grid {
  uint type;
  uint ready;
  int  extent;
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

#endif
