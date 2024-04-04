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

typedef enum Verbosity {
  V_NONE,   //  0 verbosity level, only prints failures and warnings
  V_NOTES,  // -vn prints the above plus user notes
  V_RUN,    // -v prints the above plus passing tests
  V_VERY    // -va prints everything, even headers of tests that aren't run
} Verbosity;

// TODO: take all these and split them into a meta-context object so we
// can at least pretend to be thread-safe.
static const TestGroup* test_function = NULL;
static StringRange test_description = M("");
static const TestSuite* current_suite = NULL;
static bool test_filename_printed = FALSE;
static bool test_function_printed = FALSE;
static PrintLevel test_desc_printed = NOT_PRINTED;
static bool test_failed = FALSE;
static bool test_skipped = FALSE;
static bool test_in_progress = FALSE;
static bool test_expect_fail = FALSE;
static int test_current_line = 0;
static int test_count = 0;
static int test_passed_count = 0;

static Verbosity param_verbose = V_NONE;
static int param_line = 0;
static int param_tabsize = 2;
static StringRange param_file = M("");
static bool param_no_expect_fail = FALSE;

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

////////////////////////////////////////////////////////////////////////////////
// Test Context
////////////////////////////////////////////////////////////////////////////////
// A test context allows pre-test setup to be shared between
// multiple tests. Variables can be created and accessed within the tests, and
// other setup can be performed before running the tests. After each test, the
// test group function is exited and re-entered, meaning the context is
// recreated for every test (ie, incrementing a shared value in one test will
// not affect the next test), and after the context is passed, the setup won't
// be run again for any tests that follow it.

// To allow nested contexts, we need a stack... the stack persists for the whole
// test group (between multiple calls of the group function), and is used to
// keep track of
typedef struct Context {
  StringRange desc;
  bool printed;
  bool requested_context;
  struct Context* prev;
  struct Context* next;
} Context;

Context ctx_stack_root = {
  .desc = M("<root context>"),
  .printed = FALSE,
  .requested_context = FALSE,
  .prev = NULL,
  .next = NULL,
};

// Pointer to the top of the stack.
// The stack is cleared between each test group. Root node cannot be popped.
static Context* ctx_stack_top = &ctx_stack_root;

// Iterator through the stack.
// This is reset to the root between each each call to the test function.
static Context* ctx_stack_ptr = NULL;

// Called whenever the test enters a "context()" block
bool _test_context_begin(int line, StringRange desc) {

  // If we are currently executing a test, skip the context (allow previous
  // contexts to close out their post-test statements)
  if (test_in_progress) {
    return FALSE;
  }

  // On each pass of the test function, we have to walk up the stack. If our
  // context is already there, don't create a duplicate of it.
  if (ctx_stack_ptr->next && ctx_stack_ptr->next->desc.begin == desc.begin) {
    ctx_stack_ptr = ctx_stack_ptr->next;
    return TRUE;
  }

  // If we're completing execution of the context, we expect it to be at the
  // top of the stack
  if (ctx_stack_ptr->desc.begin == desc.begin) {
    return TRUE;
  }

  // If we're not on the stack anymore, and the current test line is past our
  // context, we've completed the tests in it and can skip it.
  if (test_current_line > line) {
    return FALSE;
  }

  // Any other context on the stack should still be open (and thus already
  // passed by the stack ptr), or have already closed out and be gone.
  assert(ctx_stack_ptr == ctx_stack_top);

  // If this context's line was specified in the input params, run all the
  // tests in this context, and end the tests as soon as it's popped.
  bool is_requested = FALSE;
  if (line == param_line) {
    is_requested = TRUE;
    param_line = 0;
  }

  // When this is added to the stack, we can set it as the current line.
  // (not strictly necessary, but good for bookkeeping?)
  test_current_line = line;

  // If we get here, we are entering a context for the first time.
  ctx_stack_top = malloc(sizeof(Context));
  ctx_stack_ptr->next = ctx_stack_top;

  *ctx_stack_top = (Context) {
    .desc = desc,
    .printed = false,
    .requested_context = is_requested,
    .prev = ctx_stack_ptr,
    .next = NULL,
  };

  ctx_stack_ptr = ctx_stack_top;

  return TRUE;
}

