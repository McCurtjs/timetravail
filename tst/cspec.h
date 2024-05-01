#ifndef _CSPEC_H_
#define _CSPEC_H_

typedef _Bool csBool;
typedef unsigned int csUint;
typedef unsigned char csByte;

typedef void (*test_fn)(void);

typedef struct TestGroup {
  const int* line;
  const char* header;
  test_fn group_fn;
} TestGroup;

typedef struct TestSuite {
  const char* header;
  const char* filename;
  TestGroup (*test_groups)[];
} TestSuite;

// This is a custom test library based roughly on the RSpec testing library
//    for Ruby: https://rspec.info/features/3-12/rspec-core/
//
// RSpec "is a behavior-driven development (BDD) framework" which is something I
//    wanted to try and emulate in C before realizing it's been done before...
//    That said, it's been an interesting excuse to see what I could get away
//    with using macros, and the result seems to be surprisingly functional and
//    closer to what Ruby has than the other projects I've found attempting the
//    same thing.
//
// Example of basic setup:
//
//    file: /src/engine/widget.c
//
//        int widget_operate(void)
//        {
//            // your very critically important source code
//        }
//
//    file: /test/engine/test_widget.c
//
//        describe(widget_operate)
//        {
//            it ("returns 0 after operating the widget")
//            {
//                int result = widget_operate();
//                expect(result == 0);
//            }
//        }
//
//        create_test_suite(widget_tests)
//        {
//            test_group(widget_operate),
//            test_suite_end
//        };
//
//    file: /test/test_main.c
//
//        int main(int argc, char* argv[])
//        {
//            TestSuite* suites[] = {
//                &widget_tests
//            };
//
//            return test_run_all(suites);
//        }
//

#ifndef memory_size_max
// \brief Test scratch-size for memory testing with malloc.
//
// \brief Note: This does not account for space for fences between allocations,
//    the actual available space you can allocate will be lower.
#define memory_size_max 4096
#endif

////////////////////////////////////////////////////////////////////////////////
// Test setup
////////////////////////////////////////////////////////////////////////////////

// \brief Describes an example group containing tests that explain how
//    the function being tested should behave in various contexts.
//
// \brief Under the hood, an exmaple group is simply a function. The group
//    is executed once for every included test rather than in a single
//    iteration, meaning any function-scope changes in execution context will
//    not be preserved between example units.
//
// \param NAME - NOT A STRING - The name of the test example group being
//    described. This name will later need to be included in a test suite in
//    order to be executed. (see `test_group` for info)
#define describe(NAME)            _describe(NAME)
#define test_func(NAME)           _describe(NAME)

// \brief An `it` block declares an example case for testing.
//
// \brief Each "it" statement is run one at a time in its own execution context
//    that won't impact the result of other examples.
//
// \param DESC - String Literal: a brief description of the test that will be
//    printed with the test results.
#define it(DESC)                  _test("it "DESC)
#define test(DESC)                _test(DESC)

// \brief
#define after                     _after

////////////////////////////////////////////////////////////////////////////////
// Composing test suites
////////////////////////////////////////////////////////////////////////////////

// \brief A test suite is a batch of test groups to be executed together as a
//    conceptual object.
//
// \brief Conceptually, while a test group might describe the behaviors of a
//    given function related to a particular class, the test suite will contain
//    the set of descriptions for all functions that make up that class.
//
// \brief Use this to begin a declarative block at the end of a test file. The
//    block should contain only `test_group` calls and end with `test_suite_end`
//
// \param NAME - NOT A STRING - The name of the test suite. This will later need
//    to be given to the test_run functions to execute the tests.
#define test_suite(NAME)          _test_suite(NAME)

// \brief Used within the block of test_suite_begin to add groups to the suite.
//
// \brief Example usage:
// \brief     test_suite_begin(widget_class_tests) {
// \brief         test_group(widget_tests),
// \brief         test_suite_end
// \brief     };
//
// \param TEST_FN - NOT A STRING - The name of the test group to include in the
//    suite. This should be the same value passed into `describe` above.
#define test_group(TEST_FN)       _test_group(TEST_FN)

// \brief Must be included at the end of the test suite declaration list.
//    (thinking of a way to avoid needing this)
#define test_suite_end            _test_suite_end

void test_run_suite(const TestSuite* suite);
#define test_run_all(suites)      _test_run_all_suites(suites)

////////////////////////////////////////////////////////////////////////////////
// Contexts
////////////////////////////////////////////////////////////////////////////////

