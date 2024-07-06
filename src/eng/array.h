#ifndef _DYNAMIC_ARRAY_H_
#define _DYNAMIC_ARRAY_H_

#include "types.h"

typedef struct {
  uint  const element_size;
  uint  const capacity;
  uint  const size;
  uint  const size_bytes;
  void* const arr;
}* Array;

// todo: change all array_ prefixes to arr_
// todo: add array_emplace_back or array_push_back_ref for pointer ops?
//    (generic versions use arr_typ_push_back(arr, non_ptr_obj); )

#define array_new(TYPE) _array_new_(sizeof(TYPE))
#define array_new_reserve(TYPE, capacity) _array_new_reserve_(sizeof(TYPE), capacity)
Array _array_new_(uint elemenet_size);
Array _array_new_reserve_(uint element_size, uint capacity);
void  array_reserve(Array array, uint capacity);
void  array_truncate(Array array, uint capacity);
void  array_clear(Array array);
void  array_free(Array array);
void  array_delete(Array* array);
uint  array_write(Array array, uint position, const void* in_element);
uint  array_write_back(Array array, const void* in_element);
uint  array_pop_back(Array array);
void* array_get_ref(Array array, uint index);
void  array_read(const Array array, uint index, void* out_element);
void* array_get_front_ref(Array array);
void  array_read_front(const Array array, void* out_element);
void* array_get_back_ref(Array array);
void  array_read_back(const Array array, void* out_element);

// \brief A macro shorthand to write foreach loops with any dynamic Array or
//    Array-based sub-types.
//
// \brief usage example:
// \brief MyType* array_foreach(iterator, array) { use(iterator); }
#define array_foreach(VAR, ARRAY)                                             \
  array_foreach_index(VAR, MACRO_CONCAT(_array_iter, __LINE__), ARRAY)        //

// \brief A macro shorthand to write foreach loops with any dynamic Array or
//    Array-based sub-types. Includes a tracking index value as well.
//
// \brief usage example:
// \brief MyType* array_foreach_index(iter, i, array) { other[i] = *iter; }
#define array_foreach_index(VAR, INDEX, ARRAY)                                \
  VAR = (ARRAY)->arr;                                                         \
  assert(sizeof(*VAR) == ARRAY->element_size);                                \
  for (uint INDEX = 0; INDEX < ARRAY->size; ++INDEX,                          \
    VAR = (void*)((byte*)ARRAY->arr + INDEX * sizeof(*VAR))                   \
  )                                                                           //

// note: using VAR + i*s instead of just ++VAR in order to ensure the loop will
//    continue to work in cases where a resize is performed during iteration.

#endif

// specialized container/template type
#ifdef con_type

// Specialized container functions are declared as arr_<prefix>_<fn>
//    ex: - if con_prefix is 'str', you'll get a function arr_str_push_back
//        - if con_prefix is not set, you'll get arr_String_push_back
#ifdef con_prefix
# define _full_prefix MACRO_CONCAT(arr_, con_prefix)
#else
# define _full_prefix MACRO_CONCAT(arr_, con_type)
#endif

// The type of the specialized array class will be Array_<type>.
//    for example: Array_String, Array_Entity, etc.
#define _arr_type MACRO_CONCAT(Array_, con_type)

#define _prefix(_fn) MACRO_CONCAT(_full_prefix, _fn)

// Annoyingly have to redefine the struct to match - if we just typedef the
//    pointer type, it'll happily accept either as equivalent, but the whole
//    point is to prompt type errors.
typedef struct {
  uint const element_size;
  uint const capacity;
  uint const size;
  uint const size_bytes;
  union {
    con_type* const arr;
    con_type* const buf;
    con_type* const first;
  };
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
  return array_write((Array)arr, position, &element);
}

static inline uint _prefix(_write)
(_arr_type arr, uint position, const con_type* element) {
  return array_write((Array)arr, position, element);
}

static inline uint _prefix(_push_back)
(_arr_type arr, con_type element) {
  return array_write_back((Array)arr, &element);
}

// Will this successfully associate this comment with the function?
// todo: add documentation for each of these
static inline uint _prefix(_write_back)
(_arr_type arr, const con_type* element) {
  return array_write_back((Array)arr, element);
}

static inline uint _prefix(_pop_back)
(_arr_type arr) {
  return array_pop_back((Array)arr);
}

static inline con_type _prefix(_get)
(_arr_type arr, uint index) {
  return *(con_type*)array_get_ref((Array)arr, index);
}

static inline con_type* _prefix(_get_ref)
(_arr_type arr, uint index) {
  return (con_type*)array_get_ref((Array)arr, index);
}

static inline void _prefix(_read)
(_arr_type arr, uint index, con_type* out_element) {
  array_read((Array)arr, index, out_element);
}

static inline con_type _prefix(_get_front)
(_arr_type arr) {
  return *(con_type*)array_get_front_ref((Array)arr);
}

static inline con_type* _prefix(_get_front_ref)
(_arr_type arr) {
  return (con_type*)array_get_front_ref((Array)arr);
}

static inline void _prefix(_read_front)
(_arr_type arr, con_type* out_element) {
  array_read_front((Array)arr, out_element);
}

static inline con_type _prefix(_get_back)
(_arr_type arr) {
  return *(con_type*)array_get_back_ref((Array)arr);
}

static inline con_type* _prefix(_get_back_ref)
(_arr_type arr) {
  return (con_type*)array_get_back_ref((Array)arr);
}

static inline void _prefix(_read_back)
(_arr_type arr, con_type* out_element) {
  array_read_back((Array)arr, out_element);
}

#undef _arr_type
#undef _full_prefix
#undef _prefix

#endif
