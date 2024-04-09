#undef malloc
#undef realloc
#undef calloc
#undef free

#include "tst.h"
#include "types.h"

#include <stdlib.h>
#include <string.h>
#include <limits.h>

#ifndef __WASM__
#include <stdio.h>
#endif

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
static bool test_in_function = FALSE;
static bool test_in_progress = FALSE;
static bool test_expect_fail = FALSE;
static bool test_skip = FALSE;
static int test_current_line = 0;
static int test_count = 0;
static int test_passed_count = 0;

static Verbosity param_verbose = V_NONE;
static int param_line = 0;
static int param_tabsize = 2;
static StringRange param_file = M("");
static bool param_no_expect_fail = FALSE;
static bool param_memory_test = TRUE;

static int memory_count_mallocs = 0;
static int memory_count_frees = 0;

////////////////////////////////////////////////////////////////////////////////
// Memory Testing
////////////////////////////////////////////////////////////////////////////////

typedef enum MallocFailLevel {
  M_NORMAL,
  M_WAS_EXPECTED,
  M_FAIL_ONCE,
  M_FAIL_ALWAYS
} MallocFailLevel;

static void _test_error_mem(const StringRange* message);

#define memory_size_fence 7
//#define memory_size_max 4096 // defined in header for customizability

typedef struct MemoryRecord {
  size_t size;
  byte* block;
  bool is_free;
} MemoryRecord;

static byte memory[memory_size_max];
static size_t memory_ptr;

// Not using dynamic array here because, of course, it uses malloc!
static MemoryRecord* memory_records = NULL;
static size_t memory_records_capacity;
static size_t memory_records_size;
static bool memory_expect_error = FALSE;
static bool memory_error = FALSE;
static MallocFailLevel memory_malloc_fail = M_NORMAL;
static int memory_malloc_forced_failures = 0;
#define memory_records_grow_factor 1.5f

static bool memory_check_fence(MemoryRecord* record) {
  for (size_t i = 0; i < memory_size_fence; ++i) {
    if ('b' != *(record->block + i)
    ||  'e' != *(record->block + i + memory_size_fence + record->size)
    ) {
      return FALSE;
    }
  }
  return TRUE;
}

static int memory_record_compare(const void* key_, const void* dat) {
  const MemoryRecord* record = dat;
  const byte* key = key_;
  const byte* record_block = record->block + memory_size_fence;

  if (key > record_block) return 1;
  if (key < record_block) return -1;
  return 0;
}

static void memory_test_reset(bool enable) {
  if (!enable) {
    free(memory_records);
    memory_records = NULL;

  } else {
    memory_expect_error = FALSE;
    memory_malloc_forced_failures = 0;
    memory_malloc_fail = M_NORMAL;
    memory_error = FALSE;
    memory_count_mallocs = 0;
    memory_count_frees = 0;
    memory_records_size = 0;
    memory_ptr = 0;

    // Do a simple reset if we already have the records allocated
    if (!memory_records) {
      memory_records_size = 0;
      memory_records_capacity = 16;
      memory_records = malloc(memory_records_capacity * sizeof(MemoryRecord));
    }

    memset(memory, 'X', memory_size_max);
  }
}

static void memory_final_checks() {

  // Validate all memory records
  for (size_t i = 0; i < memory_records_size; ++i) {
    MemoryRecord* record = &memory_records[i];

    // Ensure all fences are in-tact
    if (!memory_check_fence(record)) {
      _test_error_mem(&R("after: detected buffer over/underrun"));
    }

    // Ensure memory hasn't been modified after free
    if (record->is_free) {
      byte* block = record->block + memory_size_fence;
      for (size_t j = 0; j < record->size; ++j) {
        if (block[j] != 'F') {
          _test_error_mem(&R("after: memory modified after free"));
        }
      }

    // Another check for freeing records
    } else {
      _test_error_mem(&R("after: allocated memory not freed"));
    }
  }

  // Ensure malloc/free parity
  if (memory_count_mallocs != memory_count_frees) {
    int malloc_count = memory_count_mallocs;
    int free_count = memory_count_frees;
    StringBuilder stb = stb_c_str(NULL,
      "after: mismatched malloc/free calls: ");
    stb_str(stb, str_from_int(malloc_count));
    stb_c_str(stb, " / ");
    stb_str(stb, str_from_int(free_count));
    String s = stb_resolve(&stb);
    _test_error_mem(&s->range);
    str_delete(&s);
  }

  // Ensure malloc was called if it was asked to fail
  if (memory_malloc_fail >= M_WAS_EXPECTED && !memory_malloc_forced_failures) {
    char err[] = "memory error: after: malloc fail requested, but never called";
    // causes regular error rather than memory error, since this is a failure
    // within the test design rather than memory actually breaking (ie, using
    // `expect(memory_error)` will not succeed if you forget to call malloc)
    _test_error_fn(&R(err));
  }
}

