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
    _STR_RANGE_DEF(,);
  };

  // private
  char head;
} String_Internal;

static char const str_chr_literal_empty = '\0';
static struct _Str_Base str_constants[] = {
  { .begin = &str_chr_literal_empty, .size = 0 },
  { .begin = "true", .size = 4 },
  { .begin = "false", .size = 5 },
};
static String const str_constants_end = &str_constants[0] + sizeof(str_constants);

const String str_empty = &str_constants[0];
const String str_true  = &str_constants[1];
const String str_false = &str_constants[2];

static String_Internal* str_new_internal(size_t length) {
  if (length == 0) return NULL; // prompt callers to return empty string
  // Include an extra byte for the null terminator
  String_Internal* ret = malloc(sizeof(StringRange) + length + 1);
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

String str_from_bool(bool b) {
  return b == FALSE ? str_false : str_true;
}

String str_from_int(int i) {
  return str_new(itos(i));
}

String str_from_float(float f) {
  return str_new(ftos(f));
}

void str_delete(String* str) {
  if (!str || !*str) return;
  if (!str_is_literal(*str)) free(*str);
  *str = NULL;
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

String str_concat(StringRange left, StringRange right) {
  size_t length = left.size + right.size;
  String_Internal* ret = str_new_internal(length);
  if (!ret) return str_empty;
  memcpy(ret->begin, left.begin, left.size);
  memcpy(ret->begin + left.size, right.begin, right.size);
  return str_terminate(ret);
}

// TODO: ? (not so useful when requiring ranges. Maybe make something
//          like this for string builder?)
// String _str_format(StringRange fmt, const StringRange[] argv, uint argc);
// #define _str_format_va(fmt, argv, argc, ...) _str_format(fmt, argv, argc)
// #define str_format(...) _str_fmt2(__VA_ARGS__, NULL, 0)
// TODO: verify that non-macro va args are actaully not a thing with wasi, lol

String str_prepend(StringRange str, size_t length, char c) {
  String_Internal* ret = str_new_internal(str.size + length);
  memset(ret->begin, c, length);
  memcpy(ret->begin + length, str.begin, str.size);
  return str_terminate(ret);
}

String str_append(StringRange str, size_t length, char c) {
  String_Internal* ret = str_new_internal(str.size + length);
  memcpy(ret->begin, str.begin, str.size);
  memset(ret->begin + str.size, c, length);
  return str_terminate(ret);
}

Array str_split(StringRange str, StringRange del) {
  Array ret = array_new(StringRange);

  // specialization for empty string, return a range for each char
  if (del.size == 0) {
    array_reserve(ret, (uint)str.size);
    for (size_t i = 0; i < str.size; ++i) {
      StringRange c = str_range_s(&str.begin[i], 1);
      array_push_back(ret, &c);
    }
    return ret;
  }

  int i = 0;
  do {
    int prev = i;
    i = (int)str_index_of(str, del, i);
    StringRange range = str_substring(str, prev, i);
    array_push_back(ret, &range);
    i += (int)del.size;
    if (i == (int)str.size) array_push_back(ret, &str_empty->range);
  } while (i < (int)str.size);

  return ret;
}

StringRange _str_substring(StringRange str, int start, int end) {
  if (start == end) return str_empty->range;
  if (start >= (int)str.size) return str_empty->range;
  if (start < 0) start = (int)str.size + start;
  if (start < 0) start = 0;
  if (end > (int)str.size) end = (int)str.size;
  if (end < 0) end = (int)str.size + end;
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

bool str_contains(StringRange str, StringRange check) {
  return str_find(str, check) != str.size;
}

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

size_t str_find(StringRange str, StringRange to_find) {
  return str_index_of(str, to_find, 0);
}

/*

  // A case for making a string builder that takes arguments of different types,
  // and consumes the passed String objects among them when resolving:

  String passed = str_from_int(test_passed_count);
  String total = str_from_int(test_count);
  String percent = str_from_int(ratio);
  Array arr = array_new(StringRange);
  array_push_back(arr, &R("Tests passed: %c"));
  array_push_back(arr, &passed->range);
  array_push_back(arr, &R(" out of "));
  array_push_back(arr, &total->range);
  array_push_back(arr, &R(", or "));
  array_push_back(arr, &percent->range);
  array_push_back(arr, &R("%"));
  String result = str_join(str_empty->range, arr);
  str_print_color(result->range, color);
  array_delete(&arr);
  str_delete(&result);
  str_delete(&passed);
  str_delete(&total);
  str_delete(&percent);

  // String builder helper class? How much more convenient is this?

  StringBuilder stb = stb_new();
  stb_c_str(stb, "Tests passed: %c"));
  stb_str  (stb, str_front_int(test_passed_count));
  stb_range(stb, R(" out of "));
  stb_str  (stb, str_from_int(test_count));
  stb_range(stb, R(", or "));
  stb_str  (stb, str_from_int(percent));
  stb_range(stb, R("%"));
  String result = stb_resolve(&stb);

  int color = test_count == test_passed_count ? CONCOL_Green : CONCOL_Red;
  str_print_color(result->range, color);
  str_delete(&result);

  // Arg-supporting variant?

  StringBuilder stb = stb_new();
  stb_range(stb, R("Tests passed: %c{} out of {}"));
  stb_arg_int(stb, test_passed_count);
  stb_arg_int(stb, test_count);
  stb_arg_int(stb, percent);
  String result = stb_resolve(&stb);

  int color = test_count == test_passed_count ? CONCOL_Green : CONCOL_Red;
  str_print_color(result->range, color);
  str_delete(&result);

  // Have each of the stb base functions return a new builder if passed null?

  StringBuilder stb = stb_range(NULL, R("Tests passed: %c{} out of {}, or {}%"));
  stb_arg_int(stb, test_passed_count);
  stb_arg_int(stb, test_count);
  stb_arg_int(stb, percent);
  String result = stb_resolve(&stb);

  int color = test_count == test_passed_count ? CONCOL_Green : CONCOL_Red;
  str_print_color(result->range, color);
  str_delete(&result);

*/

typedef struct {
  int type;
  String part;
} STB_Arg;

typedef enum {
  STBT_Range,
  STBT_String,
  STBT_Pad,
  STBT_PadRange
} STB_Type;

typedef struct {
  STB_Type type;
  union {
    String str;
    struct {
      size_t pad_length;
      StringRange range;
    };
    struct {
      size_t length;
      char c;
    } padding;
  };
} STB_Segment;

typedef struct {
  // public (read-only)
  size_t size;

  // private
  Array segments;
  Array args;

} StringBuilder_Internal;

#define STB_INTERNAL StringBuilder_Internal* stb = (StringBuilder_Internal*)(stb_in)
#define STB_APPENDER if (!stb_in) stb_in = stb_new(); STB_INTERNAL

StringBuilder stb_new() {
  StringBuilder_Internal* stb = malloc(sizeof(StringBuilder_Internal));
  assert(stb);
  *stb = (StringBuilder_Internal) {
    .size = 0,
    .segments = array_new(STB_Segment),
    .args = array_new(STB_Arg),
  };
  return (StringBuilder)stb;
}

StringBuilder stb_str(StringBuilder stb_in, String s) {
  STB_APPENDER;
  STB_Segment segment = {
    .type = STBT_String,
    .str = s,
  };
  array_push_back(stb->segments, &segment);
  stb->size += s->size;
  return (StringBuilder)stb;
}

StringBuilder stb_range(StringBuilder stb_in, StringRange r) {
  STB_APPENDER;
  STB_Segment segment = {
    .type = STBT_Range,
    .range = r,
  };
  array_push_back(stb->segments, &segment);
  stb->size += r.size;
  return (StringBuilder)stb;
}

StringBuilder stb_c_str(StringBuilder stb_in, const char* str) {
  return stb_range(stb_in, str_range(str));
}

StringBuilder stb_pad(StringBuilder stb_in, size_t length, char c) {
  STB_APPENDER;
  STB_Segment segment = {
    .type = STBT_Pad,
    .padding = {
      .length = length,
      .c = c,
    },
  };
  array_push_back(stb->segments, &segment);
  stb->size += segment.padding.length;
  return (StringBuilder)stb;
}

StringBuilder stb_pad_range(StringBuilder stb_in, size_t length, StringRange r) {
  STB_APPENDER;
  STB_Segment segment = {
    .type = STBT_PadRange,
    .pad_length = length,
    .range = r,
  };
  array_push_back(stb->segments, &segment);
  stb->size += segment.pad_length;
  return (StringBuilder)stb;
}

String stb_resolve(StringBuilder* stb_in_ptr) {
  StringBuilder stb_in = *stb_in_ptr;
  STB_INTERNAL;
  String_Internal* ret = str_new_internal(stb->size);
  char* c = ret->begin;

  // TODO: Make an actual set library for this kind of thing
  // Keep a list of strings to free so we don't double-free anything
  Array to_xibalba = array_new(String);

  STB_Segment* array_foreach(seg, stb->segments) {
    switch (seg->type) {
      // Copy a string - we're not deleting yet, we might use it twice...
      case STBT_String: {
        memcpy(c, seg->str->begin, seg->str->size);
        c += seg->str->size;

        String* array_foreach(search, to_xibalba) {
          if (*search == seg->str) break;
        }

        if (search && *search != seg->str) {
          array_push_back(to_xibalba, &seg->str);
        }
      } break;

      // Copy by range:
      case STBT_Range: {
        memcpy(c, seg->range.begin, seg->range.size);
        c += seg->range.size;
      } break;

      // Fill with padding:
      case STBT_Pad: {
        memset(c, seg->padding.c, seg->padding.length);
        c += seg->padding.length;
      } break;

      // Fill with non-uniform padding from a range:
      case STBT_PadRange: {
        size_t copied = 0;
        while (copied + seg->range.size < seg->pad_length) {
          memcpy(c, seg->range.begin, seg->range.size);
          c += seg->range.size;
          copied += seg->range.size;
        }
        size_t remaining = seg->pad_length - copied;
        memcpy(c, seg->range.begin, remaining);
        c += remaining;
      } break;
    }
  }

  if (stb->args->size) {
    // not yet implemented...
  }

  String* array_foreach(to_delete, to_xibalba) {
    str_delete(to_delete);
  }
  array_delete(&to_xibalba);
  array_delete(&stb->segments);
  array_delete(&stb->args);
  free(stb);
  *stb_in_ptr = NULL;

  return str_terminate(ret);
}