// Called at the end of a context block in "context_end"
bool _test_context_end(int line) {

  // If we're at the end of a context, we want to pop it off the stack if we
  // didn't actually run any tests in this pass. Otherwise, return false to
  // keep executing within this context.
  if (test_in_progress) {
    return FALSE;
  }

  // Sanity check - this generally shouldn't be possible to hit?
  //assert(test_current_line < line);
  //if (test_current_line >= line) {
  //  return FALSE;
  //}

  // Update to the next line, because the context begin and end statements
  // should actually be on the same line.
  //
  // This will usually make the line value go down (unless the context is
  // empty), which is ok because as long as it's above the context line
  // the entire block will be skipped.
  test_current_line = line + 1;

  // Once we pop a specifically requested context, end the tests.
  // If we're in verbose mode, we want to still go thorugh them all to print
  // the descriptions of un-run tests.
  if (ctx_stack_top->requested_context) {
    param_line = -1;
  }

  // Make sure we're not trying to pop the stack root
  assert(ctx_stack_top->prev != NULL);

  // Pop the context from the stack
  Context* tmp = ctx_stack_top;
  ctx_stack_top = tmp->prev;
  ctx_stack_top->next = NULL;
  free(tmp);

  return TRUE;
}

// Called between each test group, after all passes on a function are completed
static void context_clear_stack() {
  Context* ctx = ctx_stack_root.next;
  ctx_stack_root.next = NULL;

  while (ctx) {
    Context* tmp = ctx->next;
    free(ctx);
    ctx = tmp;
  }

  ctx_stack_ptr = &ctx_stack_root;
}

////////////////////////////////////////////////////////////////////////////////
// Output Printing/Formatting
////////////////////////////////////////////////////////////////////////////////

static int print_headers(
  int desc_color, PrintLevel desc_level, const StringRange* to_append
) {

  if (!test_filename_printed) {
    str_print_color(current_suite->header, CONCOL_bPurple);
    test_filename_printed = TRUE;
  }

  if (!test_function_printed) {
    StringBuilder stb = stb_pad(NULL, param_tabsize, ' ');
    stb_range(stb, R("in function ("));
    stb_str(stb, str_from_int(*test_function->line));
    stb_range(stb, test_function->header);
    String s = stb_resolve(&stb);
    str_print_color(s->range, CONCOL_bCyan);
    str_delete(&s);
    test_function_printed = TRUE;
  }

  Context* ctx = &ctx_stack_root;
  int level = 2;
  while (ctx->next) {
    ctx = ctx->next;
    if (!ctx->printed) {
      String s = str_prepend(ctx->desc, param_tabsize * level, ' ');
      str_print_color(s->range, CONCOL_Cyan);
      str_delete(&s);
      ctx->printed = TRUE;
    }
    level += 1;
  }

  if (test_desc_printed < desc_level) {
    String s;

    if (!test_in_progress) {
      s = str_prepend(R("Pre-test"), param_tabsize * level, ' ');
      str_print(s->range);
      test_desc_printed = PRINTED;

    } else {
      s = str_prepend(test_description, param_tabsize * level, ' ');
      if (to_append) {
        String t = str_concat(s->range, *to_append);
        str_delete(&s);
        s = t;
      }
      str_print_color(s->range, desc_color);
      test_desc_printed = desc_level;
    }

    str_delete(&s);
  }

  return level + 1;
}

void _test_log_fn(int line, const StringRange* message) {
  if (test_current_line && test_current_line >= line) {
    return;
  }

  if (param_verbose < V_NOTES) return;
  int mcmallocs = memory_count_mallocs;
  int mcfrees = memory_count_frees;
  int level = print_headers(CONCOL_bWhite, LOGGED, NULL);
  String s = str_prepend(*message, param_tabsize * level, ' ');
  str_print(s->range);
  str_delete(&s);
  memory_count_mallocs = mcmallocs;
  memory_count_frees = mcfrees;
}

void _test_warn_fn(int line, const StringRange* message) {
  if (test_current_line && test_current_line >= line) {
    return;
  }

  int mcmallocs = memory_count_mallocs;
  int mcfrees = memory_count_frees;
  int level = print_headers(CONCOL_Yellow, LOGGED, NULL);
  String s = str_prepend(*message, param_tabsize * level, ' ');
  str_print_color(s->range, CONCOL_Yellow);
  str_delete(&s);
  memory_count_mallocs = mcmallocs;
  memory_count_frees = mcfrees;
}

