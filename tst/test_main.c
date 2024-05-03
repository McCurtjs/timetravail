

#include "wasm.h"
#include "str.h"
#include "cspec.h"

int export(canary) (int nothing) {
  PARAM_UNUSED(nothing);
  print("WASM is connected! Only running tests.");
  return 1;
}

csUint resolve_types (
  const char** p_type, const void* value, char* out_buffer, csUint out_size
) {
  StringRange type = str_range(*p_type);
  const StringRange* string_range = NULL;

  if (str_eq(type, "StringRange*")) {
    string_range = *(const StringRange**)value;
  }
  else if (str_eq(type, "StringRange")) {
    string_range = (const StringRange*)value;
  }
  else if (str_eq(type, "String")) {
    string_range = &(*(const String*)value)->range;
  }

  if (string_range) {
    csUint w = 0;
    if (str_ends_with(type, "*") && w < out_size) out_buffer[w++] = '&';
    out_buffer[w++] = '"';
    for (csUint i = 0; w < out_size - 1 && i < string_range->size; ++i) {
      out_buffer[w++] = string_range->begin[i];
    }
    out_buffer[w++] = '"';

    return w;
  }

  return 0;
}

// Test suites

extern TestSuite tests_cspec;
extern TestSuite tests_string;

// Main

#ifdef __WASM__
static char* argv[] = {"WASM", "-v"};
static int argc = sizeof(argv) / sizeof(char*);
int export(wasm_tests) ()
#else
int main(int argc, char* argv[])
#endif
{
  resolve_user_types = resolve_types;

  TestSuite* test_suites[] = {
    &tests_cspec,
    &tests_string
  };

  return test_run_all(test_suites);
}
