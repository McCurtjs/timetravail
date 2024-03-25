#ifndef _DONK_STRING_H_
#define _DONK_STRING_H_

#include "types.h"

#include "array.h"

#define _STR_RANGE_DEF(SRC)   \
  struct {                    \
    SRC char* SRC begin;      \
    union {                   \
      size_t SRC length;      \
      size_t SRC size;        \
    };                        \
  }                          //

#define _STR_RANGE_DEF_BODY(C_STR)        \
  .begin = C_STR,                         \
  .size = sizeof(C_STR)-1                //

// \brief StringRange is a basic immutable string segment containing the start
//    and size of a string.
//
// \brief The Range object does not have ownership of the string and related functions
//    will not allocate or free any memory for a returned Range.
//    Because of this, they're not suited for long-term tracking of a string
//    object unless that string is a compile-time constant string literal
//    (which you can wrap using the str_literal, R, or str_static macros).
//
// \brief A StringRange can be created referencing any of the following:
//    1) a constant string literal (using R, str_literal, or str_static).
//    2) a range within a String object (including the implicit str->range).
//    3) a or a range along a standard C-style char* string (str_range). 
typedef _STR_RANGE_DEF(const) StringRange;

// \brief String is a handle type pointing to an immutable string on the heap.
//
// \brief Unlike StringRange, a String object has ownership of the data in its
//    contained range. Any String object returned from a function will have to
//    later be freed via str_delete(). Any StringRange returned from a function
//    using the String's range will have its lifecycle bound to the String, and
//    will be invalid once the String object is deleted.
typedef struct _Str_Base {
  union {
    const StringRange range;
    _STR_RANGE_DEF(const);
  };
}* String;

extern const String str_empty;
extern const String str_true;
extern const String str_false;

// \brief Creates a string range from a string literal - ONLY use this for
//    literal string values (eg: "abc"). Will not copy or take ownership of
//    the string's memory.
// \brief Passing a char* to this will fail because the length is calculated
//    at compile time. For a runtime string range, use str_range(c_str).
//
// \param C_STRING_LITERAL - The string literal value.
//    Can accept either a string in double quotes, or a const static char[].
//
#define str_literal(C_STRING_LITERAL) ((StringRange) { \
  _STR_RANGE_DEF_BODY(C_STRING_LITERAL)                \
})                                                    //

// \brief Alias for str_literal(c_str).
// \brief Creates a string range from a string literal - ONLY use this for
//    literal string values (eg: "abc"). Will not copy or take ownership of
//    the string's memory.
// \brief Passing a char* to this will fail because the length is calculated
//    at compile time. For a runtime string range, use str_range(c_str).
//
// \param C_STRING_LITERAL - The string literal value.
//    Can accept either a string in double quotes, or a const static char[].
//
#define R(C_STR) str_literal(C_STR)

// \brief Used to allocate a static string from a string literal. This is only
//    necessary in MSVC because it can't understand initializer list casting.
//
// \param NAME - the name symbol for the static variable
//
// \param C_STR - the compile-time constant string literal value.
//
#define str_static(NAME, C_STR) static StringRange NAME = { \
  _STR_RANGE_DEF_BODY(C_STR)                                \
}                                                          //

String str_new(const char* c_str);
String str_new_s(const char* c_str, size_t length);
String str_copy(StringRange str);
String str_from_bool(bool b);
String str_from_int(int i);
String str_from_float(float f);

StringRange str_range(const char* c_str);
StringRange str_range_s(const char* c_str, size_t length);

void str_delete(String* str);

// \brief Joins an array of strings into a new string, each separated by a 
//    given delimiter. The array can be of either Strings or StringRanges.
//
// \param del - the delimiter to insert between each string in the array.
//   ex: (" + ", ["A", "B"]) will result in "A + B"
//
// \param strings - The array of strings to join.
//    The array can be one of the following:
//    1) an array of StringRange objects.
//    2) an array of any mix of String and StringRange*. Note: in this case,
//      make sure not to include String* in the array.
// 
// \returns a new string, which must be deleted later by the caller.
//
String str_join(StringRange deliminter, const Array strings);
String str_concat(StringRange left, StringRange right);

// \brief Splits the string into an array of substrings based on the delimiter.
//
// \param str - The string to split into pieces.
//
// \param del - The substring to split the input along. Instances of the
//    delimiter are removed from the resulting substrings.
//
// \returns An array of StringRanges whose lifetimes are bound to str.
//    The Array must be deleted by the user via array_delete(&arr).
Array  str_split(StringRange str, StringRange del);

StringRange _str_substring(StringRange str, int start, int end);
StringRange str_trim(StringRange str);
StringRange str_trim_start(StringRange str);
StringRange str_trim_end(StringRange str);

bool str_eq(StringRange lhs, StringRange rhs);
bool str_starts_with(StringRange str, StringRange starts);
bool str_ends_with(StringRange str, StringRange ends);

// \brief Alias for str_index_of(str, to_find, 0)
size_t str_find(StringRange str, StringRange to_find);

// \brief Gets the start of the next instance of to_find in str, starting
//    at from_pos.
//
// \returns
//    The index in str of the match, or if none is present, returns str.end.
size_t str_index_of(StringRange str, StringRange to_find, size_t from_pos);

#define _STR_SUBSTR_(STR, START, END, ...) _str_substring(STR, (int)START, (int)END)
// \brief str_substring(str, start, ?end)
// \brief Gets a substring as a range within the input string range.
// \brief Works like javascript string.slice.
//
// \param str some notes about str
//
// \param start - The beginning of the subrange, inclusive.
//    - A non-negative value represents an offset from the beginning.
//    - A value less than zero represents an offset from the end.
//
// \param end - [optional] The end of the subrange, exclusive.
//    - A zero or positive value represents an offset from the beginning.
//    - A value less than one represents an offset from the end.
//
// \param __VA_ARGS__ - start, ?end
// 
// \returns a StringRange as a substring of the input range.
//
#define str_substring(str, ...) _STR_SUBSTR_(str, __VA_ARGS__, 0)

// \brief alias for str_substring(str, start, end)
#define str_slice(str, ...) _STR_SUBSTR_(str, __VA_ARGS__, 0)

#endif
