#ifndef _DONK_STRING_H_
#define _DONK_STRING_H_

#include "types.h"

#include "array.h"

#define _STR_RANGE_DEF(SRCA, SRCB)  \
  struct {                          \
    SRCA char*  SRCB begin;         \
    union {                         \
      size_t    SRCB length;        \
      size_t    SRCB size;          \
    };                              \
  }                                 //

#define _STR_RANGE_DEF_BODY(C_STR)  \
  .begin = C_STR,                   \
  .size = sizeof(C_STR)-1           //

// \brief StringRange is a basic immutable string segment containing the start
//    and size of a string.
//
// \brief The Range object does not have ownership of the string and related
//    functions will not allocate or free any memory for a returned Range.
//    Because of this, they're not suited for long-term tracking of a string
//    object unless that string is a compile-time constant string literal
//    (which you can wrap using the str_literal, R, or str_static macros).
//
// \brief A StringRange can be created referencing any of the following:
//    1) a constant string literal (using R, str_literal, or str_static).
//    2) a range within a String object (including the implicit str->range).
//    3) a or a range along a standard C-style char* string (str_range).
typedef _STR_RANGE_DEF(const,) StringRange;

// \brief Creates a string range from a string literal - ONLY use this for
//    literal string values (eg: "abc"). Will not copy or take ownership of
//    the string's memory.
// \brief Passing a char* to this will fail because the length is calculated
//    at compile time. For a runtime string range, use str_range(c_str).
//
// \param C_STRING_LITERAL - The string literal value.
//    Can accept either a string in double quotes, or a const static char[].
//
#define str_literal(C_STRING_LITERAL) ((StringRange) {  \
  _STR_RANGE_DEF_BODY(C_STRING_LITERAL)                 \
})                                                      //

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

// \brief Similar to R macro, just doesn't include the typename because MSVC
//    can't handle that in some cases. Very annoying.
#define M(C_STRING_LITERAL) {           \
  _STR_RANGE_DEF_BODY(C_STRING_LITERAL) \
}                                       //

// \brief Used to allocate a static string from a string literal. This is only
//    necessary in MSVC because it can't understand initializer list casting.
//
// \param NAME - the name symbol for the static variable
//
// \param C_STR - the compile-time constant string literal value.
//
#define str_static(NAME, C_STR) static StringRange NAME = { \
  _STR_RANGE_DEF_BODY(C_STR)                                \
}                                                           //

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
    _STR_RANGE_DEF(const, const);
  };
}* String;

#define con_type StringRange
#define con_prefix str
#include "array.h"
#undef con_type
#undef con_prefix
typedef Array_StringRange Array_StrR;

#ifdef _MSC_VER
// Annoyingly, MSVC for some reason detects the _Generic specifier as "unused".
#pragma warning ( disable : 4189 ) // local initialized but not referenced
#endif
// \brief Macro to coalesce a String, StringRange, or char* into a StringRange.
#define _s2r(S) _Generic((S),   \
  StringRange:  _str_range_r,   \
  String:       _str_range_st,  \
  char*:        str_range,      \
  const char*:  str_range       \
)(S)                            //

extern const String str_empty;
extern const String str_va_end;
extern const String str_true;
extern const String str_false;

StringRange str_range(const char* c_str);
StringRange str_range_s(const char* c_str, size_t length);

String  str_new(const char* c_str);
String  str_new_s(const char* c_str, size_t length);
#define str_copy(str) istr_copy(_s2r(str))
String  str_from_bool(bool b);
String  str_from_int(int i);
String  str_from_float(float f);

void    str_delete(String* str);

#define str_eq(lhs, rhs)            istr_eq(_s2r(lhs), _s2r(rhs))
#define str_starts_with(str, start) istr_starts_with(_s2r(str), _s2r(start))
#define str_ends_with(str, end)     istr_ends_with(_s2r(str), _s2r(end))
#define str_contains(str, check)    istr_contains(_s2r(str), _s2r(check))

// \brief prefer s.size, but can be useful in cases where a function is needed.
//
// \returns s.size
#define str_size(str)               _str_size(_s2r(str))

// \brief Gets the start of the next instance of to_find in str, starting
//    at from_pos.
//
// \returns
//    The index in str of the match, or if none is present, returns str.size.
#define str_index_of(str, to_find, from_pos) \
                     istr_index_of(_s2r(str), _s2r(to_find), from_pos)

// \brief Alias for str_index_of(str, to_find, 0)
#define str_find(str, to_find) istr_find(_s2r(str), _s2r(to_find))

// \brief `StringRange str_substring(str, start, ?end)`
// \brief Gets a substring as a range within the input string range.
// \brief Works like javascript string.slice.
//
// \param str The string range to get a substring of. The returned substring's
//    lifetime will be dependent on the lifetime of str.
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
#define str_substring(str, ...) _str_substring(str, __VA_ARGS__)
#define str_slice(str, ...)     _str_substring(str, __VA_ARGS__)

#define str_trim(str)       istr_trim(_s2r(str))
#define str_trim_start(str) istr_trim_start(_s2r(str))
#define str_trim_end(str)   istr_trim_end(_s2r(str))

