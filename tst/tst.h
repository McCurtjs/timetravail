#ifndef _TST_H_
#define _TST_H_

#include "types.h"
#include "str.h"

#include "wasm.h"

#define LINESTR STR(__LINE__)
#define LINERPT &R(STR(__LINE__))

typedef bool (*test_fn)(uint line);

typedef struct TestGroup {
  size_t line;
  StringRange header;
  test_fn group_fn;
} TestGroup;

typedef struct TestSuite {
  StringRange header;
  StringRange filename;
  TestGroup test_groups[];
} TestSuite;

void _test_skip();
void _test_reset(const StringRange* line_no, const StringRange* desc);
void _test_error(const StringRange* message);
void _test_log(const StringRange* messgae);
void _test_warn(const StringRange* message);
void _test_error_params(const StringRange* fmt, const void* a, const void* b);

void test_run_suite(const TestSuite* suite);
int _test_run_all(int count, TestSuite* suites[], int argc, char* argv[]);
#define test_run_all(Suites) _test_run_all(sizeof(Suites) / sizeof(TestSuite*), Suites, argc, argv)

#define test_func(NAME) bool NAME(uint line) { do
#define test(DESC) } while(FALSE); _test_reset(LINERPT, &R("    ["LINESTR"] %c"DESC)); if (line && line != __LINE__) _test_skip(); else do {
#define test_end } while(FALSE); _test_reset(LINERPT, &R("")); return FALSE
#define before {
#define after } while(0); _test_reset(LINERPT, &R("")); _test_skip(); } while (0); do {

#define test_not_implemented { .line = 0, .group_fn = NULL },
#define test_group(TEST_FN) { .line = __LINE__, .header = R("  in function ("LINESTR"): %c"#TEST_FN), .group_fn = TEST_FN },
#define test_suite_begin(NAME) TestSuite NAME = { .header = R("in file: %c" __FILE__), .filename = R(__FILE__), .test_groups = {
#define test_suite_end test_not_implemented } }

#define _test_msg(msg, c) &R("      on line " LINESTR ": " c msg)
#define test_log(message) _test_log(_test_msg(message, ""))
#define test_note(message) test_log(message)
#define test_warn(message) _test_warn(_test_msg(message, "%c"))
#define test_fail(issue) { _test_error(_test_msg(issue, "")); break; }
#define _test_fail(s, a, b) { _test_error_params(_test_msg(s, ""), &a, &b); break; }
#define expect(lhs, cmp, rhs) unless(lhs cmp rhs) test_fail(#lhs" "#cmp" "#rhs)
#define _expect(Lf, C, Rt, t) _test_fail(#Lf" "#C" "#Rt" with values: %"#t" "#C" %"#t, Lf, Rt)
#define expect_int(lhs, cmp, rhs) unless(lhs cmp rhs) _expect(lhs, cmp, rhs, i)
#define expect_float(lhs, cmp, rhs) unless(lhs cmp rhs) _expect(lhs, cmp, rhs, f)
#define expect_bool(lhs, cmp, rhs) unless(lhs cmp rhs) _expect(lhs, cmp, rhs, b)

// Test suites

extern TestSuite tests_string;

#endif
