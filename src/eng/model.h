#ifndef _MODEL_H_
#define _MODEL_H_

#include "types.h"
#include "mat.h"
#include "vector.h"

typedef enum ModelType {
  MODEL_NONE = 0,
  MODEL_GRID,
  MODEL_CUBE,
  MODEL_CUBE_COLOR,
  MODEL_SPRITES,
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

typedef struct Model_Sprites {
  uint type;
  uint ready;
  vec2i grid;
  uint vao;
  uint buffer;
  Vector verts;
} Model_Sprites;

typedef union Model {
  struct {
    uint type;
    uint ready;
  };
  Model_Grid grid;
  Model_Cube cube;
  Model_CubeColor cube_color;
  Model_Sprites sprites;
} Model;

int  model_build(Model* model);
void model_render(const Model* model);

void model_grid_set_default(Model* model, int extent);
void model_sprites_draw(const Model_Sprites* spr, vec2 pos, float scale, uint frame);

#endif
