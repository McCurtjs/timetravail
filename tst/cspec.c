#ifdef malloc
# define _CSPEC_USE_MEMORY_TESTING_
# undef malloc
# undef realloc
# undef calloc
# undef free
#endif

#include "cspec.h"
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

////////////////////////////////////////////////////////////////////////////////
// String Handling
////////////////////////////////////////////////////////////////////////////////
// To make this work as a "single-header" include as well as to make sure it
// works on WASM varianets with no libc, all our string handling for output
// should be done in a static space to avoid the need for malloc/free.
#define output_size 500
#define output_float_precision 5
static char output_buffer[output_size + 1];
static uint output_index = 0;
static const char* output_fmt = NULL;

static bool cspec_strcmp(const char* A, const char* B) {
  if (!A || !B) return FALSE;

  while (*A && *B) {
    if (*A++ != *B++) return FALSE;
  }

  return *A == *B;
}

static void output_continue_format();

static void output_str(const char* s) {
  if (!s) return;
  char prev = '\0';
  while (*s && output_index < output_size) {

    // handle case for {} format specifiers
    if (!output_fmt && *s == '{') {
      if (prev == '{') {
        ++s;
        prev = '\0';
        continue;
      } else if (*(s + 1) == '}') {
        output_fmt = s + 2;
        output_buffer[output_index] = '\0';
        return;
      }
    }

    // insert Unix-style color indicators for %c if we're not in WASM
#ifndef __WASM__
    char color_indicator[] = "\033[_;3_m";
    if (*s == '%' && *(s + 1) == 'c'
    && output_index < output_size - sizeof(color_indicator)
    ) {
      for (uint i = 0; i < sizeof(color_indicator) - 1; ++i) {
        output_buffer[output_index++] = color_indicator[i];
      }
      s += 2;
      continue;
    }
#endif

    prev = *(s++);
    output_buffer[output_index++] = prev;
  }
  output_continue_format();
}

static void output_continue_format() {
  if (output_fmt) {
    const char* tmp = output_fmt;
    output_fmt = NULL;
    output_str(tmp);
  }
  output_buffer[output_index] = '\0';
}

static void output_char(char c) {
  if (output_index >= output_size) return;
  output_buffer[output_index++] = c;
  output_continue_format();
}

static void output_pad(uint until_pos, char c) {
  if (until_pos > output_size) until_pos = output_size;
  while (output_index < until_pos) {
    output_buffer[output_index++] = c;
  }
  output_continue_format();
}

static void _output_uint_ignore_format(unsigned long int i) {
  if (i == 0) {
    output_buffer[output_index++] = '0';
    return;
  }
  uint start = output_index;
  while (i) {
    output_buffer[output_index++] = '0' + (i % 10);
    i /= 10;
  }
  // the above prints it backwards, so flip it
  uint end = output_index - 1;
  while (start < end) {
    char temp = output_buffer[start];
    output_buffer[start++] = output_buffer[end];
    output_buffer[end--] = temp;
  }
}

static void output_uint(unsigned long int i) {
  _output_uint_ignore_format(i);
  output_continue_format();
}

static void output_sint(long int i) {
  if (i < 0) {
    output_buffer[output_index++] = '-';
    i *= -1;
  }
  output_uint((unsigned long int)i);
}

static void output_float_p(double f, int precision) {
  if (f < 0.0) {
    output_buffer[output_index++] = '-';
    f *= -1.0;
  }
  unsigned long int integer_part = (unsigned long int)f;
  _output_uint_ignore_format(integer_part);
  f -= integer_part;
  if (f == 0.0) return;
  output_buffer[output_index++] = '.';
  for (int i = precision; i && f >= 0.00000000001; --i) {
    f *= 10.0;
    integer_part = (unsigned long int)f;
    output_buffer[output_index++] = '0' + (char)integer_part;
    f -= integer_part;
  }
  output_continue_format();
}

static void output_float(double f) {
  output_float_p(f, output_float_precision);
}

static void output_bool(bool b) {
  output_str(b ? "true" : "false");
}

static void output_reset() {
  output_index = 0;
  output_buffer[0] = '\0';
  output_fmt = NULL;
}

static void output(const char* s) {
#ifdef __WASM__
  str_print(str_range(s));
#else
  printf("%s\n", s);
#endif
}

static void output_print() {
  if (output_fmt) output_str(output_fmt);

#ifdef __WASM__
  str_print(str_range(output_buffer));
#else
  printf("%s\n", output_buffer);
#endif

  output_reset();
}

