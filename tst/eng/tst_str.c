#include "tst.h"

#include "str.h"

#include <stdlib.h>


void _test_error_params2(const StringRange* fmt, const void* a, const void* b, const char* ta, const char* tb);

test_func(test_new) {

  print("test pass");

  test_log("This is a note before the test");
  test_log("This is another note before the test");

  // test_vars

  int incrementor = 1;

  /*
  #define EXPECT_T(A, B, C, T) if ((T)A B (T)C);
  #define EXPECT_int(A, B, C) EXPECT_T(A, B, C, int)
  #define EXPECT_float(A, B, C) EXPECT_T(A, B, C, float)
  #define EXPECT_(A, B, C) if (A B C)

  #define TEST_ERROR(S, A, C, ta, tc) _test_error_params2(S, &_A, &_C, #ta, #tc);

  #define EXPECT_FAIL(S)
  #define EXPECT_FAIL_T(A, B, C, ta, tc) TEST_ERROR(_test_msg(#A" "#B" "#C" with values: ("#ta"){} "#B" "#tc"{}", ""), &_A, &_B, ta, tc);
  #define EXPECT_2(A, B, C, D, E) { D _A = (A); E _C = (C); unless (_A B _C); EXPECT_FAIL_T(A, B, C, D, E) }
  #define EXPECT_1(A, B, C, D, ...) { D _A = (A), _C = (C); unless(_A B _C) EXPECT_FAIL_T(A, B, C, D, D); }
  #define EXPECT_0(A, B, C, ...) EXPECT_TRUE((A) B (C))
  #define EXPECT_EXPAND_2(A, B, C, D, E, F, ...) EXPECT_T##F(A, B, C, D, E) // if (A B C) #D; #E; #F;
  #define EXPECT_EXPAND(A, B, ...) EXPECT_EXPAND_2(A, B, __VA_ARGS__, _2, _1, _0)
  #define EXPECT_INVALID(...) Invalid 'expect' macro: use one of the following forms (A), (A < B), (A, <, B), (A, <, B, type), or (A, <, B, A_type, B_type)
  #define EXPECT_TRUE(A, ...) unless(A) "fail case here";
  #define EXPECT(A, B, ...) EXPECT_EXPAND(A, B, __VA_ARGS__)
  #define EXPECT2_EXP2(A, B, C, D, E, F, ...) EXPECT##F(A, B, C, D, E) // #A #B #C #D #E #F #__VA_ARGS__
  #define EXPECT2_EXP(...) EXPECT2_EXP2(__VA_ARGS__, _2, _1, _0, _INVALID, _TRUE)
  #define EXPECT2(...) EXPECT2_EXP(__VA_ARGS__)

  // "      on line 36: (int)blah < (char)2 with values: 1 < 2"

  //float blah = 1.4f;
  //EXPECT2(TRUE, ==, FALSE, int);
  //"idbfusl";

  //*/


  test("'tst_fail' just outright fails with a message") {
    print("test: 0");
    test_fail("I failed because I felt like it");
  }

  test("increments the pre-test variable") {
    print("test: 1");
    expect_int(++incrementor, ==, 0);
  }


  test("increments the same variable again but doesn't actually") {
    print("test: 2");
    expect_int(++incrementor, ==, 0);
  }

  // TODO:
  //    - Make context skip like tests do
  //    - Use _tstlo__LINE__ for tests so we don't need do/while matching
  //    - Context doesn't need a do/while or vars actually, just "_end(__LINE__)"
  //      - no need for break; support outside of tests.
  //    - Contexts unforutnately can't be nested with just the line number :/
  //      - but regular blocks will work. They just won't skip between tests.
  //    - Update un-rolled example below and keep this in an "example" file.
  //      - both for publishing, and let's be real, I'm forgetting this in a week
  //    - Do memory tester!
  //
  //    - Make string builder (stb) per spec in str.c
  //    - Eventually json...
  context("String 'test' exists")
    print("context: 1");

    String test = str_new("This is a copy of a c-string");

    test("doesn't fail because a break; saves us from the fail statement") {
      print("test: 3");
      break;
      test_fail("Can't reach this");
    }

    test("'expect' tests any two values, but won't print variable values") {
      print("test: 4");
      str_print(test->range);
      expect(2.0, >, 1.0);
      expect(TRUE, !=, FALSE);
      expect(str_empty->size, ==, 1);
      expect(2, ==, 1);
    }

    context("This is an inner context")
      print("context: 2");

      test("'expect_int' tests two int variables and prints the values") {
        print("test: 5");
        int i = 2, j = 3;
        expect_int(i, >, j);
      }

    context_end

  context_end

  test("'expect_float' tests two float variables") {
    print("test: 6");
    float a = 3.4f, b = 1.9f;
    expect_float(a, <, b);
  }

  test("Allocates memory and never frees") {
    print("test: 7");
    str_new("This allocates a string without deleting");
    test_warn("This is another note before the test");
  }

  test_end;
}

#if 0
/* test_func(test_macros) */
int test_macros(int _line, int _context)
/**/
{
  int LINE = 0; // because splitting the macro makes it not work :P

  // user vars for every test can go here, resets every pass

  /* context */
  while(0); LINE = __LINE__;
  char _ctxlo61 = 1;
  if (_test_context(LINE)) while(_ctxlo61--) // if _context > LINE, don't use this context
  /**/
  {

    // user vars here, resets every pass while context is in scope

    /* test */
    while(0); LINE = __LINE__;
    if (_line < LINE) // we're closing the previous test's run
      return _test_reset(LINE, &R("    ["LINESTR"] %c"));
    else if (_line == LINE) do // if line > LINE, we've already done this test, so skip it
    /**/
    {

      // user code here

    }
    /* test */
    while(0); LINE = __LINE__;
    if (_line < LINE) // we're closing the previous test's run
      return _test_reset(LINE, &R("    ["LINESTR"] %c"));
    else if (_line == LINE) do // if line > LINE, we've already done this test, so skip it
    /**/
    {

      // user code here

    }
    /* context */
    while(0); LINE = __LINE__;
    if (_test_context(LINE)) do { // if _context > LINE, don't use this context
    /**/

      /* test */
      while(0); LINE = __LINE__;
      if (_line < LINE) // we're closing the previous test's run
        return _test_reset(LINE, &R("    ["LINESTR"] %c"));
      else if (_line == LINE) do // if line > LINE, we've already done this test, so skip it
      /**/
      {

        // user code here

      }

    /* context_end */
    while(0); }
    /**/

  /* context_end */
  while(0); }
  /**/

  /* test_end */
  while(0); LINE = __LINE__;
  _test_reset(0, &R(""));
  return 0;
  /**/
}
#endif

test_func(test_compare) {

  test("A second test group") {

    test_log("This is just a note");
    test_log("This is another note");
    test_log("More notes here");
    test_log("These won't display with verbose off");
    test_warn("But this will!");
    test_warn("Another warning!");
    test_warn("Uh oh!");
    test_fail("The error should cause the header to reprint in red");

  }

  test_end;
}

test_suite_begin(tests_string)
  test_group(test_new)
  test_group(test_compare)
test_suite_end;
