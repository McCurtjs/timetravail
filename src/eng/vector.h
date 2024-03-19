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

#define CV const volatile

typedef struct Array {
  uint  CV element_size;
  uint  CV size;
  uint  CV size_bytes;
  uint  CV capacity;
  void* CV data;
} Array;

// Idea that's less annoying and quite niceish:

// publicly available in the header
typedef struct DArray {
  uint CV element_size;
  uint CV size;
  uint CV size_bytes;
  uint CV capacity;
} DArray;

// opaque internal version:
typedef union DArray_Internal {
  DArray properties;
  struct {
    uint element_size;
    uint size;
    uint size_bytes;
    uint capacity;
    void* data;
  };
} DArray_Internal;

// Management functions would return &internal.properties, which would then
//   be used by the user to access the structure.
// Might still be annoying though, as internal funcitons would now ALL need
//   to cast between the public and opaque types, but could work.
// Probably less useful for dynamic array (data pointer needs to be public for
//   buffer passing reasons with GL - maybe that would be fine to do as a
//   "vector_buffer" function). Would be a more useful pattern for structures
//   with more finnicky internals (like the model classes, or hashmap)






typedef struct Range {
  Array* ref;
  uint  start;
  uint  end;
} Range;

Array array_init(uint elemenet_size);
void  array_reserve(Array* array, uint capacity);
void  array_delete(Array* array);

uint  array_push_back(Array* array, const void* in_element);
uint  array_pop_back(Array* array);
void* array_get(Array* array, uint index);
void  array_read(Array* array, uint index, void* out_element);
void* array_get_front(Array* array);
void  array_read_front(Array* array, void* out_element);
void* array_get_back(Array* array);
void  array_read_back(Array* array, void* out_element);
void  array_clear(Array* array);

void  vector_init(Vector* vector, uint element_size);
void  vector_init_reserve(Vector* vector, uint element_size, uint capacity);

void  vector_reserve(Vector* vector, uint capacity);
void* vector_get(Vector* vector, uint index);
void  vector_read(Vector* vector, uint index, void* out_element);
void* vector_get_front(Vector* vector);
void  vector_read_front(Vector* vector, void* out_element);
void* vector_get_back(Vector* vector);
void  vector_read_back(Vector* vector, void* out_element);
uint  vector_push_back(Vector* vector, const void* in_element);
uint  vector_pop_back(Vector* vector, void* out_element);

void  vector_clear(Vector* vector);
void  vector_delete(Vector* vector);

#endif
