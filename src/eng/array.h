#ifndef _DYNAMIC_ARRAY_H_
#define _DYNAMIC_ARRAY_H_

#include "types.h"

typedef struct {
  uint CV element_size;
  uint CV capacity;
  uint CV size;
  uint CV size_bytes;
}* Array;

#define array_new(TYPE) _array_new_(sizeof(TYPE))
#define array_new_reserve(TYPE, capacity) _array_new_reserve_(sizeof(TYPE), capacity)
Array _array_new_(uint elemenet_size);
Array _array_new_reserve_(uint element_size, uint capacity);
void  array_reserve(Array array, uint capacity);
void  array_truncate(Array array, uint capacity);
void  array_clear(Array array);
void  array_free(Array array);
void  array_delete(Array* array);
uint  array_insert(Array array, uint position, const void* in_element);
uint  array_push_back(Array array, const void* in_element);
uint  array_pop_back(Array array);
void* array_get(Array array, uint index);
void  array_read(const Array array, uint index, void* out_element);
void* array_get_front(Array array);
void  array_read_front(const Array array, void* out_element);
void* array_get_back(Array array);
void  array_read_back(const Array array, void* out_element);

// usage:
// MyType* array_foreach(iterator, array) { use(iterator); }
#define array_foreach(VAR, ARRAY)                                             \
  VAR = NULL;                                                                 \
  for (uint MACRO_CONCAT(_array_iter_, __LINE__) = 0;                         \
    MACRO_CONCAT(_array_iter_, __LINE__) < ARRAY->size                        \
      ? (VAR = array_get((Array)ARRAY, MACRO_CONCAT(_array_iter_,__LINE__))),1\
      : 0;                                                                    \
    ++MACRO_CONCAT(_array_iter_, __LINE__)                                    \
  )                                                                           //

// usage:
// MyType* array_foreach_index(iter, i, array) { something_else[i] = iter; }
#define array_foreach_index(VAR, INDEX, ARRAY)                                \
  VAR = NULL;                                                                 \
  for (uint INDEX = 0;                                                        \
    INDEX < ARRAY->size                                                       \
      ? (VAR = array_get((Array)ARRAY, INDEX)), 1                             \
      : 0;                                                                    \
    ++INDEX)                                                                  //

#endif

// specialized container/template type
#ifdef con_type

#ifdef con_prefix
# define _full_prefix MACRO_CONCAT(arr_, con_prefix)
#else
# define _full_prefix MACRO_CONCAT(arr_, con_type)
#endif

#define _arr_type MACRO_CONCAT(Array_, con_type)

#define _prefix(_fn) MACRO_CONCAT(_full_prefix, _fn)

// Annoyingly have to redefine the struct to match - if we just typedef the
//    pointer type, it'll happily accept either as equivalent, but the whole
//    point is to prompt type errors.
typedef struct {
  uint CV element_size;
  uint CV capacity;
  uint CV size;
  uint CV size_bytes;
}* _arr_type;

static inline _arr_type _prefix(_new)
(void) {
  return (_arr_type) { (_arr_type)array_new(con_type) };
}

static inline _arr_type _prefix(_new_reserve)
(uint capacity) {
  return (_arr_type)array_new_reserve(con_type, capacity);
}

static inline void _prefix(_reserve)
(_arr_type arr, uint capacity) {
  array_reserve((Array)arr, capacity);
}

static inline void _prefix(_truncate)
(_arr_type arr, uint capacity) {
  array_truncate((Array)arr, capacity);
}

static inline void _prefix(_clear)
(_arr_type arr) {
  array_clear((Array)arr);
}

static inline void _prefix(_free)
(_arr_type arr) {
  array_free((Array)arr);
}

static inline void _prefix(_delete)
(_arr_type* parr) {
  array_delete((Array*)parr);
}

static inline uint _prefix(_insert)
(_arr_type arr, uint position, con_type element) {
  return array_insert((Array)arr, position, &element);
}

static inline uint _prefix(_push_back)
(_arr_type arr, con_type element) {
  return array_push_back((Array)arr, &element);
}

static inline uint _prefix(_pop_back)
(_arr_type arr) {
  return array_pop_back((Array)arr);
}

static inline con_type _prefix(_get)
(_arr_type arr, uint index) {
  return *(con_type*)array_get((Array)arr, index);
}

static inline con_type* _prefix(_get_ref)
(_arr_type arr, uint index) {
  return (con_type*)array_get((Array)arr, index);
}

static inline void _prefix(_read)
(_arr_type arr, uint index, con_type* out_element) {
  array_read((Array)arr, index, out_element);
}

static inline con_type _prefix(_get_front)
(_arr_type arr) {
  return *(con_type*)array_get_front((Array)arr);
}

static inline con_type* _prefix(_get_front_ref)
(_arr_type arr) {
  return (con_type*)array_get_front((Array)arr);
}

static inline void _prefix(_read_front)
(_arr_type arr, con_type* out_element) {
  array_read_front((Array)arr, out_element);
}

static inline con_type _prefix(_get_back)
(_arr_type arr) {
  return *(con_type*)array_get_back((Array)arr);
}

static inline con_type* _prefix(_get_back_ref)
(_arr_type arr) {
  return (con_type*)array_get_back((Array)arr);
}

static inline void _prefix(_read_back)
(_arr_type arr, con_type* out_element) {
  array_read_back((Array)arr, out_element);
}

#undef _arr_type
#undef _full_prefix
#undef _prefix

#endif
