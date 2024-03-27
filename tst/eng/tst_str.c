#include "tst.h"

#include "str.h"

#include <stdlib.h>

test_func(test_new) {

  test_log("This is a note before the test");
  test_log("This is another note before the test");

  // test_vars

  int incrementor = 1;

  test("'tst_fail' just outright fails with a message") {
    test_fail("I failed because I felt like it");
  }

  test("increments the pre-test variable") {
    expect_int(++incrementor, ==, 0);
  }

  test("increments the same variable again but doesn't actually") {
    expect_int(++incrementor, ==, 0);
  }

  context

    String test = str_new("This is a copy of a c-string");

    test("doesn't fail because a break; saves us from the fail statement") {
      break;
      test_fail("Can't reach this");
    }

    test("'expect' tests any two values, but won't print variable values") {
      expect(2.0, >, 1.0);
      expect(TRUE, !=, FALSE);
      expect(str_empty->size, ==, 1);
      expect(2, ==, 1);
      str_print(test->range);
    }

  context_end

  test("'expect_int' tests two int variables and prints the values") {
    int i = 2, j = 3;
    expect_int(i, >, j);
  }

  test("'expect_float' tests two float variables") {
    float a = 3.4f, b = 1.9f;
    expect_float(a, <, b);
  }

  test("Allocates memory and never frees") {
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