// \brief Opens a descriptive context block that can contain other example
//    statements or contexts.
//
// \brief Variables can be defined in a context to be shared between between
//    multiple tests, or functions can be called to share pre-test setups.
//
// \brief Contexts can be nested. Statements in contexts will be executed in
//    order - function calls will be made in the order they appear, and any
//    variables defined in an earlier context that are changed in later ones
//    will use the last set value in the test.
//
// \param DESC - String Literal: a brief description of the context that
//    applies to all included tests and will be printed along with their output.
#define context(DESC)             _context(DESC)

////////////////////////////////////////////////////////////////////////////////
// Logging
////////////////////////////////////////////////////////////////////////////////

// \brief Logs a baisc message in the console output. The message will only be
//    printed once between all runs of the test group.
//
// \brief This level of non-critical log will not be printed unless the verbose
//    flag is set to some level (using -vn, -v, or -va)
//
// \param message - String Literal: The message to be printed. Fairly limited
//    currently in that it can only print compile-time c-string literals, but
//    somewhat useful for debugging with conditional statements. Would like
//    to replace with a version that can do dynamic strings.
#define test_log(message)         _test_log(message)

// \brief An alias for `test_log`
#define test_note(message)        _test_log(message)

// \brief Logs a warning message in the console output. The message is of higher
//    importance than a basic log, and will appear even if the verbose level is
//    not set.
//
// \param warning - String Literal: The warning to be printed. Same
//    restrictions as with test_log.
#define test_warn(warning)        _test_warn(warning)

// \brief Automatically fails the test. Do not pass GO. Do not collect $200.
//
// \param issue - String Literal: The error to print.
#define test_fail(issue)          _test_fail(issue)

// \brief Prints a block of test memory for debugging purposes.
//
// \param ptr - Any pointer within the walled garden test memory when testing
//    is enabled. If given the exact return value from a malloc (cspec_malloc)
//    call, will print the whole allocated segment. Any other pointer will print
//    just three rows of 16 bytes (starting 16 before the requested pointer).
#define test_log_memory(ptr)      _test_memory_log_block(__LINE__, ptr);

////////////////////////////////////////////////////////////////////////////////
// Value checking with "Expect"
////////////////////////////////////////////////////////////////////////////////

// \brief An `expect` clause within a test is used to check the validity of
//    output for the operations being tested. The value or expression passed is
//    expected to evaluate to TRUE, otherwise, the test aborts as a failure.
//
// \brief The expect statement can be given in many formats. The basic forms are
//    described below:
//
// \param - `expect(<condition>);` - ex: `expect(var == 5); expect(var < c);
//    expect(str_eq(a, b));` etc. Does any truthy test, but output is limited to
//    the string equivalent of `condition`. The benefit of course is that this
//    can be used for just about anything.
//
// \param - `expect(A, operator, B);` - ex: `expect(a, == , b);` - Similar to
//    the above, but with options separated by commas. The difference is that it
//    will determine the types and print the values of A and B on a failed test.
//    Note: determining types won't work in versions older than C11.
//
// \param - `expect(to_pass(<comparator_fn>, A, B));` - ex `expect(to_pass(
//    str_eq, lhs, rhs));`. Calls the given function with two arguments. Same as
//    passing a function result directly, but on fail can print the value of A.
//
// \param - `expect(A, operator, B, TYPE);` - ex: `expect(a, < , b, float);`
//    Same as above, but A and B will be explicitly converted to TYPE, and will
//    have their values printed in addition to the expression.
//
// \param - `expect(A, operator, B, type_A, type_B)` - Same as above, but
//    A and B are treated as separate types for output.
//
// \param - `expect(<directive>);` - ex: `expect(to_fail);` - Sets a general
//    expectation for the test or sets some kind of internal execution state.
//
// \param - `expect(A to <matcher>)` - ex: `expect(a to be_positive);` - Tests
//    the value of A against the given matcher expression. Matchers can be made
//    in a variety of forms, and are described individaully below.
//
// \param - `expect(cont to all(<matcher>, TYPE, TYPE_CON));` - Tests all values
//    of the given container `cont` against the given matcher. See description
//    of `all` for details.
//
// \param - `expect(cont to all_be(operator, B, TYPE, TYPE_CON));` - Tests all
//    values of the given container `cont` against the expression described by
//    `<element> operator B` (ex: `arr[n] > 5`). See description of `all_be`
//    for details.
#define expect(...)               _expect(#__VA_ARGS__, __VA_ARGS__)

////////////////////////////////////////////////////////////////////////////////
// Directives
////////////////////////////////////////////////////////////////////////////////

// \brief A pre-test directive telling the system that the test being run is
//    supposed to fail. When this is used before a test, if the test would fail,
//    it's logged as a success, but if it would otherwise succeed, it's logged
//    as a failure.
//
// \param expect(to_fail);
#define to_fail                   _test_expect_to_fail()

