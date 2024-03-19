#ifndef _ENTITY_H_
#define _ENTITY_H_

#include "mat.h"
#include "model.h"
#include "texture.h"
#include "shader.h"
#include "array.h"

#include "../animations.h"
typedef struct Line Line;

typedef enum EntityType {
  ENTITY_OTHER,
  ENTITY_PLAYER,
  ENTITY_PLATFORM,
} EntityType;

typedef struct PlayerFrameData {
  vec2 pos;
  vec2 vel;
  int airborne;
  int has_double;
  const Line* standing;
  uint start_frame;
  uint animation;
  bool warp_triggered;
  bool facing;
  bool in_combat;
  uint hitstun;
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
typedef void (*DeleteFn)(Entity* e);
//typedef void (PhysicsFn)(struct Entity* e);
//typedef void (OnCollideFn)(struct Entity* self, struct Entity* other); // ?

typedef struct Movement {
  Line* line;
  bool move_once;
  vec2 target;
  float delay;
  float duration;
  vec2 origin, v_plat, v_rail;
} Movement;

typedef struct Entity {
  uint type;

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

  // GameObject* obj; // pointer to custom data? (void*?)

  union {
    Movement movement_params;
    struct {
      AnimationData anim_data;
      Array replay;
      Array replay_temp;
      bool playback;
    };
  };

  RenderFn render;
  UpdateFn behavior;
  DeleteFn delete;

} Entity;

#endif