// \brief Splits the string into an array of substrings based on the delimiter.
//
// \param str - The string to split into pieces.
//
// \param del - The substring to split the input along. Instances of the
//    delimiter are removed from the resulting substrings.
//
// \returns An array of StringRanges whose lifetimes are bound to str.
//    The Array must be deleted by the user via arr_str_delete(&arr).
#define str_split(str, del) istr_split(_s2r(str), _s2r(del))

// \brief Joins an array of string ranges into a new string, each separated by a
//    given delimiter.
//
// \param del - the delimiter to insert between each string in the array.
//   ex: (" + ", ["A", "B"]) will result in "A + B"
//
// \param strings - The array of string ranges to join.
//
// \returns a new string, which must be deleted later by the caller.
//
#define str_join(del, strings)      istr_join(_s2r(del), strings)
#define str_concat(left, right)     istr_concat(_s2r(left), _s2r(right))
#define str_replace(str, tok, w)    istr_replace(_s2r(str), _s2r(tok), _s2r(w))
#define str_replace_all(s, t, w)    istr_replace_all(_s2r(s), _s2r(t), _s2r(w))
#define str_prepend(str, length, c) istr_prepend(_s2r(str), length, c)
#define str_append(str, length, c)  istr_append(_s2r(str), length, c)

//String str_pad_left(StringRange str, size_t length, char c);
//String str_pad_right(StringRange str, size_t length, char c);

static inline StringRange _str_range_st(const String str) { return str->range; }
static inline StringRange _str_range_r(StringRange range) { return range; }
static inline size_t      _str_size(StringRange s) { return s.size; }

String      istr_copy(StringRange str);
bool        istr_eq(StringRange lhs, StringRange rhs);
bool        istr_starts_with(StringRange str, StringRange starts);
bool        istr_ends_with(StringRange str, StringRange ends);
bool        istr_contains(StringRange str, StringRange check);
size_t      istr_index_of(StringRange str, StringRange to_find, size_t from);
//size_t    istr_index_of_last(StringRange str, StringRange find, size_t from);
size_t      istr_find(StringRange str, StringRange to_find);
//size_t    istr_find_last(StringRange str, StringRange to_find);
//Array     istr_match(StringRange str, StringRange regex);
StringRange istr_substring(StringRange str, int start, int end);
StringRange istr_trim(StringRange str);
StringRange istr_trim_start(StringRange str);
StringRange istr_trim_end(StringRange str);
Array_StrR  istr_split(StringRange str, StringRange del);
//Array     istr_tokenize(StringRange str, const StringRange[] tokens);
//Array     istr_parenthetize(StringRange str); // block out segments by parens? ([{}])
String      istr_join(StringRange deliminter, const Array_StrR strings);
String      istr_concat(StringRange left, StringRange right);
// for replace, start with basic string replace, maybe later look into adding regex support?
//    differentiate between regular strings and regex with the regular "a" vs "/a/"
//String    istr_replace(StringRange str, StringRange to_rep, StringRange with);
//String    istr_replace_all(StringRange str, StringRange r, StringRange w);
//String    istr_format(StringRange fmt, ...);
String      istr_prepend(StringRange str, size_t length, char c);
String      istr_append(StringRange str, size_t length, char c);
//String    istr_to_upper(StringRange str);
//String    istr_to_lower(StringRange str);
//String    istr_to_title(StringRange str);

#define _str_sub_args(str, start, end, ...) _s2r(str), (int)start, (int)end
#define _str_sub_a(str, ...) _str_sub_args(str, __VA_ARGS__, _s2r(str).size)
#define _str_substring(str, ...) istr_substring(_str_sub_a(str, __VA_ARGS__))

/*
typedef struct {
  int type;
  union {
    StringRange range;
    long long int i;
    long long unsigned int ui;
    double f;
  };
} _Str_FmtArg;

#define _str_format(str, ...) istr_format(str, _va_exp(_s2r, __VA_ARGS__), str_va_end)
#define str_format(str, ...) _str_format(_s2r(str), __VA_ARGS__)
//*/

////////////////////////////////////////////////////////////////////////////////

typedef struct {
  union {
    size_t CV length;
    size_t CV size;
  };
}* StringBuilder;

StringBuilder stb_new();
StringBuilder stb_str(StringBuilder stb, String s); // these will be deleted
StringBuilder stb_range(StringBuilder stb, StringRange r);
StringBuilder stb_c_str(StringBuilder stb, const char* c_str);
StringBuilder stb_pad(StringBuilder stb, size_t length, char c);
StringBuilder stb_pad_range(StringBuilder stb, size_t length, StringRange r);

/*
void stb_arg_str(StringBuilder stb, const String s);
void stb_arg_c_str(StringBuilder stb, const char* c);
void stb_arg_range(StringBuilder stb, StringRange r);
void stb_arg_int(StringBuilder stb, int i);
void stb_arg_float(StringBuilder stb, float f);
*/

String stb_resolve(StringBuilder* stb);

#endif
