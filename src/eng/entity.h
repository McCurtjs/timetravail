#ifndef _ENTITY_H_
#define _ENTITY_H_

#include "mat.h"
#include "model.h"
#include "texture.h"
#include "shader.h"
#include "vector.h"

typedef struct PlayerFrameData {
  vec2 pos;
  vec2 vel;
  int airborne;
  int has_double;
} PlayerFrameData;

typedef struct TransformIndex {
  uint shader;
  uint model;
  uint texture;
  uint transform;
} TransformIndex;

typedef struct Game Game;
typedef struct Entity Entity;
typedef void (*UpdateFn)(Entity* e, Game* game, float dt);
typedef void (*RenderFn)(Entity* e, Game* game);
//typedef void (PhysicsFn)(struct Entity* e);
//typedef void (OnCollideFn)(struct Entity* self, struct Entity* other); // ?

typedef struct Entity {
  union {
    PlayerFrameData fd;
    vec3 pos;
  };

  union {
    quat rotation;
    float angle;
  };
  union {
    TransformIndex transform_ref;
    struct {
      mat4 transform;
      const ShaderProgram* shader;
      const Model* model;
      const Texture* texture;
    };
  };

  bool hidden;

  Vector replay;
  Vector replay_temp;
  bool playback;

  uint index; // used only by forward replays for caching
  uint frame; // used only by forward replays for caching

  RenderFn render;
  UpdateFn behavior;

} Entity;

#endif
