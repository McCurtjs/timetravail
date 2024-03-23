#ifndef _DONK_STRING_H_
#define _DONK_STRING_H_

#include "types.h"

#include "array.h"

#define STR_RANGE(SRC)        \
  struct {                    \
    SRC char* SRC begin;      \
    SRC char* SRC end;        \
    union {                   \
      size_t SRC length;      \
      size_t SRC size;        \
    };                        \
  }                          //

typedef STR_RANGE(const) StringRange;

typedef struct Str_Base {
  union {
    const StringRange range;
    STR_RANGE(const);
  };
  const bool is_static;
}* String;

#define STR_RANGE_DEF_BODY(C_STR) \
  .begin = C_STR, \
  .end = (char*)C_STR + sizeof(C_STR)-1, \
  .size = sizeof(C_STR)-1 //

#define str_literal(C_STR) ((StringRange) { \
  STR_RANGE_DEF_BODY(C_STR) \
}) //
#define R(C_STR) str_literal(C_STR)

#define str_static(NAME, C_STR) static StringRange NAME = { \
  STR_RANGE_DEF_BODY(C_STR) \
} //

//#define str_static(NAME, C_STR) static String NAME = (&(struct Str_Base){ \
//  { .begin = C_STR, .end = C_STR + sizeof(C_STR) } \
//  .size = sizeof(C_STR), .is_static = FALSE })

String str_new(const char* c_str);
String str_new_s(const char* c_str, size_t length);
String str_copy(StringRange str);
void str_delete(String* str);

String str_concat(StringRange left, StringRange right);
String str_join(StringRange deliminter, const Array string_ranges);

bool str_ends_with(StringRange str);
bool str_starts_with(StringRange str);

#endif
