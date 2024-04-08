#ifndef _TST_H_
#define _TST_H_

#include "types.h"
#include "str.h"

#include "wasm.h"

typedef void (*test_fn)(void);

typedef struct TestGroup {
  const int* line;
  StringRange header;
  test_fn group_fn;
} TestGroup;

typedef struct TestSuite {
  StringRange header;
  StringRange filename;
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
//        int widget_operate(void) {
//            // very critically important source code
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
//        }
//
//    file: /test/test_main.c
//
//        int main(int argc, char* argv[])
//        {
//            TestSuite* suites[] = {
//                widget_tests
//            };
//
//            return test_run_all_suites(argc, argv, suites);
//        }
//

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
#define describe(NAME)          _describe(NAME)
#define test_func(NAME)         _describe(NAME)

// \brief Ends a description block (would like to find a way to avoid needing).
#define describe_end            _describe_end
#define test_end                _describe_end

// \brief An `it` block declares an example case for testing.
//
// \brief Each "it" statement is run one at a time in its own execution context
//    that won't impact the result of other examples.
//
// \param DESC - String Literal: a brief description of the test that will be
//    printed with the test results.
#define it(DESC)                _test("it "DESC)
#define test(DESC)              _test(DESC)

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
//  to be given to the test_run functions to execute the tests.
#define test_suite_begin(NAME) _test_suite_begin(NAME)

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
#define test_group(TEST_FN) _test_group(TEST_FN)

// \brief Must be included at the end of the test suite declaration list.
//    (thinking of a way to avoid needing this)
#define test_suite_end _test_suite_end

void test_run_suite(const TestSuite* suite);
#define test_run_all(suites) _test_run_all_suites(suites)

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
#define context(DESC) _context(DESC)

// \brief Closes a test context
#define context_end _context_end

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
#define test_log(message) _test_log(message)

// \brief An alias for `test_log`
#define test_note(message) _test_log(message)

// \brief Logs a warning message in the console output. The message is of higher
//    importance than a basic log, and will appear even if the verbose level is
//    not set.
//
// \param warning - String Literal: The warning to be printed. Same
//    restrictions as with test_log.
#define test_warn(warning) _test_warn(warning)

// \brief Automatically fails the test. Do not pass GO. Do not collect $200.
//
// \param issue - String Literal: The error to print.
#define test_fail(issue) _test_fail(issue)

////////////////////////////////////////////////////////////////////////////////
// Value checking with "Expect"
////////////////////////////////////////////////////////////////////////////////

// \brief An `expect` clause within a test is used to check the validity of
//    output for the operations being tested. The value passed is expected to
//    evaluate to TRUE, and if it doesn't, the test aborts as a failure
//
// \brief The expect statement can be given in many formats. The basic forms are
//    described below:
//
// \param - `expect(condition);` - ex: `expect(var == 5); expect(var < c);
//    expect(str_eq(a, b));` etc. Does any truthy test, but output is limited to
//    the string equivalent of `condition`. The benefit of course is that this
//    can be used for just about anything.
//
// \param - `expect(A, operator, B);` - ex: `expect(a, == , b);` - Same as
//    above, but with options separated by commas.
//
// \param - `expect(A, operator, B, TYPE);` - ex: `expect(a, < , b, float);`
//    Same as above, but A and B must be convertable to type TYPE, and will have
//    their values printed in addition to the expression.
//
// \param - `expect(A, operator, B, type_A, type_B)` - Same as above, but
//    A and B are treated as separate types for output.
//
// \param - `expect(...)` - There are other uses for the `expect` macro that
//    make use of other parameters, such as matchers. The usage of these will be
//    described below.
#define expect(...) _expect(#__VA_ARGS__, __VA_ARGS__)

////////////////////////////////////////////////////////////////////////////////
// Directives
////////////////////////////////////////////////////////////////////////////////

// \brief A pre-test directive telling the system that the test being run is
//    supposed to fail. When this is used before a test, if the test would fail,
//    it's logged as a success, but if it would otherwise succeed, it's logged
//    as a failure.
//
// \param expect(to_fail);
#define to_fail _test_expect_to_fail()

// \brief Memory errors are treated differently from regular errors; a test
//    expecting to fail will still actually fail if it encounters memory. This
//    will similarly expect memory errors to occur in the test. This is mostly
//    only for testing the memory checker itself, there's little other reason
//    to use this.
//
// \param expect(memory_errors);
#define memory_errors _test_memory_expect_to_fail()

////////////////////////////////////////////////////////////////////////////////
// Matchers
////////////////////////////////////////////////////////////////////////////////

// \brief This can be used as syntactic sugar for matchers
//
// \param - `expect(value to matcher);`
#define to ,

#ifndef not
// \brief Syntactic sugar used to negate matchers.
//
// \param - `expect(value to not be_positive);`.
# define not !
#endif

// \brief This is an example of a matcher which checks if a value is positive.
//    it's functionally equivalent to `expect(A >= 0)`, but serves as a proof of
//    concept for matchers in general.
//
// \param - `expect(value, to be_positive);` - The value given on the left is
//    evaluated by the macro given on the right. A simple matcher can be any
//    basic test that takes a single value and does a statically defined test.
#define be_positive(A) ((A) >= 0)

// \brief This is a matcher that takes params and uses them to compose a more
//    complex expectation for the test. It checks if the value is between a
//    minimum and maximum bound. By default, the check is inclusive and assumes
//    integer values.
//
// \param - `expect(value, to be_between(A, B));` - succeeds if the given value
//    is between A and B inclusive. In this case, the value is expected to be
//    an int.
//
// \param - `expect(value, to be_between(A, B, TYPE));` - inclusive check
//    between A and B, but interprets all three as values of type TYPE.
//
// \param - `expect(value, to be_between(A, B, TYPE, inclusive));` - same as
//    previous, but can be explicitly specified as inclusive or exclusive.
#define be_between(...) _be_between(__VA_ARGS__)

// \brief This matcher will check if the test value is between two numbers.
//    By default, the check is inclusive and assumes integer values.
//
// \param - `expect(value, to be_within(A of B));` - expects the value to be
//    within A of B in either direction. All three params are expected to be
//    ints, and the check is inclusive.
//
// \param - `expect(value, to be_within(A of B, TYPE));` - same as above but
//    specifies a type for the comparison.
//
// \param - `expect(value, to be_within(A of B, TYPE, inclusive)); - same as
//    above, but can be explicitly specified as inclusive or exclusive.
#define be_within(...) _be_within(__VA_ARGS__)

// \brief A little syntactic sugar for `be_within`, as a treat.
#define of ,

// \brief The `all` parameter is a composite matcher that applies the condition
//    of the given matcher to all elements in a container. All values in the
//    container must satisfy the condition in order to pass.
//
// \brief In order to function, the container must support a "foreach" macro
//    that takes the form (using array for example),
//    `int* array_foreach(it_val, arr) {...}`, as well as a "get" function in
//    the form `void* array_get(arr);`.
//
// \brief Given that definition for iterating an `array` class, the expect all
//    check would look like: `expect(arr, to all(be_positive, int, array));`.
//
// \param matcher - A regular matcher, such as be_positive, or be_within(A, B).
//
// \param T_elem - The type of the elements in the container. Note: this
//    should be the actual type of the elements in the container, not the
//    pointer type expected to be returned from TYPE_get().
//
// \param T_cont - The type of container. This value is not the actual
//    type name of the container's struct, but an associated prefix expected to
//    be used by functions associated with that type. For example, the
//    aforementioned "array_get" function is associated with the struct `Array`.
#define all(matcher, T_elem, T_cont) _all(matcher, T_elem, T_cont)

// \brief The `all_be` matcher is similar to the `all` matcher, but rather than
//    composing other matchers, it applies a basic expression to every element
//    in the container. 
// 
// \brief Example: expect(arr to all_be( > , 7, int, array));
//
// \param op - A basic C comparison operator (==, !=, >, <, >=, <=)
//
// \param value - The value to compare container elements with
//
// \T_elem - The type of the elements of the container
//
// \T_cont - The type of the container. Like with the `all` matcher, this is not
//    an actual type name of an object/struct, but a standard prefix used by
//    functions associated with the type (ie: array_get(...))
#define all_be(op, value, T_elem, T_cont) _all_be(op, value, T_elem, T_cont)

////////////////////////////////////////////////////////////////////////////////
// Implementation details, turn back now, here there be dragons.
////////////////////////////////////////////////////////////////////////////////

bool _test_begin(int line, StringRange desc);
bool _test_end();
bool _test_context_begin(int line, StringRange desc);
bool _test_context_end(int line);
void _test_log_fn(int line, const StringRange* messgae);
void _test_warn_fn(int line, const StringRange* message);
void _test_error_fn(const StringRange* message);
bool _test_expect_to_fail();
bool _test_memory_expect_to_fail();
int  _test_run_all(int count, TestSuite* suites[], int argc, char* argv[]);
void _test_error_typed(
  const StringRange* prefix, const StringRange* fmt,
  const void* A, const void* B,
  const StringRange* type_A, const StringRange* type_B
);

#define _test_run_all_suites(Suites) _test_run_all(sizeof(Suites) / sizeof(TestSuite*), Suites, argc, argv)

#define LINESTR STR(__LINE__)
#define _test_msg(msg, c) &R("line "LINESTR": "c msg)
#define _test_msg2(msg, fmt, c) &R("line "LINESTR": "c msg), &R(fmt)

#define _loop_tst MACRO_CONCAT(_loop_tst_, __LINE__)
#define _loop_ctx MACRO_CONCAT(_loop_ctx_, __LINE__)
#define _iter_all MACRO_CONCAT(_iter_all_, __LINE__)
#define _loop_all MACRO_CONCAT(_loop_all_, __LINE__)

#define _describe(NAME) static const int _fn_line_##NAME = __LINE__; void test_##NAME(void)
#define _context(DESC) for (int _loop_ctx = 0; (_loop_ctx++ < 2) && _test_context_begin(__LINE__, R("context: %c["LINESTR"] "DESC));) if (_loop_ctx == 2) do { if (_test_context_end(__LINE__)) return; } while(0); else
#define _test(DESC) for (int _loop_tst = 0; _loop_tst++ < 1 && _test_begin(__LINE__, R("test %c["LINESTR"] "DESC));)

#define _test_suite_begin(NAME) TestSuite NAME = { .header=M("in file: %c"__FILE__), .filename = M(__FILE__), .test_groups = (TestGroup(*)[])(&(TestGroup[])
#define _test_group(TEST_FN) { .line = &_fn_line_##TEST_FN, .header=M("): %ctest_"#TEST_FN), .group_fn = test_##TEST_FN }
#define _test_suite_end { .line = NULL, .group_fn = NULL } })

#define _test_log(message) _test_log_fn(__LINE__, _test_msg(message, ""))
#define _test_warn(message) _test_warn_fn(__LINE__, _test_msg(message, "%c"))
#define _test_fail(issue) do { _test_error_fn(_test_msg(issue, "")); return; } while(0)
#define _test_fail_args(E, fmt, A, C, Ta, Tc) do { _test_error_typed(_test_msg(E, ""), &R(fmt), A, C, &R(#Ta), &R(#Tc)); return; } while(0)
#define _test_fail_t(A, B, C, Ta, Tc) _test_fail_args("expected "#A" "#B" "#C, " but got values: $ "#B" $", &_A, &_C, Ta, Tc);

#define _expect_comp_all(S, A, B, C, F, T, ...) do { bool _test = F(A, B, C); unless(_test) _test_fail_args("expected "S, ", but found $ on iteration $", _pvalue, &_index, T, uint); } while(0)
#define _expect_type2(S, A, B, C, D, E, ...) do { D _A=(A); E _C=(C); unless(_A B _C) _test_fail_t(A, B, C, D, E); } while(0)
#define _expect_type1(S, A, B, C, D, ...) do { D _A=(A); D _C=(C); unless(_A B _C) _test_fail_t(A, B, C, D, D); } while(0)
#define _expect_true2(S, A, B, C, ...) _expect_true(#A" "#B" "#C, (A) B C)
#define _expect_comp(S, A, B, ...) do { bool _test = B(A); unless(_test) _test_fail("expected "S); } while(0)
#define _expect_true(S, A, ...) do { unless(A) _test_fail("expected "S); } while(0)
#define _expect_va(S, A, B, C, D, E, _, F, ...) _expect##F(S, A, B, C, D, E)
#define _expect(S, ...) _expect_va(S, __VA_ARGS__, _comp_all, _type2, _type1, _true2, _comp, _true)

#define _matcher_setup(B, C, D) FALSE; D _B = (B); D _C = (C); D _A =

#define _be_between_exclusive(A) (A); _test ^= (_B < _A && _A < _C)
#define _be_between_inclusive(A) (A); _test ^= (_B <= _A && _A <= _C)
#define _be_between_int(A) _be_between_inclusive(A)
#define _be_between_va(B, C, D, F, ...) _matcher_setup(B, C, D) _be_between_##F
#define _be_between(...) _be_between_va(__VA_ARGS__, int, inclusive, exclusive)

#define _be_within_exclusive(A) (A); _test ^= (_C - _B < _A && _A < _C + _B)
#define _be_within_inclusive(A) (A); _test ^= (_C - _B <= _A && _A <= _C + _B)
#define _be_within_int(A) _be_within_inclusive(A)
#define _be_within_va(B, C, D, F, ...) _matcher_setup(B, C, D) _be_within_##F
#define _be_within(...) _be_within_va(__VA_ARGS__, int, inclusive, exclusive)

#define _all_comp_part(A, FOREACH, MATCHER) FOREACH(_iter_all, _loop_all, A) { _test = MATCHER; if (!_test) { _index = _loop_all; _pvalue = _iter_all; break; } } _test ^= _tmp
#define _all_comp(A, B, C) _all_comp_part(A, B, C(*_iter_all))
#define _all_be_comp(A, B, C) _all_comp_part(A, B, ((*_iter_all) C))

#define _all_setup FALSE; bool _tmp = _test; uint _index = 0; void* _pvalue = NULL;
#define _all(matcher, T_el, T_con) _all_setup T_el* T_con##_foreach_index, matcher, _all_comp, T_el, 0
#define _all_be(B, C, T_el, T_con) _all_setup T_el _C = (C); T_el* T_con##_foreach_index, B _C, _all_be_comp, T_el, 0

//#define be(opr, value) opr (value),
//#define _all_va(T_el, T_con, matcher, _, F, ...) _all_setup T_el* T_con##_foreach_index, matcher, F, T_el, 0
//#define _all(T_el, T_con, ...) _all_va(T_el, T_con, __VA_ARGS__, _all_be_comp, _all_comp)
//#define _all(matcher, T_el, T_container) TRUE; T_el* T_container##_foreach, matcher, 0, _all_b, _all_a

// Test suites

extern TestSuite tests_string;

#endif
