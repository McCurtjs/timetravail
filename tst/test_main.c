

#include "wasm.h"
#include "str.h"
#include "cspec.h"

str_static(str_test_1, "|Hello, this is a static string|");
str_static(str_test_2, "|This is another test!|");

int export(canary) (int nothing) {
  PARAM_UNUSED(nothing);
  print("WASM is connected! Only running tests.");
  return 1;
}

void string_tests() {
  print("Type Sizes");
  str_print(str_test_1);
  print_int(str_test_1.length);
  str_print(str_test_2);
  print_int(str_test_2.length);
  str_print(str_literal("This is an inline string literal"));
  str_print(R("This is an inline literal with short macro"));

  String tmp = str_concat(str_test_1, str_test_2);
  str_print(tmp->range);
  print_int(tmp->size);
  print_ptr(tmp);
  print_ptr(tmp->begin);
  str_delete(&tmp);
  print_ptr(tmp);

  Array ranges = array_new(StringRange);
  array_push_back(ranges, &R("First string"));
  array_push_back(ranges, &R("Second range"));
  array_push_back(ranges, &R("Another one!"));
  array_push_back(ranges, &R("And another!"));
  print("Joining strings...");
  tmp = str_join(R("--POTATO--"), ranges);
  str_print(tmp->range);
  print_int(tmp->size);
  str_delete(&tmp);
  array_delete(&ranges);
  print_ptr(ranges);
  print_ptr(tmp);

  String empty = str_new(NULL);
  String tmp1 = str_new("Copying as an actual string");
  String tmp2 = str_copy(R("Copied string"));
  String tmp3 = str_new("third string");
  StringRange* tmp4 = &R("Sneaky range pointer");

  print_ptr(tmp1);

  ranges = array_new(String);
  array_push_back(ranges, &tmp1);
  array_push_back(ranges, &tmp2);
  array_push_back(ranges, &empty);
  array_push_back(ranges, &tmp3);
  array_push_back(ranges, &tmp4);
  print("Joining strings...");
  tmp = str_join(R(" +++ "), ranges);
  str_print(tmp->range);
  print_int(tmp->size);
  str_delete(&tmp);
  str_delete(&tmp1);
  str_delete(&tmp2);
  str_delete(&tmp3);
  print_ptr(empty);
  str_delete(&empty);
  print_ptr(empty);
  array_delete(&ranges);

  str_print(str_substring(str_test_1, 1, 6));
  str_print(str_substring(R("xHELLOyz"), 1, 6));
  str_print(str_substring(R("xNO"), 1, 6));

  str_static(str_test_sub, "Static string for substr tests");
  str_print(str_substring(str_test_sub, 7));
  str_print(str_substring(str_test_sub, 7, -6));
  str_print(str_substring(str_test_sub, -5));
  str_print(str_substring(str_test_sub, 0, 13));
  str_print(str_substring(str_test_sub, str_test_sub.size, 0)); // empty

  print(str_eq(R("Is eq?"), R("Not eq")) ? "True" : "False");
  print(str_eq(R("Is eq?"), R("Is eq?")) ? "True" : "False");

  tmp = str_concat(R("Starts with 'Static': "), str_starts_with(str_test_sub, R("Static")) ? R("True") : R("False"));
  str_print(tmp->range);
  str_delete(&tmp);

  tmp = str_concat(R("Starts with 'Statics': "), str_starts_with(str_test_sub, R("Statics")) ? R("True") : R("False"));
  str_print(tmp->range);
  str_delete(&tmp);

  tmp = str_concat(R("Ends with 'Static': "), str_ends_with(str_test_sub, R("Static")) ? R("True") : R("False"));
  str_print(tmp->range);
  str_delete(&tmp);

  tmp = str_concat(R("Ends with 'tests': "), str_ends_with(str_test_sub, R("tests")) ? R("True") : R("False"));
  str_print(tmp->range);
  str_delete(&tmp);

  str_print(str_substring(str_test_sub, str_find(str_test_sub, R("for"))));
  str_print(str_substring(str_test_sub, str_find(str_test_sub, R("tests"))));
  str_print(str_substring(str_test_sub, str_find(str_test_sub, R("nonsense"))));
  str_print(str_substring(str_test_sub, str_find(str_test_sub, R(""))));

  print("Finding all substrs starting with 'st'");
  size_t tracker = 0;
  loop {
    tracker = str_index_of(str_test_sub, R("st"), tracker);
    until (tracker >= str_test_sub.size);
    str_print(str_substring(str_test_sub, tracker++, 0));
  }
  print("End of index checks");

  print("Split strings from array: (space)");
  Array substr_splits = str_split(str_test_sub, R(" "));
  for (uint i = 0; i < substr_splits->size; ++i) {
    StringRange* range = array_get(substr_splits, i);
    str_print(*range);
  }
  array_delete(&substr_splits);
  print("End of array");

  print("Split strings from array: (st)");
  substr_splits = str_split(str_test_sub, R("st"));
  for (uint i = 0; i < substr_splits->size; ++i) {
    StringRange* range = array_get(substr_splits, i);
    str_print(*range);
  }
  array_delete(&substr_splits);
  print("End of array");

  ranges = array_new(StringRange);
  StringRange trimmed_both = str_trim(R("  \t   String   needing trim    "));
  StringRange trimmed_front = str_trim_start(R("  \t   String   needing trim    "));
  StringRange trimmed_back = str_trim_end(R("  \t   String   needing trim    "));
  array_push_back(ranges, &R(""));
  array_push_back(ranges, &trimmed_front);
  array_push_back(ranges, &trimmed_back);
  array_push_back(ranges, &trimmed_both);
  array_push_back(ranges, &R(""));
  tmp = str_join(R("-"), ranges);
  str_print(tmp->range);
  str_delete(&tmp);
  array_delete(&ranges);
  str_print(str_trim(R("     This string needs trimmed!   ")));

  str_print(str_substring(R("Overload?"), 2));
  str_print(str_substring(R("Overload?"), 2, 0));

  tmp = str_from_bool(1);
  str_print(tmp->range);
  print_ptr(tmp);
  str_delete(&tmp);
  tmp = str_from_bool(0);
  str_print(tmp->range);
  print_ptr(tmp);
  str_delete(&tmp);
  tmp = str_from_bool(2);
  str_print(tmp->range);
  print_ptr(tmp);
  str_delete(&tmp);

  String num = str_from_int(132);
  tmp = str_concat(R("String converted from int: "), num->range);
  str_print(tmp->range);
  str_delete(&tmp);
  str_delete(&num);

  num = str_from_float(34.29f);
  tmp = str_concat(R("String converted from float: "), num->range);
  str_print(tmp->range);
  str_delete(&tmp);
  str_delete(&num);
}

// Test suites

extern TestSuite tests_cspec;
extern TestSuite tests_string;

// Main

#ifdef __WASM__
static char* argv[] = {"WASM", "-v", "-f"};
static int argc = sizeof(argv) / sizeof(char*);
int export(wasm_tests) ()
#else
int main(int argc, char* argv[])
#endif
{
  TestSuite* test_suites[] = {
    &tests_cspec,
    &tests_string
  };

  return test_run_all(test_suites);
}
