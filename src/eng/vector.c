#include "vector.h"

#include <stdlib.h>
#include <memory.h>

#include "types.h"

static const uint default_capacity = 16;
static const uint capacity_growth = 16;

void vector_init(Vector* v, uint element_size) {
  vector_init_reserve(v, element_size, default_capacity);
}

void vector_init_reserve(Vector* v, uint element_size, uint capacity) {
  if (!v) return;
  if (v->data) free(v->data);
  *v = (Vector) {
    .data = malloc(capacity * element_size),
    .size = 0,
    .bytes = 0,
    .capacity = capacity,
    .element_size = element_size
  };
}

void vector_reserve(Vector* v, uint capacity) {
  if (!v) return;
  void* old_data = v->data;
  uint old_capacity = v->capacity;
  if (capacity <= old_capacity) return;
  v->capacity = capacity;
  v->data = malloc(capacity * v->element_size);
  v->bytes = v->size * v->element_size;
  memcpy(v->data, old_data, v->size * v->element_size);
}

uint vector_push_back(Vector* v, const void* element) {
  if (v->size >= v->capacity) {
    vector_reserve(v, v->capacity + capacity_growth);
  }
  memcpy(v->data + v->size * v->bytes, element, v->element_size);
  v->bytes += v->element_size;
  return v->size++;
}

void* vector_get(Vector* v, uint index) {
  if (!v || index >= v->size) return NULL;
  return v->data + index * v->element_size;
}

void* vector_back(Vector* v) {
  if (!v || v->size == 0) return NULL;
  return v->data + v->size * v->element_size;
}

uint vector_pop_back(Vector* v, void* ret) {
  if (!v || v->size == 0) return 0;
  memcpy(ret, v->data + v->size * v->element_size, v->element_size);
  return --v->size;
}

void vector_delete(Vector* v) {
  if (!v || !v->data) return;
  free(v->data);
  *v = (Vector) {
    .data = NULL,
    .size = 0,
    .bytes = 0,
    .capacity = 0,
    .element_size = 0
  };
}