// \brief Memory errors are treated differently from regular errors; a test
//    expecting to fail will still actually fail if it encounters memory
//    problems. This will similarly expect memory errors to occur in the test.
//    This is mostly only for testing the memory checker itself, there's little
//    other reason to use this.
//
// \param expect(memory_errors);
#define memory_errors             _test_memory_expect_to_fail()

// \brief Force the next call to malloc to return NULL. Only the first call to
//    malloc after this will fail. If malloc is not called, the test will fail.
//
// \brief Note: this also impacts allocations through calloc and realloc,
//    including when realloc would normally just grow the memory space.
//
// \param expect(null_malloc)
#define null_malloc               _test_memory_malloc_null(TRUE)

// \brief Force all remaining attempts to allocate memory for this test to fail.
//
// \brief Note: this also impacts allocations through calloc and realloc,
//    including when realloc would normally just grow the memory space.
//
// \param expect(null_mallocs)
#define null_mallocs              _test_memory_malloc_null(FALSE)

////////////////////////////////////////////////////////////////////////////////
// Matchers
////////////////////////////////////////////////////////////////////////////////

// \brief This can be used as syntactic sugar for matchers.
//
// \param - `expect(value to <matcher>);`
#define to ,

// \brief A little syntactic sugar for `be_within`, as a treat.
//
// \param - `expect(value to be_within(A of B));`
#define of ,

#ifndef not
// \brief Syntactic sugar used to negate matchers.
//
// \param - `expect(value to not be_positive);`.
# define not !
#endif

// \brief This is an example of a matcher which checks if a value is positive.
//    it's functionally equivalent* to `expect(A > 0)`, except that it will also
//    print the value of A.
//
// \param - `expect(value to be_positive);` - The value given on the left is
//    evaluated by the macro given on the right. A simple matcher can be any
//    basic test that takes a single value and does a statically defined test.
#define be_positive(A)            ((A) > 0)

// \brief Checks that a given value is negative.
//
// \param - `expect(value to be_negative);`
#define be_negative(A)            ((A) < 0)

// \brief Checks that a given value is even.
//
// \param - `expect(value to be_even);`
#define be_even(A)                ((A) % 2 == 0)

// \brief Checks that a given value is odd.
//
// \brief - `expect(value to be_odd);`
#define be_odd(A)                 ((A) % 2 != 0)

// \brief Evaluates the result of a function given two arguments. This is
//    functionally the same as `expect(function(subject, param_2))` except in
//    that it is able to deduce the type and print the value of the subject on
//    test failure.
//
// \brief Example: `expect(subject to match(function, param_2));`
//
// \param fn - The function to be called
//
// \param B - The value to test the subject against, second parameter to fn
#define match(fn, B)              _fn_comp(fn, B)

// \brief Evaluates the result of a function given two arguments. This is
//    functionally the same as `expect(function(subject, param_2))` except in
//    that it is able to deduce the type and print the value of the subject on
//    test failure.
//
// \brief Example: `expect(to_pass(function, subject, param_2));`
//
// \param fn - The function to be called
//
// \param A - The subject variable of the test, first parameter to fn
//
// \param B - The value to test the subject against, second parameter to fn
#define to_pass(fn, A, B)         A, _fn_comp(fn, B)

// \brief Evaluates the result of a function given two arguments and fails the
//    test if the value is non-zero. This is the same as
//    `expect(subject to not match(function, param_2));` and is only really here
//    because to_pass is not compatible with the `not` macro :(
//
// \brief Example: `expect(to_not_pass(function, subject, param_2));`
//
// \param fn - The function to be called
//
// \param A - The subject variable of the test, first parameter to fn
//
// \param B - The value to test the subject against, second parameter to fn
#define to_not_pass(fn, A, B)     A, !_fn_comp(fn, B)

// \brief This is a matcher that takes params and uses them to compose a more
//    complex expectation for the test. It checks if the value is between a
//    minimum and maximum bound.
//
// \brief By default, the check is inclusive. It will infer the type based on
//    the first parameter, unless pre-C11 where it assumes integer values.
//
// \param - `expect(value to be_between(A, B));` - succeeds if the given value
//    is between A and B, inclusive.
//
// \param - `expect(value to be_between(A, B, <mode>));` - sets the function
//    mode. Default value is `inclusive`, also accepts `exclusive`,
//    `exclusive_start`, and `exclusive_end`.
//
// \param - `expect(value to be_between(A, B, inclusive, <type>));` - sets an
//    explicit type to use for the comparison. By defualt, the type is inferred
//    using typeof. If pre-C11, the type defaults to int.
#define be_between(...)           _be_between(__VA_ARGS__)

