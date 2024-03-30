#ifndef _TST_H_
#define _TST_H_

#include "types.h"
#include "str.h"

#include "wasm.h"

#define LINESTR STR(__LINE__)

typedef int (*test_fn)(int line);

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

typedef struct TestContext {
  int line;

} TestContext;

bool _test_begin(int line, const StringRange* desc);
bool _test_end(int line);
bool _test_context_begin(int line, const StringRange* desc);
void _test_context_end(int line);
void _test_error(const StringRange* message);
void _test_log(const StringRange* messgae);
void _test_warn(const StringRange* message);
//void _test_error_params(const StringRange* fmt, const void* a, const void* b);
//void _test_error_2(const StringRange* fmt, const StringRange* desc, 
//  const void* a, const void* b, const void* tA, const void* tB);

void test_run_suite(const TestSuite* suite);
int _test_run_all(int count, TestSuite* suites[], int argc, char* argv[]);
#define test_run_all(Suites) _test_run_all(sizeof(Suites) / sizeof(TestSuite*), Suites, argc, argv)

#define describe(NAME) test_func(NAME)
#define test_func(NAME) const static int _fn_line_##NAME = __LINE__; int test_##NAME(int _line)
#define it(DESC) test("it "DESC)
#define test(DESC) while(0); if (_test_end(__LINE__)) return __LINE__; else if (_test_begin(__LINE__, &R("test %c["LINESTR"] "DESC))) do
#define test_end while(0); _test_end(0); return 0
#define context(DESC) while(0); if (_test_end(__LINE__)) return __LINE__; if (_test_context_begin(__LINE__, &R("context: %c["LINESTR"] "DESC))) {
#define context_end while(0); _test_end(__LINE__); _test_context_end(__LINE__); return __LINE__; }

#define test_not_implemented { .line = 0, .group_fn = NULL },
#define test_group(TEST_FN) { .line = _fn_line_##TEST_FN, .header=M("): %ctest_"#TEST_FN), .group_fn = test_##TEST_FN },
#define test_suite_begin(NAME) TestSuite NAME = { .header=M("in file: %c"__FILE__), .filename = M(__FILE__), .test_groups = {
#define test_suite_end test_not_implemented } }

#define _test_msg(msg, c) &R("line "LINESTR": "c msg)
#define test_log(message) if (_line < __LINE__) _test_log(_test_msg(message, ""))
#define test_note(message) if (_line < __LINE__) test_log(message)
#define test_warn(message) if (_line < __LINE__) _test_warn(_test_msg(message, "%c"))
#define test_fail(issue) do { _test_error(_test_msg(issue, "")); return __LINE__; } while(0)
//#define expect(lhs, cmp, rhs) unless((lhs) cmp (rhs)) test_fail(#lhs" "#cmp" "#rhs)
//#define _test_fail(s) _test_error_params(_test_msg(s, ""), &_A, &_B); break;
//#define _expect(Lf, C, Rt, t) _test_fail(#Lf" "#C" "#Rt" with values: %"#t" "#C" %"#t)
//#define _expect_t(Lf, C, Rt, t, T) { T _A=(Lf), _B=(Rt); unless(_A C _B) _expect(Lf, C, Rt, t) }
//#define expect_int(lhs, cmp, rhs) _expect_t(lhs, cmp, rhs, i, int)
//#define expect_float(lhs, cmp, rhs) _expect_t(lhs, cmp, rhs, f, float)
//#define expect_bool(lhs, cmp, rhs) _expect_t(lhs, cmp, rhs, b, bool)



#define expect(...) _expect(#__VA_ARGS__, __VA_ARGS__)

#define to_fail _test_expect_to_fail()

// matchers
#define be_between(...) _be_between(__VA_ARGS__)
#define be_within(...) _be_within(__VA_ARGS__)
#define be_positive(A) ((A) >= 0)

#define be(B, C) B (C), 

#define all(matcher, element_type, container_type) _all(element_type, container_type, matcher)

#define to
#define to_not !
#define not_to !
#define of ,

#ifndef not
# define not !
#endif

bool _test_expect_to_fail();

void _test_error_typed(const StringRange* fmt, const void* A, const void* B, 
  const StringRange* type_A, const StringRange* type_B);

#define _expect_t(A, B, C, Ta, Tc) _test_error_typed(_test_msg(#A" "#B" "#C" with values: $ "#B" $", ""), &_A, &_C, &R(#Ta), &R(#Tc))
#define _expect_comp2(S, A, B, C, D, ...) do { bool _test = D(A, B, C); unless(_test) test_fail("expected "#S); } while(0)
#define _expect_type2(S, A, B, C, D, E, ...) do { D _A=(A); E _C=(C); unless(_A B _C) _expect_t(A, B, C, D, E); } while(0)
#define _expect_type1(S, A, B, C, D, ...) do { D _A=(A); D _C=(C); unless(_A B _C) _expect_t(A, B, C, D, D); } while(0)
#define _expect_true2(S, A, B, C, ...) _expect_true(#A" "#B" "#C, (A) B C)
#define _expect_comp(S, A, B, ...) do { bool _test = B(A); unless(_test) test_fail("expected "S); } while(0)
#define _expect_true(S, A, ...) do { unless(A) test_fail("expected "S); } while(0)
#define _expect_va(S, A, B, C, D, E, _, F, ...) _expect##F(S, A, B, C, D, E)
#define _expect(S, ...) _expect_va(S, __VA_ARGS__, _comp2, _type2, _type1, _true2, _comp, _true)

#define _matcher_setup(B, C, D) FALSE; D _B = (B); D _C = (C); D _A =

#define _be_between_exclusive(A) (A); _test ^= (_B < _A && _A < _C)
#define _be_between_inclusive(A) (A); _test ^= (_B <= _A && _A <= _C)
#define _be_between_int(A) _be_between_inclusive(A)
#define _be_between_va(B, C, D, F, ...) _matcher_setup(B, C, D) _be_between_##F
#define _be_between(...) _be_between_va(__VA_ARGS__, int, inclusive, exclusive)

#define _be_within_exclusive(A) (A); _test ^= (_B - _C < _A && _A < _B + _C)
#define _be_within_inclusive(A) (A); _test ^= (_B - _C <= _A && _A <= _B + _C)
#define _be_within_int(A) _be_within_inclusive(A)
#define _be_within_va(B, C, D, F, ...) _matcher_setup(B, C, D) _be_within_##F
#define _be_within(...) _be_within_va(__VA_ARGS__, int, inclusive, exclusive)

#define _all_comp(A, B, C) B(_arrval, A) { bool _tmp = _test; _test = C(*_arrval); if (_test != _tmp) { break; } }
#define _be_comp(A, B, C) B(_arrval, A) { bool _tmp = _test; _test = ((*_arrval) C); if (_test != _tmp) { break; } }
#define _all_b(T_el, T_con, matcher) TRUE; T_el* T_con##_foreach, matcher, _be_comp, 0, 0
#define _all_a(T_el, T_con, matcher) TRUE; T_el* T_con##_foreach, matcher, _all_comp, 0, 0
#define _all_va(T_el, T_con, matcher, _, F, ...) F(T_el, T_con, matcher)
#define _all(T_el, T_con, ...) _all_va(T_el, T_con, __VA_ARGS__, _all_b, _all_a)
//#define _all(matcher, T_el, T_container) TRUE; T_el* T_container##_foreach, matcher, 0, _all_b, _all_a

// Test suites

extern TestSuite tests_string;

#endif
