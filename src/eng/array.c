#include "array.h"

#include <stdlib.h>
#include <memory.h>

#include "wasm.h"
#include "types.h"

static const uint capacity_min = 16;

// internal opaque structure:
typedef struct Array_Internal {
  // public (read only)
  uint element_size;
  uint capacity;
  uint size;
  uint size_bytes;

  // private
  byte* data;
} Array_Internal;

#define DARRAY_INTERNAL Array_Internal* a = (Array_Internal*)(a_in)
#define DARRAY_INTERNAL_CONST const Array_Internal* a = (const Array_Internal*)(a_in)

Array _array_new_(uint element_size) {
  Array_Internal* ret = malloc(sizeof(Array_Internal));
  assert(ret); // TODO: better handling of critical memory situations
  *ret = (Array_Internal) {
    .element_size = element_size,
    .capacity = 0,
    .size = 0,
    .size_bytes = 0,
    .data = NULL,
  };
  return (Array)ret;
}

Array _array_new_reserve_(uint element_size, uint capacity) {
  Array_Internal* ret = malloc(sizeof(Array_Internal));
  assert(ret); // TODO: better handling of critical memory situations
  *ret = (Array_Internal) {
    .element_size = element_size,
    .capacity = capacity,
    .size = 0,
    .size_bytes = 0,
    .data = malloc(element_size * capacity),
  };
  return (Array)ret;
}

void array_reserve(Array a_in, uint capacity) {
  DARRAY_INTERNAL;
  if (!a || a->capacity >= capacity) return;
  void* new_data = realloc(a->data, a->element_size * capacity);
  assert(new_data); // TODO: better handling of critical memory situations
  a->data = new_data;
  a->capacity = capacity;
}

void array_truncate(Array a_in, uint max_size) {
  DARRAY_INTERNAL;
  if (!a || a->capacity < max_size) return;
  void* new_data = realloc(a->data, a->element_size * max_size);
  if (!new_data) return;
  a->data = new_data;
  a->capacity = max_size;
  if (a->size > max_size) {
    a->size = max_size;
    a->size_bytes = max_size * a->element_size;
  }
}

void array_clear(Array a_in) {
  DARRAY_INTERNAL;
  a->size = 0;
  a->size_bytes = 0;
}

void array_free(Array a_in) {
  DARRAY_INTERNAL;
  if (!a || !a->data) return;
  array_clear(a_in);
  free(a->data);
  a->capacity = 0;
  a->data = NULL;
}

void array_delete(Array* a_in) {
  if (!a_in || !*a_in) return;
  array_free(*a_in);
  free(*a_in);
  *a_in = NULL;
}

uint array_push_back(Array a_in, const void* element) {
  DARRAY_INTERNAL;
  if (a->size >= a->capacity) {
    array_reserve(a_in, MAX(capacity_min, a->capacity + a->capacity / 2));
  }
  memcpy(a->data + a->size * a->element_size, element, a->element_size);
  a->size_bytes += a->element_size;
  return ++a->size;
}

uint array_pop_back(Array a_in) {
  DARRAY_INTERNAL;
  if (!a || a->size == 0) return 0;
  a->size_bytes -= a->element_size;
  return --a->size;
}

void* array_get(Array a_in, uint index) {
  DARRAY_INTERNAL;
  if (!a || index >= a->size) return NULL;
  return a->data + index * a->element_size;
}

void array_read(const Array a_in, uint index, void* element) {
  DARRAY_INTERNAL_CONST;
  if (!a || index >= a->size) return;
  memcpy(element, a->data + index * a->element_size, a->element_size);
}

void* array_get_front(Array a_in) {
  DARRAY_INTERNAL;
  if (!a || a->size == 0) return NULL;
  return a->data;
}

void array_read_front(const Array a_in, void* element) {
  DARRAY_INTERNAL_CONST;
  if (!a || a->size == 0) return;
  memcpy(element, a->data, a->element_size);
}

void* array_get_back(Array a_in) {
  DARRAY_INTERNAL;
  if (!a || a->size == 0) return NULL;
  return a->data + (a->size - 1) * a->element_size;
}

void array_read_back(const Array a_in, void* element) {
  DARRAY_INTERNAL_CONST;
  if (!a || a->size == 0) return;
  memcpy(element, a->data + (a->size - 1) * a->element_size, a->element_size);
}
