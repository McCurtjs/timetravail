#undef malloc
#undef realloc
#undef calloc
#undef free

#include "tst.h"
#include "types.h"

#include <stdlib.h>
#include <string.h>

typedef enum PrintLevel {
  NOT_PRINTED = 0,
  LOGGED,
  PRINTED
} PrintLevel;

// TODO: take all these and split them into a meta-context object so we
// can at least pretend to be thread-safe.
static const StringRange* test_function = NULL;
static const StringRange* test_description = NULL;
static const TestSuite* current_suite = NULL;
static bool test_filename_printed = FALSE;
static bool test_function_printed = FALSE;
static PrintLevel test_desc_printed = NOT_PRINTED;
static bool test_failed = FALSE;
static bool test_skipped = FALSE;
static bool test_in_progress = FALSE;
static int test_current_line = 0;
static int test_count = 0;
static int test_passed_count = 0;

static bool param_verbose = FALSE;
static int param_line = 0;
static StringRange* param_file = NULL;

static int memory_count_mallocs = 0;
static int memory_count_frees = 0;

void* malloc_test(size_t size) {
  ++memory_count_mallocs;
  return malloc(size);
}

void* calloc_test(size_t ct, size_t sel) {
  ++memory_count_mallocs;
  return calloc(ct, sel);
}

void* realloc_test(void* mem, size_t nsize) {
  if (mem == NULL) ++memory_count_mallocs;
  return realloc(mem, nsize);
}

void free_test(void* mem) {
  ++memory_count_frees;
  /* TODO: test for double-free */
  free(mem);
}

static bool test_blank() {
  return test_description == NULL || test_description->size == 0;
}

static void print_headers(int desc_color, uint desc_level) {

  if (!test_filename_printed) {
    str_print_color(current_suite->header, CONCOL_bCyan);
    test_filename_printed = TRUE;
  }

  if (!test_function_printed) {
    str_print_color(*test_function, CONCOL_Purple);
    test_function_printed = TRUE;
  }

  if (test_blank()) {
    if (!test_desc_printed) {
      print("    Pre-test:");
      test_desc_printed = LOGGED;
    }
  } else if (test_desc_printed < desc_level) {
    str_print_color(*test_description, desc_color);
    test_desc_printed = desc_level;
  }
}

bool _test_context(int line) {
  return TRUE;
}

bool _test_begin(int line, const StringRange* desc) {
  if (test_current_line > line) {
    return FALSE;
  }

  test_description = desc;
  test_desc_printed = NOT_PRINTED;
  test_failed = FALSE;
  memory_count_mallocs = 0;
  memory_count_frees = 0;
  /* TODO: store memory 'end' pointer here to not count context allocations? */

  if (param_line == 0 || param_line == line) {
    test_in_progress = TRUE;
    test_skipped = FALSE;

  } else {
    test_in_progress = FALSE;
    test_skipped = TRUE; // not needed

    if (param_verbose) {
      print_headers(CONCOL_Blue, LOGGED);
    }
  }

  return test_in_progress;
}

bool _test_end(int line) {
  if ((line && test_current_line >= line) || !test_in_progress) {
    return FALSE;
  }

  // TODO: I don't think we need to check test_blank anymore (thanks to in_prog)
  if (!test_blank() && !test_failed) {

    if (memory_count_mallocs != memory_count_frees) {
      int malloc_count = (int)memory_count_mallocs;
      int free_count = (int)memory_count_frees;
      String str_m = str_from_int(malloc_count);
      String str_f = str_from_int(free_count);
      Array arr = array_new(StringRange);
      array_push_back(arr, &R("      mismatched malloc/free calls: "));
      array_push_back(arr, &str_m->range);
      array_push_back(arr, &R("/"));
      array_push_back(arr, &str_f->range);
      String to_print = str_join(str_empty->range, arr);
      _test_error(&to_print->range);
      array_delete(&arr);
      str_delete(&str_m);
      str_delete(&str_f);
      str_delete(&to_print);
    }

    /* TODO: do real memory checks for malloc and allocate garbage */
  }

  ++test_count;

  if (!test_failed) {
    ++test_passed_count;

    if (param_verbose || param_line) {
      print_headers(CONCOL_Green, LOGGED);
    }
  }

  test_in_progress = FALSE;

  return TRUE;
}

void _test_log(const StringRange* message) {

  if (!param_verbose) return;
  print_headers(CONCOL_White, LOGGED);
  str_print(*message);
}