// \brief This matcher will check if the test value is within a certain range
//    of the target value.
//
// \brief By default, the check is inclusive. It will infer the type based on
//    the first parameter, unless pre-C11 where it assumes integer values.
//
// \param - `expect(value to be_within(A of B));` - succeeds if the value is
//    within A units of B in either direction, inclusive.
//
// \param - `expect(value to be_within(A of B, <mode>));` - sets the function
//    mode. Default value is `inclusive`, also accepts `exclusive`.
//
// \param - `expect(value to be_within(A of B, inclusive, <type>)); - sets an
//    explicit type to use for the comparison. By defualt, the type is inferred
//    using typeof. If pre-C11, the type defaults to int.
#define be_within(...)            _be_within(__VA_ARGS__)

// \brief This matcher will check if a floating point number is about equal to
//    the given value, within the (inclusive) margin set by `about_epsilon`.
//
// \param - `expect(value to be_about(5.0f));` - expects the value to be
//    approximately 5.0f.
#define be_about(N)               _be_within(about_epsilon, N, inclusive, float)

#ifndef about_epsilon
# define about_epsilon 0.0001f
#endif

// \brief The `all` parameter is a composite matcher that applies the condition
//    of the given matcher to all elements in a container. All values in the
//    container must satisfy the condition in order to pass.
//
// \brief In order to function, the container must support a "foreach_index"
//    macro that functions in the same way as c_array_foreach_index.
//
// \brief Example: `expect(arr to all(be_positive, int, c_array));`
//
// \brief Example: `expect(arr to all(be_about(samples[n]), float, c_array));`
// 
// \brief Example: `expect(arr to all(my_fn, rhs, int, c_array));`
//
// \param matcher - A regular matcher, such as be_positive, or be_within(A, B),
//    or a function that takes the container element as its first of up to two
//    arguments.
//
// \param value [opt] - A value to pass to the matcher as a second parameter if
//    needed. The value to compare against can either be a single value to use
//    across the whole array, or it can be passed as a piecewise comparison by
//    using `value[n]`. Notes: bounds are not checked, so first ensure the input
//    container is of the correct size. This value is not side-effect safe; it
//    will be evaluated for every iteration of the loop.
//
// \param T_elem - The type of the elements in the container. Note: this should
//    be the actual type of the elements in the container, not the pointer type
//    expected to be returned from, say, TYPE_get() (ie, void*).
//
// \param T_cont - The type of container. This value is not the actual type name
//    of the container's struct, but the associated prefix before a
//    _foreach_index macro (ex: c_array).
#define all(matcher, ...) _all_va(matcher, __VA_ARGS__, _all_match, _all)

// \brief Alternate explicit alias for using a function with second parameter as
//    a matcher. Functions the same as calling `expect(lhs to match(fn, value))`
//    on each element of the container.
//
// \brief Example: `expect(arr to all_match(my_fn, rhs, int, c_array));`
//
// \param matcher - A function or macro that takes two arguments and returns a
//    boolean value.
//
// \param value - The value to test each subject with, second parameter to fn.
//
// \param T_el - The type of the elements in the container.
//
// \param T_cont - The type of container. This value is not the actual type name
//    of the container's struct, but the associated prefix before a
//    _foreach_index macro (ex: c_array).
#define all_match(matcher, value, T_el, T_con) all(matcher, value, T_el, T_con)

// \brief The `all_be` matcher is similar to the `all` matcher, but rather than
//    composing other matchers, it applies a basic expression to every element
//    in the container, as if calling `expect(<expression>)` on them.
//
// \brief Example: `expect(arr to all_be( > , 7, int, c_array));`
//
// \brief Example: `expect(arr to all_be( == , data[n], int, c_array));`
//
// \param op - A basic C comparison operator (==, !=, >, <, >=, <=)
//
// \param value - The value to compare container elements with. If passed as an
//    array with subscript [n], will function as a piecewise comparison - bounds
//    will not be checked, so perform an expect(result.size == x) first.
//    Note: this parameter is NOT side-effect safe; it will be evaluated in
//    every iteration of the loop.
//
// \param T_elem - The type of the elements of the container
//
// \param T_cont - The type of the container. Like with the `all` matcher, this
//    is not an actual type name of an object/struct, but a prefix used by
//    associated functions, in particular the foreach_index macro.
#define all_be(op, value, T_elem, T_cont) _all_be(op, value, T_elem, T_cont)

#define with ,