void* malloc_test(size_t size) {
  if (!memory_records || !test_in_function) {
    //++memory_count_mallocs;
    return malloc(size);
  }

  if (size == 0) {
    return NULL;
  }

  if (memory_malloc_fail >= M_FAIL_ONCE) {
    if (memory_malloc_fail == M_FAIL_ONCE) {
      memory_malloc_fail = M_WAS_EXPECTED;
    }
    ++memory_malloc_forced_failures;
    return NULL;
  }

  size_t next = memory_ptr + memory_size_fence*2 + size;

  if (next >= memory_size_max - memory_size_fence*2) {
    memory_expect_error = FALSE;
    _test_error_mem(&R(
      "malloc: ran out of test memory space! Increase limit from "
      STR(memory_size_max)" bytes.")
    );

    return NULL;
  }

  ++memory_count_mallocs;

  if (memory_records_size >= memory_records_capacity) {
    size_t new_cap = (size_t)((float)memory_records_capacity * memory_records_grow_factor);
    MemoryRecord* new_mem_rec = realloc(memory_records, new_cap * sizeof(MemoryRecord));
    if (!new_mem_rec) {
      memory_expect_error = FALSE;
      print("memory error: malloc: ran out of actual memory?");
      return NULL;
    }
    memory_records = new_mem_rec;
    memory_records_capacity = new_cap;
  }

  MemoryRecord* record = &memory_records[memory_records_size++];

  if (memory_ptr != 0) {
    size_t fence = memory_ptr - memory_size_fence;
    for (; fence < memory_ptr; ++fence) {
      if (memory[fence] != 'e') {
        _test_error_mem(&R("malloc: preceeding fence broken"));
        return NULL;
      }
    }
  }

  record->size = size;
  record->block = memory + memory_ptr;
  record->is_free = FALSE;
  memset(record->block, 'b', memory_size_fence);
  memset(record->block + memory_size_fence, 'N', size);
  memset(record->block + memory_size_fence + size, 'e', memory_size_fence);

  memory_ptr = next;

  return record->block + memory_size_fence;
}

void free_test(void* mem_) {
  byte* mem = mem_;

  if (!memory_records || !test_in_function) {
    //++memory_count_frees;
    free(mem);
    return;
  }

  // free(NULL) is a NOP
  if (mem == NULL)
    return;

  // check for memory outside of our bounds
  if (mem < memory || mem >= memory + memory_size_max) {
    StringRange err = R("free: invalid pointer, out of bounds");
    _test_error_mem(&err);
    return;
  }

  // check if the pointer is in our allocated pointers list
  MemoryRecord* record = bsearch(
    mem, memory_records,
    memory_records_size, sizeof(MemoryRecord),
    memory_record_compare
  );

  if (record == NULL) {
    StringRange err = R("free: invalid pointer, not malloc result");
    _test_error_mem(&err);
    return;
  }

  // check for double-free
  if (record->is_free) {
    _test_error_mem(&R("free: pointer already freed"));
  }

  // check fences
  if (!memory_check_fence(record)) {
    _test_error_mem(&R("free: broken fence"));
  }

  // free the memory
  memset(record->block + memory_size_fence, 'F', record->size);
  record->is_free = TRUE;
  ++memory_count_frees;
}