void _test_warn(const StringRange* message) {
  print_headers(CONCOL_Yellow, LOGGED);
  str_print_color(*message, CONCOL_Yellow);
}

void _test_error(const StringRange* message) {
  print_headers(CONCOL_Red, PRINTED);
  str_print(*message);
  test_failed = TRUE;
}

static String resolve_param(StringRange* r, const void* v) {
  String formatted;
  int n = 1;

  switch (r->begin[0]) {
    case 'i': formatted = str_from_int(*(int*)v); break;
    case 'f': formatted = str_from_float(*(float*)v); break;
    case 'b': formatted = str_from_bool(*(bool*)v); break;

    default:
      return str_new(" - Error formatting: Nonexistent type specifier");
    break;
  }

  String result = str_concat(formatted->range, str_substring(*r, n));
  str_delete(&formatted);
  return result;
}

void _test_error_params(const StringRange* fmt, const void* a, const void* b) {
  Array split = str_split(*fmt, R("%"));

  if (split->size != 3) {
    String to_print = str_concat(*fmt, R(" - Error formatting, expect two % specifiers"));
    _test_error(&to_print->range);
    str_delete(&to_print);
  } else {
    String first = resolve_param(array_get(split, 1), a);
    String second = resolve_param(array_get(split, 2), b);
    array_pop_back(split);
    array_pop_back(split);
    array_push_back(split, &first->range);
    array_push_back(split, &second->range);
    String result = str_join(str_empty->range, split);
    str_delete(&first);
    str_delete(&second);
    _test_error(&result->range);
    str_delete(&result);
  }

  array_delete(&split);
}

static void before_run() {
  test_count = 0;
  test_passed_count = 0;
}

static void before_suite(const TestSuite* suite) {
  current_suite = suite;
  test_filename_printed = FALSE;
  test_failed = FALSE;
}

static void before_fn(const TestGroup* t) {
  test_function_printed = FALSE;
  test_function = &t->header;
}

static void process_function(const TestGroup* t) {
  before_fn(t);
  test_current_line = 0;
  int ctx = 0;
  do {
    test_current_line = t->group_fn(test_current_line, ctx);
  } while(test_current_line);
}

void test_run_suite(const TestSuite* suite) {
  before_suite(suite);

  if (param_file && !str_ends_with(suite->filename, *param_file)) {
    if (param_verbose) {
      String msg = str_concat(R("skipping file: %c"), suite->filename);
      str_print_color(msg->range, CONCOL_Cyan);
      str_delete(&msg);
    }
    return;
  }

  const TestGroup* t = &suite->test_groups[0];
  while (t->line != 0) {
    if (t->line == param_line) {
      int tmp = param_line;
      param_line = 0;
      process_function(t);
      param_line = tmp;
      goto end_suite;
    }
    ++t;
  }

  t = &suite->test_groups[0];
  while (t->line != 0) {
    process_function(t++);
  }

end_suite:

  current_suite = NULL;
}

static void process_args(int argc, char* argv[]) {
  for (int i = 1; i < argc; ++i) {
    StringRange param = str_range(argv[i]);

    if (str_starts_with(param, R("-"))) {
      if (str_eq(param, R("-v"))) {
        param_verbose = TRUE;
      }
    } else {
      // Can't use str_split or other functions that allocate here or it'll
      // pollute the test allocator! Use only in-place functions from str.h.
      size_t sep = str_find(param, R(":"));
      if (sep != param.size) {
        param_line = atoi(str_substring(param, sep + 1).begin);
      }

      // Zero-length, don't bother. In this case, the string was entered as ":3"
      // so we'll take the number, but not single it to a file. Maybe someone
      // meticulously puts a specific test on one line of every file, who knows.
      if (sep != 0) {
        param_file = malloc(sizeof(StringRange));
        StringRange to_copy = str_substring(param, 0, sep);
        memcpy(param_file, &to_copy, sizeof(StringRange));
      }
    }
  }
}

int _test_run_all(int count, TestSuite* suites[], int argc, char* argv[]) {
  process_args(argc, argv);
  before_run();

  for (int i = 0; i < count; ++i) {
    test_run_suite(suites[i]);
  }

  int color = test_count == test_passed_count ? CONCOL_bGreen : CONCOL_bRed;
  int ratio = 100;
  if (test_count) {
    ratio = (int)(100.f * (float)test_passed_count / (float)test_count);
  }

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

  // return the number of failed tests
  return test_count - test_passed_count;
}