#ifndef c_array_foreach_index
// \brief A macro to provide an indexed foreach that can be used to iterate over
//    a standard C-style array (ex: `int test_array[] = { ... }`).
//
// \brief This style macro is used by the `all` matchers to enumerate containers
//    and should be replicated for any other containers you want to support with
//    expect(all).
//
// \brief The basic usage (outside the context of testing) is:
//    `int* c_array_foreach_index(it, i, test_array) { *it = i; }`
//    (sets array values to ascending order: [0, 1, 2, 3, 4, 5, ...])
//
// \param it - The name of the iterating pointer into the container,  accessible
//    within the body  of the loop.  The type of the  iterator is set before the
//    macro (as a pointer).
//
// \param i - The name of a variable that will hold a sequential index.
//    This may not be necessary to access elements in the test container (ie, if
//    it's iterating a linked list), but is used to output which iteration the
//    test fails on, and can be used in `expect(all)` checks to index an array
//    of sample values to compare against by using `[n]`.
//
// \param arr - The container to iterate over, in this case, a C-style array
//    delcared with the form `type arr[n]`.
#define c_array_foreach_index(iter, i, arr)                                 \
  iter = NULL;                                                              \
  for (csUint i = 0; i < ARRAY_COUNT(arr) ? (iter = &arr[i]), 1 : 0; ++i)   //
#endif

#ifndef c_array_ptr_foreach_index
// \brief A macro to provide an indexed foreach that can be used to iterate over
//    a C-style array referenced by a pointer type (ex: `char* test = "..."`);
//
// \brief This is similar to the c_array_foreach_index macro, but expects a size
//    value to be set alongside the given array. The size is expected to be an
//    unsigned integer value with the same name as the input array with the
//    suffix _size (ex: test_size). The size denotes the number of elements,
//    not the size of the whole array in bytes.
//
// \param it - The name of the iterating pointer into the container, accessible
//    within the body of the loop.
//
// \param i - The name of an index variable that will increase by one each loop.
//
// \param arr - A pointer to a C-style array whose quantity of elements is
//    denoted in a variable named `<arr>_size`.
#define c_array_ptr_foreach_index(iter, i, arr)               \
  iter = NULL;                                                \
  for (csUint i = 0;                                          \
      i < MACRO_CONCAT(arr, _size) ? (iter = &arr[i]), 1 : 0; \
      ++i                                                     \
  )                                                           //
#endif

////////////////////////////////////////////////////////////////////////////////
// Other
////////////////////////////////////////////////////////////////////////////////

// \brief Gets the number of calls to malloc at this point in test execution
//
// \param - `expect(malloc_count == 1);`
#define malloc_count _test_memory_malloc_count()

// \brief Gets the number of calls to free at this point in test execution
//
// \param - `expect(free_count == 1);`
#define free_count _test_memory_free_count()

////////////////////////////////////////////////////////////////////////////////
// Extras
////////////////////////////////////////////////////////////////////////////////

typedef csUint (*resolve_user_types_fn)
  (const char** p_type, const void* value, char* out_buffer, csUint out_size);

// \brief A function pointer that is initially null, but can be set by a user to
//    describe how to print custom types without having to modify cspec.c.
//
// \param &p_type - a pointer to a pointer to c-string containing the name of 
//    the type to convert. You can compare this to a typename you've defined,
//    and either add a custom handler to write output to the buffer, or you can
//    change the pointer to a string representing an equivalent type that can
//    already be parsed by cspec. Ex: you can use `if(strcmp(*ptyp_N, "MyInt")
//    == 0) { *ptyp_N = "int"; return 0; }` to set MyInt as an alias for int.
//
// \param value - A pointer to the object being written.
//
// \param out_buffer - The char buffer that can be written to.
//
// \param out_size - The amount of space available in the write buffer.
//
// \returns The number of characters written to the buffer. Returns 0 if no
//    characters were written to the buffer, indicating that the type was not
//    resolved in the user handler and will be written out internally instead.
//    In this case, if ptyp_N was modified, the updated value will be used to
//    determine how to write the contents of N.
extern resolve_user_types_fn resolve_user_types;

////////////////////////////////////////////////////////////////////////////////
// Implementation details, turn back now, here there be dragons.
////////////////////////////////////////////////////////////////////////////////

#ifndef assert
# if defined(__WASM__) && defined(__has_builtin)
#  if __has_builtin(__builtin_trap)
#   define define assert(CONDITION) (!(CONDITION) ? __builtin_trap() : 0);
#  endif
# endif
#endif
#ifndef assert
# define assert(C)
#endif

#ifndef NULL
# define NULL ((void*)0)
#endif

#ifndef TRUE
# define TRUE ((csBool)1)
#endif

