#include "str.h"

#include <stdlib.h>
#include <string.h>

#undef SRCV
#define SRCV
typedef struct {
  // public (read-only)
  union {
    StringRange range;
    STR_RANGE();
  };
  bool is_static;

  // private
  char head;
} String_Internal;

const static char empty_string_literal = '\0';
static struct Str_Base empty_string = {
  .begin = &empty_string_literal, // these would ideally actually point to &head
  .end = &empty_string_literal,
  .size = 0,
  .is_static = TRUE
};

static String_Internal* str_new_internal(size_t length) {
  if (length == 0) return NULL; // prompt callers to return empty string
  String_Internal* ret = malloc(length + sizeof(String_Internal) + 1);
  assert(ret);
  ret->begin = &ret->head;
  ret->end = &ret->head + length;
  ret->size = length;
  ret->is_static = FALSE;
  return ret;
}

String str_new(const char* c_str) {
  if (!c_str) return &empty_string;
  size_t length = strlen(c_str);
  String_Internal* ret = str_new_internal(length);
  if (!ret) return &empty_string;
  memcpy(&ret->head, c_str, length);
  *ret->end = '\0';
  return (String)ret;
}

String str_new_s(const char* c_str, size_t length) {
  if (!c_str) return &empty_string;
  String_Internal* ret = str_new_internal(length);
  if (!ret) return &empty_string;
  memcpy(&ret->head, c_str, length);
  *ret->end = '\0';
  return (String)ret;
}

String str_copy(StringRange str) {
  String_Internal* ret = str_new_internal(str.size);
  if (!ret) return &empty_string;
  memcpy(ret->begin, str.begin, str.size);
  *ret->end = '\0';
  return (String)ret;
}

void str_delete(String* str) {
  if (!str || !*str) return;
  if (*str != &empty_string) free(*str);
  *str = NULL;
}

String str_concat(StringRange left, StringRange right) {
  size_t length = left.size + right.size;
  String_Internal* ret = str_new_internal(length);
  if (!ret) return &empty_string;
  memcpy(ret->begin, left.begin, left.size);
  memcpy(ret->begin + left.size, right.begin, right.size);
  *ret->end = '\0';
  return (String)ret;
}

static const StringRange* str_to_range(const void* element, bool is_ptr) {
  return is_ptr ? *(StringRange**)element : element;
}

String str_join(StringRange del, const Array ranges) {
  const size_t range_count = ranges->size;
  if (range_count == 0) return &empty_string;

  bool is_ptr = ranges->element_size == sizeof(String);
  size_t length = 0;
  const StringRange* range;

  for (uint i = 0; i < range_count; ++i) {
    range = str_to_range(array_get(ranges, i), is_ptr);
    length += range->size;
  }

  length += del.size * (range_count - 1);

  String_Internal* ret = str_new_internal(length);
  if (!ret) return &empty_string;

  char* dst = ret->begin;

  for (uint i = 0; i < range_count; ++i) {
    range = str_to_range(array_get(ranges, i), is_ptr);
    memcpy(dst, range->begin, range->size);

    dst += range->size;

    if (i != range_count - 1) {
      memcpy(dst, del.begin, del.size);
      dst += del.size;
    }
  }

  *ret->end = '\0';
  return (String)ret;
}