static void output_print_color(ConsoleColor color) {
  // flush any remaining format string
  if (output_fmt) output_str(output_fmt);

#ifndef __WASM__
  // find the color specifier if it was added into the string
  for (uint i = 0; i < output_index; ++i) {
    if (output_buffer[i] == '\033') {
      // set boldness flag
      output_buffer[i + 2] = color >= 40 ? '1' : '0';

      // fill out the color code being requested
      output_buffer[i + 5] = '0' + color % 10;

      // cap the string with a closing color specifier
      output_str("\033[0m");

      break;
    }
  }
#endif

  // finally print the string
#ifdef __WASM__
  str_print_color(str_range(output_buffer), color);
#else
  printf("%s\n", output_buffer);
#endif

  output_reset();
}

////////////////////////////////////////////////////////////////////////////////
// Memory Testing
////////////////////////////////////////////////////////////////////////////////

#ifdef _CSPEC_USE_MEMORY_TESTING_

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
static int memory_count_mallocs = 0;
static int memory_count_frees = 0;
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
    int mallocs = memory_count_mallocs;
    int frees = memory_count_frees;
    StringBuilder stb = stb_c_str(NULL,
      "after: mismatched malloc/free calls: ");
    stb_str(stb, str_from_int(mallocs));
    stb_c_str(stb, " / ");
    stb_str(stb, str_from_int(frees));
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

void* cspec_malloc(size_t size) {
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

void cspec_free(void* mem_) {
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

void* cspec_calloc(size_t ct, size_t sel) {
  if (!memory_records || !test_in_function) {
    return calloc(ct, sel);
  }

  byte* ret = cspec_malloc(ct * sel);
  if (!ret) return NULL;

  memset(ret, 0, ct * sel);
  return ret;
}

void* cspec_realloc(void* mem, size_t nsize) {
  if (!memory_records || !test_in_function) {
    //if (mem == NULL) ++memory_count_mallocs;
    return realloc(mem, nsize);
  }

  if (mem == NULL) {
    return cspec_malloc(nsize);
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
      void* ret = cspec_malloc(nsize);
      if (!ret) {
        _test_error_mem(&R("realloc: malloc failed in realloc"));
        return ret;
      }

      memcpy(ret, record->block, record->size + memory_size_fence * 2);
      cspec_free(record->block + memory_size_fence);
      return ret;
    }
  }

  _test_error_mem(&R("realloc: nothing previously allocated"));
  return cspec_malloc(nsize);
}

#else

static void memory_test_reset(bool enable) {
  (void)enable;
}

static void memory_final_checks() { }

#endif

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
    output_str(current_suite->header.begin);
    output_print_color(CONCOL_bPurple);
    test_filename_printed = TRUE;
  }

  if (!test_function_printed) {
    output_pad(param_tabsize, ' ');
    output_str("in function (");
    output_sint(*test_function->line);
    output_str(test_function->header.begin);
    output_print_color(CONCOL_bCyan);
    test_function_printed = TRUE;
  }

  Context* ctx = &ctx_stack_root;
  int level = 2;
  while (ctx->next) {
    ctx = ctx->next;
    if (!ctx->printed) {
      output_pad(param_tabsize * level, ' ');
      output_str(ctx->desc.begin);
      output_print_color(CONCOL_Cyan);
      ctx->printed = TRUE;
    }
    level += 1;
  }

  if (test_desc_printed < desc_level) {
    output_pad(param_tabsize * level, ' ');

    if (!test_in_progress) {
      output_str("pre-test");
      output_print();
      test_desc_printed = PRINTED;

    } else {
      output_str(test_description.begin);
      output_str(to_append ? to_append->begin : NULL); // may be null
      output_print_color(desc_color);
      test_desc_printed = desc_level;
    }
  }

  return level + 1;
}

void _test_log_fn(int line, const StringRange* message) {
  if ((test_current_line && test_current_line >= line)
  || param_verbose < V_NOTES
  ) {
    return;
  }
  int level = print_headers(CONCOL_bWhite, LOGGED, NULL);
  output_pad(param_tabsize * level, ' ');
  output_str(message->begin);
  output_print();
}

void _test_warn_fn(int line, const StringRange* message) {
  if (test_current_line && test_current_line >= line) {
    return;
  }
  int level = print_headers(CONCOL_Yellow, LOGGED, NULL);
  output_pad(param_tabsize * level, ' ');
  output_str(message->begin);
  output_print_color(CONCOL_Yellow);
}

static void test_error_no_fail(const StringRange* message, bool is_mem_err) {
  int level = print_headers(CONCOL_Red, PRINTED, NULL);
  output_pad(param_tabsize * level, ' ');
  if (is_mem_err) output_str("Memory error: ");
  output_str(message->begin);
  output_print();
}

void _test_error_fn(const StringRange* message) {
  if (test_in_progress) {
    if (!test_expect_fail) {
      test_error_no_fail(message, FALSE);
    }
    test_failed = TRUE;
  }
}

