#ifndef _STORAGE_VECTOR_H_
#define _STORAGE_VECTOR_H_

#include "types.h"

typedef struct Vector {
  void* data;
  uint size;
  uint bytes;
  uint capacity;
  uint element_size;
} Vector;

void  vector_init(Vector* vector, uint element_size);
void  vector_init_reserve(Vector* vector, uint element_size, uint capacity);

void  vector_reserve(Vector* vector, uint capacity);
void* vector_get(Vector* vector, uint index);
void* vector_back(Vector* vector);
uint  vector_push_back(Vector* vector, const void* element);
uint  vector_pop_back(Vector* vector, void* ret);

void  vector_delete(Vector* vector);

#endif