void _test_error_fn(const StringRange* message) {
  if (!test_expect_fail) {
    int mcmallocs = memory_count_mallocs;
    int mcfrees = memory_count_frees;
    int level = print_headers(CONCOL_Red, PRINTED, NULL);
    String s = str_prepend(*message, param_tabsize * level, ' ');
    str_print(s->range);
    str_delete(&s);
    memory_count_mallocs = mcmallocs;
    memory_count_frees = mcfrees;
  }
  test_failed = TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// Printing fo typed values
////////////////////////////////////////////////////////////////////////////////

static String resolve_param_(const StringRange* fmt, const StringRange* type_N, const void* N) {
  String formatted;

  // TODO: before converting, check for a user-defined conversion function that
  // either does the conversion, or returns a string literal with an alternate
  // type name to use instead (ie, "SDL_sint32" -> "int").
  // This function will always be responsible for deleting, make sure to inform
  // the user to "#undef malloc" before allocating memory for this.

  if (str_eq(*type_N, R("int"))) {
    formatted = str_from_int(*(int*)N);
  }
  else if (str_eq(*type_N, R("uint"))) {
    formatted = str_from_int((int)*(uint*)N); // should also make a uint one.
  }
  else if (str_eq(*type_N, R("size_t"))) {
    formatted = str_from_int((int)*(size_t*)N); // should make a long-int constructor
  }
  else if (str_eq(*type_N, R("float"))) {
    formatted = str_from_float(*(float*)N);
  }
  else if (str_eq(*type_N, R("_Bool"))) {
    formatted = str_from_bool(*(bool*)N);
  }
  else {
    String s = str_concat(R("Error formatting: Missing conversion info for type: "), *type_N);
    _test_error_fn(&s->range);
    str_delete(&s);
    return str_empty;
  }

  String result = str_concat(formatted->range, *fmt);
  str_delete(&formatted);
  return result;
}

void _test_error_typed(
  const StringRange* prefix, const StringRange* fmt,
  const void* A, const void* B,
  const StringRange* typ_A, const StringRange* typ_B
) {

  if ((fmt == NULL) || (typ_A && !A) || (typ_B && !B) || (B && !A)) {
    _test_error_fn(prefix);
    return;
  }

  Array split = str_split(*fmt, R("$"));

  array_insert(split, 0, prefix);

  String first = NULL;
  String second = NULL;

  if (split->size == 4) {
    second = resolve_param_(array_get_back(split), typ_B, B);
    array_pop_back(split);
  }

  if (split->size == 3) {
    first = resolve_param_(array_get_back(split), typ_A, A);
    array_pop_back(split);
  }

  if (first) array_push_back(split, &first->range);
  if (second) array_push_back(split, &second->range);

  String result = str_join(str_empty->range, split);
  str_delete(&first);
  str_delete(&second);
  _test_error_fn(&result->range);
  str_delete(&result);

  array_delete(&split);
}

////////////////////////////////////////////////////////////////////////////////
// Test Begin/End
////////////////////////////////////////////////////////////////////////////////

bool _test_begin(int line, StringRange desc) {

  // A test is currently in progress, just ignore this test for now
  if (test_in_progress) {
    return FALSE;
  }

  // Current line is past this, we've already run this test
  if (test_current_line >= line) {
    return FALSE;
  }

  test_current_line = line;
  test_description = desc;
  test_desc_printed = NOT_PRINTED;
  test_failed = FALSE;
  /* TODO: store memory 'end' pointer here to not count context allocations? */

  // At this point, normally we'rd run the test, but if we have a specific test
  //    number requested, we might still want to skip it.
  if (param_line == 0 || param_line == line) {
    test_in_progress = TRUE;
    test_skipped = FALSE;

  } else {
    test_skipped = TRUE; // not needed?

    if (param_verbose == V_VERY) {
      // Set test in progress temporarily just so it prints the title in blue
      test_in_progress = TRUE;
      print_headers(CONCOL_Blue, LOGGED, NULL);
    }
    test_in_progress = FALSE;
  }

  return test_in_progress;
}

bool _test_end() {
  if (!test_in_progress) {
    return FALSE;
  }

  // TODO: I don't think we need to check test_blank anymore (thanks to in_prog)
  if (!test_failed) {

    bool memory_safe = memory_count_mallocs == memory_count_frees;

    if (!memory_safe) {
      int malloc_count = memory_count_mallocs;
      int free_count = memory_count_frees;
      StringBuilder stb = stb_c_str(NULL, "mismatched malloc/free calls: ");
      stb_str(stb, str_from_int(malloc_count));
      stb_c_str(stb, "/");
      stb_str(stb, str_from_int(free_count));
      String s = stb_resolve(&stb);
      _test_error_fn(&s->range);
      str_delete(&s);
    }

    // memory_reset();

    /* TODO: do real memory checks for malloc and allocate garbage */
  }

  ++test_count;

  if (!test_failed ^ test_expect_fail) {
    ++test_passed_count;

    if (param_verbose >= V_RUN || param_line) {
      StringRange* failnote = test_expect_fail ? &R(" (failed successfully)") : NULL;
      print_headers(CONCOL_Green, LOGGED, failnote);
    }
  } else if (test_expect_fail) {
    test_expect_fail = FALSE; // clear this so it prints the error
    _test_error_fn(&R("Test was expected to fail, but succeeded instead"));
  }

  test_in_progress = FALSE;

  return TRUE;
}

bool _test_expect_to_fail() {
  unless(param_no_expect_fail)
    test_expect_fail = TRUE;
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// Test Runners
////////////////////////////////////////////////////////////////////////////////

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
  test_function = t;
}

static void before_pass() {
  ctx_stack_ptr = &ctx_stack_root;
  test_expect_fail = FALSE;
  memory_count_mallocs = 0;
  memory_count_frees = 0;
}

static void process_function(const TestGroup* t) {
  before_fn(t);
  test_current_line = 0;
  int prev_line;

  loop {
    before_pass();
    prev_line = test_current_line;
    t->group_fn();

    until (!test_in_progress && prev_line == test_current_line);

    _test_end();
  }

  context_clear_stack();
}

void test_run_suite(const TestSuite* suite) {
  before_suite(suite);

  if (!str_ends_with(suite->filename, param_file)) {
    if (param_verbose == V_VERY) {
      String msg = str_concat(R("skipping file: %c"), suite->filename);
      str_print_color(msg->range, CONCOL_Purple);
      str_delete(&msg);
    }
    return;
  }

  const TestGroup* t = &(*suite->test_groups)[0];
  while (t->line) {
    int tmp_line = param_line;
    if (*t->line == param_line) param_line = 0;
    process_function(t++);
    param_line = tmp_line;
  }

  current_suite = NULL;
}

static bool process_args(int argc, char* argv[]) {
  for (int i = 1; i < argc; ++i) {
    StringRange param = str_range(argv[i]);

    if (str_starts_with(param, R("-"))) {
      if (str_eq(param, R("-h")) || str_eq(param, R("--help"))) {
        print(": Usage: tests [OPTIONS]");
        print(":      : tests filename [OPTIONS]");
        print(":      : tests filename:line [OPTIONS]");
        print(":");
        print(": If filename is given, limits tests to that file. Matches end of name.");
        print(": If line is given, runs only that test, context, or group.");
        print(":");
        print(": - -- Options       Args");
        print(": h help                            : prints this message");
        print(": va                                : verbose output (maximum)");
        print(": v                                 : verbose output (prints all tests run)");
        print(": vn                                : verbose output (includes user notes)");
        print(": t tab-size         n (default 2)  : spaces per indent in test output");
        print(": f force-fails                     : disables 'expect(to_fail)', printing failure output");
        return TRUE;

      } else if (str_eq(param, R("-va"))) {
        param_verbose = V_VERY;

      } else if (str_eq(param, R("-v"))) {
        param_verbose = V_RUN;

      } else if (str_eq(param, R("-vn"))) {
        param_verbose = V_NOTES;

      } else if (str_eq(param, R("-f")) || str_eq(param, R("--force-fails"))) {
        param_no_expect_fail = TRUE;

      } else if (str_eq(param, R("-t")) || str_eq(param, R("--tab-size"))) {
        if (i + 1 < argc) {
          StringRange arg = str_range(argv[++i]);
          int as_i = atoi(arg.begin);
          param_tabsize = MAX(as_i, 0);
        } else {
          print("--tab-size requires a number as an argument");
          return TRUE;
        }
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
        param_file = str_substring(param, 0, sep);
      }
    }
  }

  return FALSE;
}

int _test_run_all(int count, TestSuite* suites[], int argc, char* argv[]) {
  if (process_args(argc, argv)) {
    return 0;
  }

  before_run();

  for (int i = 0; i < count; ++i) {
    test_run_suite(suites[i]);
  }

  if (test_count) {

    int color = test_count == test_passed_count ? CONCOL_bGreen : CONCOL_bRed;
    int ratio = (int)(100.f * (float)test_passed_count / (float)test_count);
    StringBuilder stb = stb_range(NULL, R("Tests passed: %c"));
    stb_str(stb, str_from_int(test_passed_count));
    stb_range(stb, R(" out of "));
    stb_str(stb, str_from_int(test_count));
    stb_range(stb, R(", or "));
    stb_str(stb, str_from_int(ratio));
    stb_range(stb, R("%"));
    String result = stb_resolve(&stb);
    str_print_color(result->range, color);
    str_delete(&result);
  } else {
    str_print_color(R("Tests passed: %c0 out of 0"), CONCOL_bYellow);
  }

  // return the number of failed tests
  return test_count - test_passed_count;
}
