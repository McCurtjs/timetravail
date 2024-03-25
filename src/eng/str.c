#include "str.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "utility.h"

#undef SRCV
#define SRCV
typedef struct {
  // public (read-only)
  union {
    StringRange range;
    _STR_RANGE_DEF();
  };

  // private
  char head;
} String_Internal;

const static char str_chr_literal_empty = '\0';
static struct _Str_Base str_constants[] = {
  { .begin = &str_chr_literal_empty, .size = 0 },
  { .begin = "true", .size = 4 },
  { .begin = "false", .size = 5 },
};
const static String str_constants_end = &str_constants[0] + sizeof(str_constants);

const String str_empty = &str_constants[0];
const String str_true  = &str_constants[1];
const String str_false = &str_constants[2];

static String_Internal* str_new_internal(size_t length) {
  if (length == 0) return NULL; // prompt callers to return empty string
  String_Internal* ret = malloc(length + sizeof(String_Internal) + 1);
  assert(ret);
  ret->begin = &ret->head;
  ret->size = length;
  return ret;
}

inline static String str_terminate(String_Internal* str) {
  *(str->begin + str->length) = '\0';
  return (String)str;
}

inline static bool str_is_literal(String str) {
  return (str >= &str_constants[0] && str < str_constants_end);
}

String str_new(const char* c_str) {
  if (!c_str) return str_empty;
  size_t length = strlen(c_str);
  String_Internal* ret = str_new_internal(length);
  if (!ret) return str_empty;
  memcpy(&ret->head, c_str, length);
  return str_terminate(ret);
}

String str_new_s(const char* c_str, size_t length) {
  if (!c_str) return str_empty;
  String_Internal* ret = str_new_internal(length);
  if (!ret) return str_empty;
  memcpy(&ret->head, c_str, length);
  return str_terminate(ret);
}

String str_copy(StringRange str) {
  String_Internal* ret = str_new_internal(str.size);
  if (!ret) return str_empty;
  memcpy(ret->begin, str.begin, str.size);
  return str_terminate(ret);
}

void str_delete(String* str) {
  if (!str || !*str) return;
  if (!str_is_literal(*str)) free(*str);
  *str = NULL;
}

String str_from_bool(bool b) {
  return b == FALSE ? str_false : str_true;
}

String str_from_int(int i) {
  return str_new(itos(i));
}

String str_from_float(float f) {
  return str_new(ftos(f));
}

StringRange str_range(const char* c_str) {
  return (StringRange) {
    .begin = c_str,
    .length = strlen(c_str),
  };
}

StringRange str_range_s(const char* c_str, size_t length) {
  return (StringRange) {
    .begin = c_str,
    .length = length,
  };
}

String str_concat(StringRange left, StringRange right) {
  size_t length = left.size + right.size;
  String_Internal* ret = str_new_internal(length);
  if (!ret) return str_empty;
  memcpy(ret->begin, left.begin, left.size);
  memcpy(ret->begin + left.size, right.begin, right.size);
  return str_terminate(ret);
}

static const StringRange* str_to_range(const void* element, bool is_ptr) {
  return is_ptr ? *(StringRange**)element : element;
}

String str_join(StringRange del, const Array strings) {
  const size_t range_count = strings->size;
  if (range_count == 0) return str_empty;

  bool is_ptr = strings->element_size == sizeof(String);
  size_t length = 0;
  const StringRange* range;

  for (uint i = 0; i < range_count; ++i) {
    range = str_to_range(array_get(strings, i), is_ptr);
    length += range->size;
  }

  length += del.size * (range_count - 1);

  String_Internal* ret = str_new_internal(length);
  if (!ret) return str_empty;

  char* dst = ret->begin;

  for (uint i = 0; i < range_count; ++i) {
    range = str_to_range(array_get(strings, i), is_ptr);
    memcpy(dst, range->begin, range->size);

    dst += range->size;

    if (i != range_count - 1) {
      memcpy(dst, del.begin, del.size);
      dst += del.size;
    }
  }

  str_terminate(ret);
  return (String)ret;
}

Array str_split(StringRange str, StringRange del) {
  Array ret = array_new(StringRange);

  int i = 0;
  do {
    int prev = i;
    i = (int)str_index_of(str, del, i);
    StringRange range = str_substring(str, prev, i);
    array_push_back(ret, &range);
    i += (int)del.size;
  } while (i < (int)str.size);

  return ret;
}

StringRange _str_substring(StringRange str, int start, int end) {
  if (start >= (int)str.size) return str_empty->range;
  if (start < 0) start = (int)str.size + start;
  if (start < 0) start = 0;
  if (end > (int)str.size) end = (int)str.size;
  if (end < 1) end = (int)str.size + end;
  if (end <= start) return str_empty->range;
  return (StringRange) {
    .begin = str.begin + start,
    .size = end - start,
  };
}

StringRange str_trim(StringRange str) {
  StringRange ret = str_trim_start(str);
  return str_trim_end(ret);
}

StringRange str_trim_start(StringRange str) {
  int start, end = (int)str.size;
  for (start = 0; start < (int)str.size; ++start) {
    if (!isspace(str.begin[start])) break;
  }
  if (start == end) return str_empty->range;
  return (StringRange) {
    .begin = str.begin + start,
    .size = end - start,
  };
}

StringRange str_trim_end(StringRange str) {
  int end = (int)str.size;
  while (end > 0) {
    if (isspace(str.begin[end - 1])) --end;
    else break;
  }
  if (end <= 0) return str_empty->range;
  return (StringRange) {
    .begin = str.begin,
    .size = end,
  };
}

bool str_eq(StringRange lhs, StringRange rhs) {
  if (lhs.size != rhs.size) return FALSE;
  return memcmp(lhs.begin, rhs.begin, lhs.size) == 0;
}

bool str_starts_with(StringRange str, StringRange starts) {
  if (starts.size > str.size) return FALSE;
  return memcmp(str.begin, starts.begin, starts.size) == 0;
}

bool str_ends_with(StringRange str, StringRange ends) {
  if (ends.size > str.size) return FALSE;
  return memcmp(str.begin + str.size - ends.size, ends.begin, ends.size) == 0;
}

size_t str_find(StringRange str, StringRange to_find) {
  return str_index_of(str, to_find, 0);
}

// Gets the first index of to_find in str. If not found, returns str.size
size_t str_index_of(StringRange str, StringRange to_find, size_t from_pos) {
  if (str.size < to_find.size) return str.size;
  if (to_find.size == 0) return from_pos;
  size_t j;
  for (size_t i = from_pos; i <= str.size - to_find.size; ++i) {
    j = 0;
    while (j < to_find.size) {
      if (str.begin[i + j] != to_find.begin[j]) break;
      if (++j == to_find.size) return i;
    }
  }
  return str.size;
}