#ifndef FALSE
# define FALSE ((csBool)0)
#endif

#ifndef MACRO_CONCAT
# define MACRO_CONCAT_RECUR(X, Y) X ## Y
# define MACRO_CONCAT(X, Y) MACRO_CONCAT_RECUR(X, Y)
#endif

#ifndef STR
# define STR_RECUR(S) #S
# define STR(S) STR_RECUR(S)
#endif

#ifndef ARRAY_COUNT
# define ARRAY_COUNT(arr) (sizeof(arr) / sizeof(*arr))
#endif

////////////////////////////////////////////////////////////////////////////////
// I warned you, turn back now, it gets ugly.
////////////////////////////////////////////////////////////////////////////////

#include <stdarg.h>

csBool  _test_begin(int line, const char* desc);
csBool  _test_end(void);
csBool  _test_active(void);
csBool  _test_context_begin(int line, const char* desc);
csBool  _test_context_end(int line);
void    _test_log_fn(int line, const char* messgae);
void    _test_warn_fn(int line, const char* message);
void    _test_error_fn(const char* message);
void    _test_error_typed(int line, const char* pfix, const char* fmt, ...);
csBool  _test_expect_to_fail(void);
csBool  _test_memory_expect_to_fail(void);
csBool  _test_memory_malloc_null(csBool only_next);
int     _test_memory_malloc_count(void);
int     _test_memory_free_count(void);
void    _test_memory_log_block(int line, const void* ptr);
int     _test_run_all(int count, TestSuite* suites[], int argc, char* argv[]);

////////////////////////////////////////////////////////////////////////////////
// I warned you about MACROS bro!!! I told you dog!
////////////////////////////////////////////////////////////////////////////////

#define _test_run_all_suites(Suites) _test_run_all(sizeof(Suites) / sizeof(TestSuite*), Suites, argc, argv)

#if __STDC_VERSION__ >= 201112
# define _USE_DEDUCTION
#endif

#define LINESTR STR(__LINE__)

#if defined(_USE_DEDUCTION)
# ifndef CSPEC_CUSTOM_TYPES
#  define CSPEC_CUSTOM_TYPES
# endif
# define _type_s_lit(X, T) _Generic((&X), T**: #T"*", default: #T"[]" )
# define _type_s_h(X, T) T: #T, T*: _type_s_lit(X, T), const T*: _type_s_lit(X, const T)
/*
# define _type_s(X) _Generic((X), void*: "void*", const void*: "const void*",                                         \
  CSPEC_CUSTOM_TYPES   _type_s_h(X, _Bool),                                                                           \
  _type_s_h(X, char),  _type_s_h(X, short), _type_s_h(X, int),    _type_s_h(X, long),   _type_s_h(X, long long),      \
  _type_s_h(X, unsigned char), _type_s_h(X, unsigned short),      _type_s_h(X, unsigned int),                         \
  _type_s_h(X, unsigned long), _type_s_h(X, unsigned long long),  _type_s_h(X, float),  _type_s_h(X, double)          \
)                                                                                                                     //
/*/
// abridged version that's easier on the msvc macro previewer, lol
# define _type_s(X) _Generic((X),                                             \
  CSPEC_CUSTOM_TYPES                _Bool:              "bool",               \
  _type_s_h(X, char),               _type_s_h(X, int),                        \
  void*:          "void*",          const void*:        "const void*",        \
  long:           "long",           long long:          "llong",              \
  float:          "float",          double:             "double",             \
  unsigned char:  "unsigned char",  unsigned char*:     "unsigned char*",     \
  unsigned int:   "unsigned int",   unsigned int*:      "unsigned int*",      \
  unsigned long:  "unsigned long",  unsigned long long: "unsigned long long"  \
)                                                                             //
//*/
#endif

#define _loop_tst MACRO_CONCAT(_loop_tst_, __LINE__)
#define _loop_ctx MACRO_CONCAT(_loop_ctx_, __LINE__)
#define _iter_all MACRO_CONCAT(_iter_all_, __LINE__)
#define _loop_all n //MACRO_CONCAT(_loop_all_, __LINE__)

#define _describe(NAME) static const int _fn_line_##NAME = __LINE__; void test_##NAME(void)
#define _context(DESC) for (int _loop_ctx = 0; (_loop_ctx++ < 2) && _test_context_begin(__LINE__, "context: %c["LINESTR"] "DESC);) if (_loop_ctx == 2) { if (_test_context_end(__LINE__)) return; } else
#define _test(DESC) for (int _loop_tst = 0; _loop_tst++ < 1 && _test_begin(__LINE__, "test %c["LINESTR"] "DESC);)
#define _after for (int _loop_ctx = 0; _loop_ctx++ < 1 && _test_active();)