#ifdef _CSPEC_USE_MEMORY_TESTING_

static void _test_error_mem(const StringRange* message) {
  if (test_in_progress) {
    if (!memory_expect_error) {
      test_error_no_fail(message, TRUE);
    }
    memory_error = TRUE;
  }
}

#endif

////////////////////////////////////////////////////////////////////////////////
// Printing fo typed values
////////////////////////////////////////////////////////////////////////////////

resolve_user_types_fn resolve_user_types = NULL;

static void resolve_param(const char* typ_N, const void* N) {

  if (resolve_user_types) {
    uint written = resolve_user_types(&typ_N, N,
      output_buffer + output_index, output_size - output_index
    );

    if (written) {
      output_index += written;
      output_buffer[output_index] = '\0';
      return;
    }
  }

  if (cspec_strcmp(typ_N, "int")) {
    output_sint(*(int*)N);
  }
  else if
  (  cspec_strcmp(typ_N, "short")
  || cspec_strcmp(typ_N, "short int")
  ) {
    output_sint(*(short int*)N);
  }
  else if
  (  cspec_strcmp(typ_N, "long")
  || cspec_strcmp(typ_N, "long int")
  ) {
    output_sint(*(long int*)N);
  }
  else if
  (  cspec_strcmp(typ_N, "uint")
  || cspec_strcmp(typ_N, "unsigned")
  || cspec_strcmp(typ_N, "unsigned int")
  ) {
    output_uint(*(unsigned int*)N);
  }
  else if
  (  cspec_strcmp(typ_N, "ushort")
  || cspec_strcmp(typ_N, "unsigned short")
  ) {
    output_uint(*(unsigned short*)N);
  }
  else if
  (  cspec_strcmp(typ_N, "unsigned long")
  || cspec_strcmp(typ_N, "unsigned long int")
  || cspec_strcmp(typ_N, "size_t")
  ) {
    output_uint(*(unsigned long int*)N);
  }
  else if (cspec_strcmp(typ_N, "float")) {
    output_float(*(float*)N);
  }
  else if (cspec_strcmp(typ_N, "double")) {
    output_float(*(double*)N);
  }
  else if
  (  cspec_strcmp(typ_N, "_Bool")
  || cspec_strcmp(typ_N, "bool")
  ) {
    output_bool(*(bool*)N);
  }
  else if
  (  cspec_strcmp(typ_N, "char")
  || cspec_strcmp(typ_N, "unsigned char")
  || cspec_strcmp(typ_N, "byte")
  ) {
    output_char(*(char*)N);
  }
}

void _test_error_typed(
  const StringRange* prefix, const StringRange* fmt,
  const void* A, const void* B,
  const StringRange* typ_A, const StringRange* typ_B
) {

  if (!test_in_progress) {
    return;
  }

  test_failed = TRUE;

  if (test_expect_fail) {
    return;
  }

  if ((fmt == NULL) || (typ_A && !A) || (typ_B && !B) || (B && !A)) {
    _test_error_fn(prefix);
    return;
  }

  int level = print_headers(CONCOL_Red, PRINTED, NULL);
  output_pad(param_tabsize * level, ' ');
  output_str(prefix->begin);
  output_str(fmt->begin);

  if (A && typ_A) resolve_param(typ_A->begin, A);
  if (B && typ_B) resolve_param(typ_B->begin, B);

  output_print();
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
#ifdef _CSPEC_USE_MEMORY_TESTING_
  && !memory_error ^ memory_expect_error
#endif
  ) {
    ++test_passed_count;

    if (param_verbose >= V_RUN || param_line) {
      bool failed = test_expect_fail;
#ifdef _CSPEC_USE_MEMORY_TESTING_
      failed |= memory_expect_error;
#endif
      StringRange* failnote = failed ? &R(" (failed successfully)") : NULL;
      print_headers(CONCOL_Green, LOGGED, failnote);
    }
  } else {
    if (test_expect_fail) {
      test_expect_fail = FALSE; // clear this so it prints the error
      _test_error_fn(&R("expected to fail, but succeeded instead"));
    }
#ifdef _CSPEC_USE_MEMORY_TESTING_
    if (memory_expect_error) {
      _test_error_fn(&R("expected memory errors, but none were found"));
    }
#endif
  }

  test_in_progress = FALSE;

  return TRUE;
}

bool _test_active() {
  return test_in_progress;
}

////////////////////////////////////////////////////////////////////////////////
// Directives
////////////////////////////////////////////////////////////////////////////////

bool _test_expect_to_fail() {
  unless(param_no_expect_fail)
    test_expect_fail = TRUE;
  return TRUE;
}

#ifdef _CSPEC_USE_MEMORY_TESTING_
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
#endif

