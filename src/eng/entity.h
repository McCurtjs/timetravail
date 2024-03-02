#ifndef _ENTITY_H_
#define _ENTITY_H_

#include "mat.h"
#include "model.h"
#include "texture.h"
#include "shader.h"

typedef struct TransformIndex {
  uint shader;
  uint model;
  uint texture;
  uint transform;
} TransformIndex;

typedef struct Game Game;
typedef struct Entity Entity;
typedef void (*UpdateFn)(Entity* e, const Game* game, float dt);
typedef void (*RenderFn)(Entity* e, const Game* game);
//typedef void (PhysicsFn)(struct Entity* e);
//typedef void (OnCollideFn)(struct Entity* self, struct Entity* other); // ?

typedef struct Entity {
  vec3 pos;
  union {
    quat rotation;
    float angle;    // for 2d, angle around z axis
  };
  union {
    TransformIndex transform_ref;
    struct {
      mat4 transform; // what even is a transform in this context
      const ShaderProgram* shader;
      const Model* model;
      const Texture* texture;
    };
  };

  RenderFn render;
  UpdateFn behavior;

} Entity;

#endif
