#ifndef _TST_H_
#define _TST_H_

#include "types.h"
#include "str.h"

#include "wasm.h"

#define LINESTR STR(__LINE__)
#define LINERPT &R(STR(__LINE__))

typedef int (*test_fn)(int line, int context);

typedef struct TestGroup {
  int line;
  StringRange header;
  test_fn group_fn;
} TestGroup;

typedef struct TestSuite {
  StringRange header;
  StringRange filename;
  TestGroup test_groups[];
} TestSuite;

bool _test_begin(int line, const StringRange* desc);
bool _test_end(int line);
bool _test_context(int line);
void _test_error(const StringRange* message);
void _test_log(const StringRange* messgae);
void _test_warn(const StringRange* message);
void _test_error_params(const StringRange* fmt, const void* a, const void* b);

void test_run_suite(const TestSuite* suite);
int _test_run_all(int count, TestSuite* suites[], int argc, char* argv[]);
#define test_run_all(Suites) _test_run_all(sizeof(Suites) / sizeof(TestSuite*), Suites, argc, argv)

#define test_func(NAME) int NAME(int _line, int _context)
#define test(DESC) while(0); if (_test_end(__LINE__)) return __LINE__; else if (_test_begin(__LINE__, &R("    ["LINESTR"] %c"DESC))) do
#define test_end while(0); _test_end(0); return 0
#define context while(0); _test_end(0); if (_test_context(__LINE__)) do {
#define block
#define context_end while(0); }

#define test_not_implemented { .line = 0, .group_fn = NULL },
#define test_group(TEST_FN) { .line = __LINE__, .header = R("  in function ("LINESTR"): %c"#TEST_FN), .group_fn = TEST_FN },
#define test_suite_begin(NAME) TestSuite NAME = { .header = R("in file: %c"__FILE__), .filename = R(__FILE__), .test_groups = {
#define test_suite_end test_not_implemented } }

#define _test_msg(msg, c) &R("      on line "LINESTR": "c msg)
#define test_log(message) if (_line < __LINE__) _test_log(_test_msg(message, ""))
#define test_note(message) if (_line < __LINE__) test_log(message)
#define test_warn(message) if (_line < __LINE__) _test_warn(_test_msg(message, "%c"))
#define test_fail(issue) { _test_error(_test_msg(issue, "")); break; }
#define expect(lhs, cmp, rhs) unless((lhs) cmp (rhs)) test_fail(#lhs" "#cmp" "#rhs)
#define _test_fail(s) _test_error_params(_test_msg(s, ""), &_A, &_B); break;
#define _expect(Lf, C, Rt, t) _test_fail(#Lf" "#C" "#Rt" with values: %"#t" "#C" %"#t)
#define _expect_t(Lf, C, Rt, t, T) { T _A=(Lf), _B=(Rt); unless(_A C _B) _expect(Lf, C, Rt, t) }
#define expect_int(lhs, cmp, rhs) _expect_t(lhs, cmp, rhs, i, int)
#define expect_float(lhs, cmp, rhs) _expect_t(lhs, cmp, rhs, f, float)
#define expect_bool(lhs, cmp, rhs) _expect_t(lhs, cmp, rhs, b, bool)

// Test suites

extern TestSuite tests_string;

#endif