#define _test_suite(NAME) TestSuite NAME = { .header="in file: %c"__FILE__, .filename=__FILE__, .test_groups = (TestGroup(*)[])(&(TestGroup[])
#define _test_group(TEST_FN) { .line = &_fn_line_##TEST_FN, .header=#TEST_FN, .group_fn = test_##TEST_FN }
#define _test_suite_end { .line = NULL, .group_fn = NULL } })

#define _test_log(message) _test_log_fn(__LINE__, message)
#define _test_warn(message) _test_warn_fn(__LINE__, message)
#define _test_fail(issue) do { _test_error_fn(issue); return; } while(0)
#define _test_fail_args(S, /* fmt, */ ...) _test_error_typed(__LINE__, S, __VA_ARGS__, NULL, NULL)
#define _test_fail_t(A, x, B, sTa, sTb) do { _test_fail_args("expected "#A" "#x" "#B, "%n\nreceived {} "#x" {}", sTa, &_A, sTb, &_B); return; } while(0)
#define _test_fail_all(S, T)                                                                                                                    \
  _test_fail_args("expected "S, NULL); if (_pvalue) {                                                                                           \
    if (_print_expected_value) _test_fail_args("", "but found {} on iteration {}\nexpecting {}", #T, _pvalue, "uint", &_index, #T, &_expected); \
    else                       _test_fail_args("", "but found {} on iteration {}",               #T, _pvalue, "uint", &_index);                 \
  return;                                                                                                                                       \
}                                                                                                                                               //

#define _expect_comp_all(S, A, E, B, x, T, F, ...) csBool _test = F(A, B, E, x); if(!_test) { _test_fail_all(S, T); }
#define _expect_type2(S, A, x, B, T, t, ...) T _A=(A); t _B=(B); if(!(_A x _B)) _test_fail_t(A, x, B, #T, #t)
#define _expect_type1(S, A, x, B, T, ...) T _A=(A); T _B=(B); if(!(_A x _B)) _test_fail_t(A, x, B, #T, #T)
//#define _expect_comp3(S, F, H, P, ...) if (H != (F P) { _test_fail_args("expected to pass "#F"( "#P" )", "%n\nparam 1 {}\nparam 2 {}", _type_s(_A), &_A, _type_s(_B), &_B); return; }
#define _expect_comp2(S, A, F, B, ...) typeof(A) _A = A; typeof(B) _B = B; if (!(F(_A, _B))) { _test_fail_args("expected to pass "#F"( "#A", "#B" )", "%n\nparam 1 {}\nparam 2 {}", _type_s(_A), &_A, _type_s(_B), &_B); return; }
#define _expect_true2(S, A, x, B, ...) typeof(A) _A=(A); typeof(B) _B=(B); if (!(_A x _B)) _test_fail_t(A, x, B, _type_s(_A), _type_s(_B))
#define _expect_comp(S, A, F, ...) typeof(A) _Aout = (A); csBool _test = F(_Aout); if(!(_test)) { _test_fail_args("expected "S, "%n\nreceived {}", _type_s(_Aout), &_Aout); return; }
#define _expect_true(S, A, ...) if(!(A)) _test_fail("line "STR(__LINE__)": expected "S)
#define _expect_va(S, U, V, W, X, Y, Z, _,__,F, ...) do { _expect##F(S, U, V, W, X, Y, Z); } while(0)
#define _expect(S, ...) _expect_va(S, __VA_ARGS__, _comp3, _comp2, _comp_all, _type2, _type1, _true2, _comp, _true)

/*
#define _fn_comp_args(A) A, _B
#define _fn_wow2(a, b, c) a(b(c))
#define _fn_wow(a, b) _fn_wow2(a, b)
#define _fn_break2(a, ...) a, __VA_ARGS__
#define _fn_break(...) _fn_break2(__VA_ARGS__)
#define _fn_magic(fn, args, ...) _fn_wow(_fn_break fn, args)
#define _fn_comp(fn, B) (FALSE; typeof(B) _B = B; _test ^= fn, _fn_comp_args)
*/

#define _fn_comp(fn, B) fn, B, 0, 0, 0, 0

//#define succeed_with(A, B) 1, (A, B), 0, 0, 0, 0, 0
//#define succeed 1, (), 0, 0, 0, 0, 0 // kinda dumb? It's just different syntax for expect(fn()), but expect(fn to succeed).

#define _matcher_setup(B, C, T) FALSE; T _B = (B); T _C = (C); T _A =

#define _be_type_inclusive(T, R) T
#define _be_type_default(T, R) R
#define _be_type_typeof(_) _be_type_default

#define _be_between_exclusive(A) (A); _test ^= (_B < _A && _A < _C)
#define _be_between_inclusive(A) (A); _test ^= (_B <= _A && _A <= _C)
#define _be_between_exclusive_end(A) (A); _test ^= (_B <= _A && _A < C)
#define _be_between_exclusive_start(A) (A); _test ^= (_B < _A && _A <= C)
#define _be_between_va(B_LO, C_HI, MODE, T, T_RES, ...) _matcher_setup(B_LO, C_HI, _be_type_##T_RES(T, T_RES)) _be_between_##MODE
#define _be_between(B, ...) _be_between_va(B, __VA_ARGS__, inclusive, typeof(B), typeof(B))

#define _be_within_exclusive(A) (A); _test ^= (_C - _B < _A && _A < _C + _B)
#define _be_within_inclusive(A) (A); _test ^= (_C - _B <= _A && _A <= _C + _B)
#define _be_within_va(B_EXT, C_MID, MODE, T, T_RES, ...) _matcher_setup(B_EXT, C_MID, _be_type_##T_RES(T, T_RES)) _be_within_##MODE
#define _be_within(B, ...) _be_within_va(B, __VA_ARGS__, inclusive, typeof(B), typeof(B))

#define _all_comp_part(A, FOREACH, MATCHER, EXPECTED) FOREACH(_iter_all, _loop_all, A) { _test = MATCHER; if (!_test) { _index = _loop_all; _pvalue = _iter_all; EXPECTED break; } } _test ^= _tmp
#define _all_comp(A, B, FOREACH, F) _all_comp_part(A, FOREACH, F(*_iter_all), )
#define _all_be_comp(A, B, FOREACH, x) _all_comp_part(A, FOREACH, ((*_iter_all) x B), _expected = B;)
#define _all_match_comp(A, B, FOREACH, F) _all_comp_part(A, FOREACH, F(*_iter_all, B), _expected = B;)

#define _all_setup(T) FALSE; csBool _tmp = _test; csUint _index = 0; void* _pvalue = NULL; T _expected; csBool _print_expected_value = FALSE
#define _all(matcher, T_el, T_con, ...) _all_setup(T_el); T_el* T_con##_foreach_index, 0, matcher, T_el, _all_comp
#define _all_be(x, B, T_el, T_con) _all_setup(T_el); _print_expected_value = TRUE; T_el* T_con##_foreach_index, B, x, T_el, _all_be_comp
#define _all_match(x, B, T_el, T_con, ...) _all_setup(T_el); _print_expected_value = TRUE; T_el* T_con##_foreach_index, B, x, T_el, _all_match_comp

#define _all_va(matcher, B, C, D, F, ...) F(matcher, B, C, D)

//#define be(opr, value) opr (value),
//#define _all_va(T_el, T_con, matcher, _, F, ...) _all_setup T_el* T_con##_foreach_index, matcher, F, T_el, 0
//#define _all(T_el, T_con, ...) _all_va(T_el, T_con, __VA_ARGS__, _all_be_comp, _all_comp)
//#define _all(matcher, T_el, T_container) TRUE; T_el* T_container##_foreach, matcher, 0, _all_b, _all_a

#ifndef _USE_DEDUCTION
# undef _expect_true2
# define _expect_true2(S, A, B, C, ...)                                       \
  _test_warn("output type deduction is disabled pre C11");                    \
  _test_warn("use `expect("#A", "#B" , "#C", type)` for value display");      \
  if(!(A B C)) _test_fail("line "STR(__LINE__)": expected "#A" "#B" "#C)      //

# undef _expect_comp
# define _expect_comp(S, A, B, ...) csBool _test = B(A); if(!(_test)) _test_fail("line "STR(__LINE__)": expected "S)

# undef _expect_comp2
# define _expect_comp2(S, A, F, B, ...) if (!F(A, B)) { _test_fail("line "STR(__LINE__)": expected to pass "#F"( "#A", "#B" )"); }

# undef _eval
# undef _eval_comp
# undef to_pass

// \brief `to_pass` and `match` don't work in C11 without typeof, but at least
//    to_pass can functionally operate even though it can't print variables.
# define to_pass(fn, A, C) fn(A, C)

# undef _be_between
# undef _be_within
# define _be_type_int _be_type_default
# define _be_between(B, ...) _be_between_va(B, __VA_ARGS__, inclusive, int, int)
# define _be_within(B, ...) _be_within_va(B, __VA_ARGS__, inclusive, int, int)
#endif

#endif
