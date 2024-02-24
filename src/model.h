#ifndef _MODEL_H_
#define _MODEL_H_

#include "types.h"
#include "mat.h"

typedef enum ModelType {
  MODEL_NONE = 0,
  MODEL_CUBE,
  MODEL_CUBE_COLOR,
  MODEL_TYPES_COUNT
} ModelType;

typedef struct Model_Cube {
  uint type;
  uint ready;
} Model_Cube;

typedef struct Model_CubeColor {
  uint type;
  uint ready;
} Model_CubeColor;

typedef union Model {
  uint type;
  uint ready;
  Model_Cube cube;
  Model_CubeColor cube_color;
} Model;

int model_build(Model* model);

#endif
