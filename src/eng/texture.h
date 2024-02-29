#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include "image.h"

typedef struct Texture {
  jshandle handle;
} Texture;

int texture_build_from_image(Texture* texture, const Image* image);

#endif