void* calloc_test(size_t ct, size_t sel) {
  if (!memory_records || !test_in_function) {
    return calloc(ct, sel);
  }

  byte* ret = malloc_test(ct * sel);
  if (!ret) return NULL;

  memset(ret, 0, ct * sel);
  return ret;
}

void* realloc_test(void* mem, size_t nsize) {
  if (!memory_records || !test_in_function) {
    //if (mem == NULL) ++memory_count_mallocs;
    return realloc(mem, nsize);
  }

  if (mem == NULL) {
    return malloc_test(nsize);
  }

  // you can realloc the last block, but that's it
  if (memory_records_size) {
    MemoryRecord* record = &memory_records[memory_records_size - 1];

    if (memory_malloc_fail >= M_FAIL_ONCE) {
      if (memory_malloc_fail == M_FAIL_ONCE) {
        memory_malloc_fail = M_WAS_EXPECTED;
      }
      ++memory_malloc_forced_failures;
      return NULL;
    }

    if (record->block + memory_size_fence == mem) {

      if (!memory_check_fence(record)) {
        _test_error_mem(&R("realloc: broken fence"));
        return NULL;
      }

      byte* block_start = record->block + memory_size_fence;
      memset(block_start + nsize, 'e', memory_size_fence);
      memset(block_start + record->size, 'N', nsize - record->size);

      record->size = nsize;
      memory_ptr = block_start + record->size + memory_size_fence - memory;

      return record->block + memory_size_fence;

    } else {
      void* ret = malloc_test(nsize);
      if (!ret) {
        _test_error_mem(&R("realloc: malloc failed in realloc"));
        return ret;
      }

      memcpy(ret, record->block, record->size + memory_size_fence * 2);
      free_test(record->block + memory_size_fence);
      return ret;
    }
  }

  _test_error_mem(&R("realloc: nothing previously allocated"));
  return malloc_test(nsize);
}

////////////////////////////////////////////////////////////////////////////////
// Test Context
////////////////////////////////////////////////////////////////////////////////
// A test context allows pre-test setup to be shared between multiple tests.
// Variables can be created and accessed within the tests, and other setup can
// be performed before running the tests. After each test, the test group
// function is exited and re-entered, meaning the context is recreated for every
// test (ie, incrementing a shared value in one test will not affect the next
// test), and after the context is passed, the setup won't be run again for any
// tests that follow it.

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
  ctx_stack_top = &ctx_stack_root;
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
      s = str_prepend(R("pre-test"), param_tabsize * level, ' ');
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

  bool mem_test_temp = test_in_function;
  test_in_function = FALSE;

  int mcmallocs = memory_count_mallocs;
  int mcfrees = memory_count_frees;
  int level = print_headers(CONCOL_bWhite, LOGGED, NULL);
  String s = str_prepend(*message, param_tabsize * level, ' ');
  str_print(s->range);
  str_delete(&s);
  memory_count_mallocs = mcmallocs;
  memory_count_frees = mcfrees;

  test_in_function = mem_test_temp;
}

void _test_warn_fn(int line, const StringRange* message) {
  if (test_current_line && test_current_line >= line) {
    return;
  }

  bool mem_test_temp = test_in_function;
  test_in_function = FALSE;

  int mcmallocs = memory_count_mallocs;
  int mcfrees = memory_count_frees;
  int level = print_headers(CONCOL_Yellow, LOGGED, NULL);
  String s = str_prepend(*message, param_tabsize * level, ' ');
  str_print_color(s->range, CONCOL_Yellow);
  str_delete(&s);
  memory_count_mallocs = mcmallocs;
  memory_count_frees = mcfrees;

  test_in_function = mem_test_temp;
}

static void _test_error_no_fail(const StringRange* message, bool is_mem_err) {
  bool mem_test_temp = test_in_function;
  test_in_function = FALSE;

  int mcmallocs = memory_count_mallocs;
  int mcfrees = memory_count_frees;
  int level = print_headers(CONCOL_Red, PRINTED, NULL);
  String t = NULL;
  if (is_mem_err) {
    t = str_concat(R("memory error: "), *message);
    message = &t->range;
  }
  String s = str_prepend(*message, param_tabsize * level, ' ');
  str_print(s->range);
  str_delete(&s);
  str_delete(&t);
  memory_count_mallocs = mcmallocs;
  memory_count_frees = mcfrees;

  test_in_function = mem_test_temp;
}

