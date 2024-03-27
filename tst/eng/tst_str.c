#include "tst.h"

#include "str.h"

#include <stdlib.h>

test_func(test_new) {

  test_log("This is a note before the test");
  test_log("This is another note before the test");

  String test = str_new("This is a copy of a c-string");

  before

  test("'tst_fail' just outright fails with a message") {
    test_fail("I failed because I felt like it");
  }

  test("'expect' tests any two values, but won't print variable values") {
    expect(2.0, >, 1.0);
    expect(TRUE, !=, FALSE);
    expect(str_empty->size, ==, 1);
    expect(2, ==, 1);
    str_print(test->range);
  }

  test("'expect_int' tests two int variables and prints the values") {
    int i = 2, j = 3;
    expect_int(i, >, j);
  }

  test("'expect_float' tests two float variables") {
    float a = 3.4f, b = 1.9f;
    expect_float(a, <, b);
  }

  test("Allocates memory and never frees") {
    //String s = str_new("This allocates and prints a string without deleting");
    //str_print(s->range);
    test_warn("This is another note before the test");
  }

  after

  test_end;
}

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