bool _test_memory_expect_to_fail() {
#ifdef _CSPEC_USE_MEMORY_TESTING_
  if (memory_directive_warning()) {
    test_skip = TRUE;
    return !test_in_progress;
  } else unless(param_no_expect_fail)
    memory_expect_error = TRUE;
  return TRUE;
#else
  _test_error_fn(&R("Expected memory failure, but memory testing is disabled"));
  return TRUE;
#endif
}

bool _test_memory_malloc_null(bool only_once) {
#ifdef _CSPEC_USE_MEMORY_TESTING_
  if (memory_directive_warning()) {
    test_skip = TRUE;
    return !test_in_progress;
  } else
    memory_malloc_fail = only_once ? M_FAIL_ONCE : M_FAIL_ALWAYS;
  return TRUE;
#else
  (void)only_once;
  _test_error_fn(&R("Requesting failed malloc, but memory testing is disabled"));
  return TRUE;
#endif
}

int _test_memory_malloc_count() {
#ifdef _CSPEC_USE_MEMORY_TESTING_
  if (memory_directive_warning()) {
    test_skip = TRUE;
    return -1;
  }
  return memory_count_mallocs;
#else
  _test_error_fn(&R("Reading malloc counts, but memory testing is disabled"));
  return -1;
#endif
}

int _test_memory_free_count() {
#ifdef _CSPEC_USE_MEMORY_TESTING_
  if (memory_directive_warning()) {
    test_skip = TRUE;
    return -1;
  }
  return memory_count_frees;
#else
  _test_error_fn(&R("Reading free counts, but memory testing is disabled"));
  return -1;
#endif
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
      output_str("skipping file: %c");
      output_str(suite->filename.begin);
      output_print_color(CONCOL_Purple);
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
    char* arg = argv[i];

    if (arg[0] == '-') {
      if (cspec_strcmp(arg, "-h") || cspec_strcmp(arg, "--help")) {
        output(
            ": Usage: tests [OPTIONS]"
          "\n:      : tests filename [OPTIONS]"
          "\n:      : tests filename:line [OPTIONS]"
          "\n:"
          "\n: If filename is given, limits tests to that file. Matches end of name."
          "\n: If line is given, runs only that test, context, or group."
          "\n:"
          "\n: - -- Options       Args"
          "\n: h help                            : prints this message"
          "\n: va                                : verbose output (maximum)"
          "\n: v                                 : verbose output (prints all tests run)"
          "\n: vn                                : verbose output (includes user notes)"
          "\n: t tab-size         n (default 2)  : spaces per indent in test output"
          "\n: f force-fails                     : disables 'expect(to_fail)', printing failure output"
          "\n: m ignore-memory                   : disables memory testing"
        );
        return TRUE;

      } else if (cspec_strcmp(arg, "-va")) {
        param_verbose = V_VERY;

      } else if (cspec_strcmp(arg, "-v")) {
        param_verbose = V_RUN;

      } else if (cspec_strcmp(arg, "-vn")) {
        param_verbose = V_NOTES;
      }
      else if
      (  cspec_strcmp(arg, "-f")
      || cspec_strcmp(arg, "--force-fails")
      ) {
        param_no_expect_fail = TRUE;

      } else if
      (  cspec_strcmp(arg, "-m")
      || cspec_strcmp(arg, "--ignore-memory")
      ) {
        param_memory_test = FALSE;

      } else if
      (  cspec_strcmp(arg, "-t")
      || cspec_strcmp(arg, "--tab-size")
      ) {
        if (i + 1 < argc) {
          char* param = argv[++i];
          int as_i = atoi(param);
          param_tabsize = MAX(as_i, 0);
        } else {
          output("--tab-size requires a number as an argument");
          return TRUE;
        }
      }
    } else {
      // Can't use str_split or other functions that allocate here or it'll
      // pollute the test allocator! Use only in-place functions from str.h.
      char* s = arg;
      while (*s) {
        if (*s == ':') {
          *(s++) = '\0';
          param_line = atoi(s);
          break;
        }
        ++s;
      }

      // Zero-length, don't bother. In this case, the string was entered as ":3"
      // so we'll take the number, but not single it to a file. Maybe someone
      // meticulously puts a specific test on one line of every file, who knows.
      if (arg[0] != '\0') {
        param_file = str_range(arg);
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
    output_str("Tests passed: %c{} out of {}, or {}%");
    output_sint(test_passed_count);
    output_sint(test_count);
    output_sint((int)(100.f * (float)test_passed_count / (float)test_count));
    output_print_color(test_count == test_passed_count ? CONCOL_bGreen : CONCOL_bRed);
  } else {
    output_str("Tests passed: %c0 out of 0");
    output_print_color(CONCOL_bYellow);
  }

  // return the number of failed tests
  return test_count - test_passed_count;
}