void _test_error_fn(const StringRange* message) {
  if (!test_expect_fail) {
    _test_error_no_fail(message, FALSE);
  }
  test_failed = TRUE;
}

static void _test_error_mem(const StringRange* message) {
  if (!memory_expect_error) {
    _test_error_no_fail(message, TRUE);
  }
  memory_error = TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// Printing fo typed values
////////////////////////////////////////////////////////////////////////////////

static String resolve_param_(
  const StringRange* fmt, const StringRange* type_N, const void* N
) {
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
    String s = str_concat(R("error formatting: Missing conversion info for type: "), *type_N);
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

  bool mem_test_temp = test_in_function;
  test_in_function = FALSE;

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

  test_in_function = mem_test_temp;
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

  // At this point, normally we'rd run the test, but if we have a specific test
  //    number requested, we might still want to skip it.
  if ((param_line == 0 || param_line == line) && !test_skip) {
    test_in_progress = TRUE;

  } else {

    if (param_verbose == V_VERY || test_skip) {
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
    memory_final_checks();
  }

  ++test_count;

  if (!test_failed ^ test_expect_fail 
  && !memory_error ^ memory_expect_error
  ) {
    ++test_passed_count;

    if (param_verbose >= V_RUN || param_line) {
      bool failed = test_expect_fail || memory_expect_error;
      StringRange* failnote = failed ? &R(" (failed successfully)") : NULL;
      print_headers(CONCOL_Green, LOGGED, failnote);
    }
  } else {
    if (test_expect_fail) {
      test_expect_fail = FALSE; // clear this so it prints the error
      _test_error_fn(&R("expected to fail, but succeeded instead"));
    }
    if (memory_expect_error) {
      _test_error_fn(&R("expected memory errors, but none were found"));
    }
  }

  test_in_progress = FALSE;

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// Directives
////////////////////////////////////////////////////////////////////////////////

bool _test_expect_to_fail() {
  unless(param_no_expect_fail)
    test_expect_fail = TRUE;
  return TRUE;
}

static bool memory_directive_warning() {
  if (!param_memory_test) {
    _test_warn_fn(INT_MAX, &R(
      "warning: expecting memory errors, but memory testing is disabled"
    ));
    test_expect_fail = TRUE;
    return TRUE;
  }
  return FALSE;
}

bool _test_memory_expect_to_fail() {
  if (memory_directive_warning()) {
    test_skip = TRUE;
    return !test_in_progress;
  } else unless(param_no_expect_fail)
    memory_expect_error = TRUE;
  return TRUE;
}

bool _test_memory_malloc_null(bool only_once) {
  if (memory_directive_warning()) {
    test_skip = TRUE;
    return !test_in_progress;
  } else
    memory_malloc_fail = only_once ? M_FAIL_ONCE : M_FAIL_ALWAYS;
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
  test_skip = FALSE;
  memory_test_reset(param_memory_test);
}

static void process_function(const TestGroup* t) {
  before_fn(t);
  test_current_line = 0;
  int prev_line;

  loop {
    before_pass();
    prev_line = test_current_line;

    test_in_function = TRUE;
    t->group_fn();
    test_in_function = FALSE;

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
        print(": m ignore-memory                   : disables memory testing");
        return TRUE;

      } else if (str_eq(param, R("-va"))) {
        param_verbose = V_VERY;

      } else if (str_eq(param, R("-v"))) {
        param_verbose = V_RUN;

      } else if (str_eq(param, R("-vn"))) {
        param_verbose = V_NOTES;

      } else if (str_eq(param, R("-f")) || str_eq(param, R("--force-fails"))) {
        param_no_expect_fail = TRUE;

      } else if (str_eq(param, R("-m")) || str_eq(param, R("--ignore-memory"))) {
        param_memory_test = FALSE;

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
