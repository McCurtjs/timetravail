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
  { .begin = &str_chr_literal_empty, .size = 0 },
  { .begin = "true", .size = 4 },
  { .begin = "false", .size = 5 },
};
static String const str_constants_end = &str_constants[0] + ARRAY_COUNT(str_constants);

const String str_empty  = &str_constants[0];
const String str_va_end = &str_constants[1];
const String str_true   = &str_constants[2];
const String str_false  = &str_constants[3];

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

String istr_copy(StringRange str) {
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

String istr_join(StringRange del, const Array_StringRange strings) {
  const size_t range_count = strings->size;
  if (range_count == 0) return str_empty;

  size_t length = 0;

  StringRange* array_foreach(range, strings) {
    length += range->size;
  }

  length += del.size * (range_count - 1);

  String_Internal* ret = str_new_internal(length);
  if (!ret) return str_empty;

  char* dst = ret->begin;

  array_foreach_index(range, i, strings) {
    memcpy(dst, range->begin, range->size);
    dst += range->size;

    if (i != range_count - 1) {
      memcpy(dst, del.begin, del.size);
      dst += del.size;
    }
  }

  return str_terminate(ret);
}

String istr_concat(StringRange left, StringRange right) {
  size_t length = left.size + right.size;
  String_Internal* ret = str_new_internal(length);
  if (!ret) return str_empty;
  memcpy(ret->begin, left.begin, left.size);
  memcpy(ret->begin + left.size, right.begin, right.size);
  return str_terminate(ret);
}

String istr_prepend(StringRange str, size_t length, char c) {
  String_Internal* ret = str_new_internal(str.size + length);
  memset(ret->begin, c, length);
  memcpy(ret->begin + length, str.begin, str.size);
  return str_terminate(ret);
}

String istr_append(StringRange str, size_t length, char c) {
  String_Internal* ret = str_new_internal(str.size + length);
  memcpy(ret->begin, str.begin, str.size);
  memset(ret->begin + str.size, c, length);
  return str_terminate(ret);
}

Array_StrR istr_split(StringRange str, StringRange del) {
  Array_StrR ret = arr_str_new();

  // specialization for empty delimiter, return a range for each char
  if (del.size == 0) {
    arr_str_reserve(ret, (uint)str.size);
    for (size_t i = 0; i < str.size; ++i) {
      StringRange c = str_range_s(&str.begin[i], 1);
      arr_str_push_back(ret, c);
    }
    return ret;
  }

  int i = 0;
  do {
    int prev = i;
    i = (int)istr_index_of(str, del, i);
    StringRange range = istr_substring(str, prev, i);
    arr_str_push_back(ret, range);
    i += (int)del.size;
    if (i == (int)str.size) arr_str_push_back(ret, str_empty->range);
  } while (i < (int)str.size);

  return ret;
}

StringRange istr_substring(StringRange str, int start, int end) {
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

StringRange istr_trim(StringRange str) {
  StringRange ret = istr_trim_start(str);
  return istr_trim_end(ret);
}

StringRange istr_trim_start(StringRange str) {
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

StringRange istr_trim_end(StringRange str) {
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

bool istr_eq(StringRange lhs, StringRange rhs) {
  if (lhs.size != rhs.size) return FALSE;
  return memcmp(lhs.begin, rhs.begin, lhs.size) == 0;
}

bool istr_starts_with(StringRange str, StringRange starts) {
  if (starts.size > str.size) return FALSE;
  return memcmp(str.begin, starts.begin, starts.size) == 0;
}

bool istr_ends_with(StringRange str, StringRange ends) {
  if (ends.size > str.size) return FALSE;
  return memcmp(str.begin + str.size - ends.size, ends.begin, ends.size) == 0;
}

bool istr_contains(StringRange str, StringRange check) {
  return istr_find(str, check) != str.size;
}

size_t istr_index_of_char(StringRange str, char c, size_t from_pos) {
  if (from_pos >= str.size) return str.size;
  for (size_t i = from_pos; i < str.size; ++i) {
    if (str.begin[i] == c) {
      return i;
    }
  }
  return str.size;
}

size_t istr_index_of(StringRange str, StringRange to_find, size_t from_pos) {
  if (str.size < to_find.size) return str.size;
  if (to_find.size == 0) return MIN(from_pos, str.size);
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

size_t istr_find(StringRange str, StringRange to_find) {
  return istr_index_of(str, to_find, 0);
}

////////////////////////////////////////////////////////////////////////////////
// str_format
////////////////////////////////////////////////////////////////////////////////

// TODO: ? (not so useful when requiring ranges. Maybe make something
//          like this for string builder?)
// String _str_format(StringRange fmt, const StringRange[] argv, uint argc);
// #define _str_format_va(fmt, argv, argc, ...) _str_format(fmt, argv, argc)
// #define str_format(...) _str_fmt2(__VA_ARGS__, NULL, 0)
// TODO: verify that non-macro va args are actaully not a thing with wasi, lol

#define con_type byte
#include "array.h"
#undef con_type

const _Str_FmtArg _str_fmtarg_end = { .type = _Str_FmtArg_End, .i = 0 };

enum _Str_FmtSpec_Alignment {
  _Str_FmtSpec_Left,
  _Str_FmtSpec_Center,
  _Str_FmtSpec_Right
};

typedef struct {
  byte index;
  byte width;               // 0 = no padding
  byte precision;           // 0 = default of 1
  byte alignment : 2;       // 0 = center
  byte sign : 1;            // 0 = - only, 1 = + for positive numbers
  byte trailing_zeroes : 1; // 0 = don't, 1 = do ; only for decimals
  byte representation : 2;  // 0 = default, 1 = hex, 2 = char, 3 = binary
} _Str_FmtSpec;

// handle format specifier
// {[index][:(+)(<^>)(width)(.precision[+])]}
// basic: "values: {}, {}, {}",                   1, 2, "hi"-> "values: 1, 2, hi"
// index: "values: {2}, {1}, {0}",                1, 2, 3   -> "values: 3, 2, 1"
// width: "values: {:4}, {0:4}",                  1         -> "values: 1   , 1   "
// align: "values: |{0:<4}|{0:>4}|{0:^4}|{0:^5}", 1         -> "values: |1   |   1| 1  |  1  |"
// preci: "values: |{0:.5}|{0:^.1}|{0:>10.6+}|",  1.23      -> "values: |1.23|1.2|1.230000  |"
// types: "values: {0:i} {0:h} {0:c}",            65        -> "values: 65, 41, A"
// date format specifiers? (ie, {%d} for "Monday" (use standard)
static _Str_FmtSpec _str_fmt_read_spec(StringRange spec_str, byte arg_index) {

  // set up the format specifier struct
  _Str_FmtSpec spec = { 0 };
  spec.index = arg_index;

  size_t colon_offset = 0;

  // no explicit argument index given, use the next in the list
  if (spec_str.size == 0) {
    return spec;

  // format does not start with colon, meaning there is a format index
  // if the format does start with a colon, use next index but still format
  } else if (spec_str.begin[0] != ':') {
    colon_offset = istr_index_of(spec_str, R(":"), 0);
    // arg_index = str_parse_int(istr_substring(spec_str, 0, spec_str.size));
    spec.index = arg_index;
  }

  // if no colon is in the string, the rest of the spec is defaults
  if (colon_offset == spec_str.size) {
    return spec;
  }
  /*
  // otherwise, parse the format specifier
  // TODO: add 'end' as a size alias so the check can be against 'spec_str.end'?
  size_t width_start = 0;
  size_t width_end = 0;
  size_t prec_start = 0;
  size_t prec_end = 0;

  for (size_t i = 1; i < spec_str.size; ++i) {
    byte c = spec_str.begin[i];

    // prefix options (sign, alignment)
    if (!width_start) {

      if (isdigit(c)) {
        width_start = i;

      }
      else if (c == '<') {
        spec.alignment = _Str_FmtSpec_Left;

      }
      else if (c == '>') {
        spec.alignment = _Str_FmtSpec_Right;

      }
      else if (c == '^') {
        spec.alignment = _Str_FmtSpec_Center;

      }
      else if (c == '+') {
        spec.sign = 1;

      }
      else if (c == '-') {
        spec.sign = 0;
      }

      // seek for decimal precision marker
    }
    else if (!width_end && !prec_start) {

      if (c == '.') {
        width_end = i;
        prec_start = i + 1;
      }

      // post 
    }
    else if (prec_start) {

      // mark the end of the precision field
      if (!isdigit(c)) {
        if (!prec_end) {
          prec_end = i;
        }

        // post-precision flags
        if (c == '+') {
          spec.trailing_zeroes = 1;
        }

      }

    }

  }

  if (!width_end) {
    width_end = spec_str.size;
  }

  if (!prec_end) {
    prec_end = spec_str.size;
  }


  spec_str = istr_substring(spec_str, 1, spec_str.size);

  // handle specifier prefix
  switch (*spec_str.begin) {
  }

  // handle sign (+) suffix
  if (spec_str.begin[spec_str.size - 1] == '+') {
    spec.sign = 1;
  }
  //*/
  return spec;
}

static void _str_fmt_print_arg(Array_byte out, Array params, _Str_FmtSpec spec) {

  if (spec.index >= params->size) {
    return;
  }

  _Str_FmtArg* arg = array_get_ref(params, spec.index);

  switch (arg->type) {

    case _Str_FmtArg_StringRange: {

      byte* bytes = arr_byte_emplace_back_range(out, (uint)arg->range.size);
      memcpy(bytes, arg->range.begin, arg->range.size);

    } break;

    default: {

      arr_byte_push_back(out, ':');
      arr_byte_push_back(out, '(');

    } break;

  }

}

String istr_format(StringRange fmt, ...) {
  // TODO: better error handling on memory errors
  // TODO: Move this to its own section, possibly want to split out a new 
  //    header just for this function, especially if other dependent types
  //    end up being supported (such as vec3).
  Array params = array_new(_Str_FmtArg);
  uint reserve_size = (uint)(sizeof(String_Internal) + fmt.size);

  _Str_FmtArg arg;
  va_list args;

  // Read all the params into an array and calculate an approximate size
  va_start(args, fmt);

  loop {
    arg = va_arg(args, _Str_FmtArg);

    until(arg.type == _Str_FmtArg_End);

    reserve_size += (arg.type == _Str_FmtArg_StringRange) ? (uint)arg.range.size : 3;

    array_write_back(params, &arg);
  }

  va_end(args);

  // Set the starting allocation for the new string
  Array_byte output = arr_byte_new_reserve(reserve_size);

  // Push the String header to the front of the array data
  String_Internal* header = (String_Internal*)arr_byte_emplace_back_range(
    output, sizeof(struct _Str_Base)
  );

  if (!header) return str_empty;

  // Process the format string
  byte arg_index = 0;

  // track position in each section between formatters
  size_t section_start = 0;
  uint section_size = 0;

  for (size_t i = 0; i < fmt.size; ++i) {
    byte c = fmt.begin[i];

    // just do a 1:1 copy by character until we hit a format specifier
    if (c != '{') {
      ++section_size;
      continue;
    }

    // at the start of a format section, copy all the bytes up to this point
    if (section_size) {
      byte* bytes = arr_byte_emplace_back_range(output, section_size);
      memcpy(bytes, fmt.begin + section_start, section_size);
    }

    section_size = 0;

    // handle case for "{{" to print escaped left brace
    if (fmt.begin[i + 1] == '{') {
      arr_byte_push_back(output, '{');
      ++i;
      section_start = ++i;
      continue;
    }

    // get format specifier contents
    size_t spec_end = istr_index_of(fmt, R("}"), i+1);

    // case for un-closed brace
    if (spec_end == fmt.size) {
      section_size = (uint)(fmt.size - section_start);
      break;
    }

    StringRange spec_str = istr_substring(fmt, (int)i + 1, (int)spec_end);
    _Str_FmtSpec spec = _str_fmt_read_spec(spec_str, arg_index);
    arg_index = spec.index + 1;

    _str_fmt_print_arg(output, params, spec);

    i = spec_end;
    section_start = spec_end + 1;
  }

  if (section_size) {
    byte* bytes = arr_byte_emplace_back_range(output, section_size);
    memcpy(bytes, fmt.begin + section_start, section_size);
  }

  array_delete(&params);

  arr_byte_push_back(output, '\0');
  header->size = output->size - sizeof(struct _Str_Base);
  header->begin = &header->head;
  String ret = (String)arr_byte_release(&output);

  return ret;
}
