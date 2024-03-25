#ifndef _DYNAMIC_ARRAY_H_
#define _DYNAMIC_ARRAY_H_

#include "types.h"

typedef struct {
  uint CV element_size;
  uint CV capacity;
  uint CV size;
  uint CV size_bytes;
}* Array;



//typedef struct Range {
//  Array* ref;
//  uint  start;
//  uint  end;
//} Range;


#define array_new(TYPE) _array_new_(sizeof(TYPE))
#define array_new_reserve(TYPE, capacity) _array_new_reserve_(sizeof(TYPE), capacity)
Array _array_new_(uint elemenet_size);
Array _array_new_reserve_(uint element_size, uint capacity);
void  array_reserve(Array array, uint capacity);
void  array_truncate(Array array, uint capacity);
void  array_clear(Array array);
void  array_free(Array array);
void  array_delete(Array* array);
uint  array_push_back(Array array, const void* in_element);
uint  array_pop_back(Array array);
void* array_get(Array array, uint index);
void  array_read(const Array array, uint index, void* out_element);
void* array_get_front(Array array);
void  array_read_front(const Array array, void* out_element);
void* array_get_back(Array array);
void  array_read_back(const Array array, void* out_element);

#define array_foreach(VAR, ARRAY) \
  VAR = NULL; \
  for (uint MACRO_CONCAT(array_iter_, __LINE__) = 0; \
       (VAR = array_get(ARRAY, MACRO_CONCAT(array_iter_, __LINE__))), \
       MACRO_CONCAT(array_iter_, __LINE__) < ARRAY->size; \
       ++MACRO_CONCAT(array_iter_, __LINE__))

#define array_foreach_index(VAR, INDEX, ARRAY) \
  VAR = NULL; \
  for (uint INDEX = 0; \
       (VAR = array_get(ARRAY, INDEX)), INDEX < ARRAY->size; \
       ++INDEX)

#endif
