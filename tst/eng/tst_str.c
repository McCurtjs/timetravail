

#include "str.h"
#include "tst.h"
#include <stdlib.h>

#ifdef _MSC_VER
#pragma warning ( push )
// Disable MSVC warning "conditional expression is constant"
#pragma warning ( disable : 4127 )
#endif


void _test_error_params2(const StringRange* fmt, const void* a, const void* b, const char* ta, const char* tb);

#ifdef SKIP_THIS
test_func(test_new) {

  StringRange asdf = M("asdf");

  test_log("This is a note before the test");
  test_log("This is another note before the test");

  //StringRange some_string = R("This is just some string");
  //Array arr = array_new(int);

  //expect(5 == 5);
  //expect(5, ==, 5);
  //expect(5, ==, 5, int);
  //expect(5, ==, 5, float, int);
  //expect(TRUE == FALSE);
  //expect(str_contains(some_string, R("Stuff")));
  //expect(5, to_not be_between(2, 6));
  //expect(5, to_not be_between(2, 6, float));
  //expect(5, to_not be_between(2, 6, float, exclusive));
  //expect(5.1, to be_within(5.0f of 5.5f, float));
  //expect(3, to be_positive);
  //expect(arr, to all(be_positive, int, array));
  //expect(arr, to_not all(be_between(2, 3), int, array));
  //expect(3, to be_within(1 of 4, int, exclusive));
  //expect(arr, to all(not be_within(1 of 5), int, array));
  //expect(arr, to_not all(not be_within(1 of 5, float, exclusive), float, array));
  //expect(arr, to all(be( > , 7), int, array));
  //expect(5, to be(< , 7));


  // "      on line 36: (int)blah < (char)2 with values: 1 < 2"

  //float blah = 1.4f;
  //EXPECT2(TRUE, ==, FALSE, bool);
  //"idbfusl";

  /*

  // new Expect fn:
  // fmt is the input string (ie, "    on line N: blah < what with values {} < {}")
  // desc is an optional description to append to the failure description above
  // a is a pointer to parameter value a (ignored if null)
  // ta is type of param a (ignored if null)
  // tb_or_desc could be either the type of b, or a description (given valid forms:
  //    expect(a, <, b, int, "description") and expect(a, <, b, int, char, "description");
  //    so in this case, check tb_or_desc against all included type values. If it doesn't
  //    match any of them (and "desc" is NULL), set "desc" to "tb_or_desc" before running
  //    the code to print "desc".
  void _test_error_params3(const StringRange* fmt, StringRange desc,
    const void* a, const void* b, const char* ta, const char* tb_or_desc);

  // unrelated: should there be a "success" macro that just auto-succeeds if hit?
  // note: actually, that's just "break;"

  //*/

  test("'tst_fail' just outright fails with a message") {
    test_fail("I failed because I felt like it");
  }

  context("String 'test' exists")

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
    }

    context("This is an inner context")

      test("'expect_int' tests two int variables and prints the values") {
        int i = 2, j = 3;
        //expect_int(i, >, j);
      }

    context_end

  context_end

  test("'expect_float' tests two float variables") {
    float a = 3.4f, b = 1.9f;
    //expect_float(a, <, b);
  }

  test("Allocates memory and never frees") {
    str_new("This allocates a string without deleting");
    test_warn("This is another note before the test");
  }

  test_end;
}

#endif

#ifdef _MSC_VER
#pragma warning ( push )
// Disable MSVC warning about the break; test causing unreachable code.
#pragma warning ( disable : 4702 )
#endif
describe(tests) {

  test("an empty test that succeeds") { }

  test("doesn't fail because a break; saves us from the fail statement") {
    break;
    test_fail("Can't reach this");
  }

  it("prints a warning but doesn't fail") {
    test_warn("This is a warning");
  }

  context("tests fail") {

    expect(to_fail);

    test("'test_fail' just causes a test to outright fail") {
      test_fail("I failed because I felt like it");
    }

    test("logs a message (only visible with a verbose/-v setting) then fails") {
      test_log("this causes the header to print twice... would like to fix, but hey");
      test_fail("oops, failed again");
    }

    test("another fail to balance output...") {
      test_fail("Yep, it fails");
    }

    //test("is expected to fail but succeeds, so it fails");
  }

}
#ifdef _MSC_VER
#pragma warning ( pop )
#endif

describe(memory) {

  it("allocates memory and never frees") {
    expect(to_fail);
    str_new("This allocates a string without deleting");
  }

  it("properly frees the memory after allocating") {
    String s = str_new("This is a string being allocated");
    str_delete(&s);
  }

}

describe(contexts) {

}

describe(expect_basic) {

  StringRange str1 = R("Test string");

  context("using the basic format without commas") {

    context("tests succeed") {

      test("most basic equality check") {
        expect(2 == 2);
      }

      test("boolean (aka, macroed) values") {
        expect(TRUE != FALSE);
      }

      test("float macro value") {
        expect(PI > 1);
      }

      test("using other operator") {
        expect(2 < 3);
      }

      test("string compare") {
        expect(str_eq(str1, R("Test string")));
      }

      test("more string funcs") {
        expect(str_contains(str1, R("Test")));
      }

    }

    context("tests fail") {

      expect(to_fail);

      test("most basic equality check") {
        expect(2 == 3);
      }

      test("boolean (aka, macroed) values") {
        expect(TRUE == FALSE);
      }

      test("float macro value") {
        expect(PI < 1);
      }

      test("using other operator") {
        expect(2 > 3);
      }

      test("string compare") {
        expect(str_eq(str1, R("Something")));
      }

      test("more string funcs") {
        expect(str_contains(str1, R("xyz")));
      }

    }

  }

}

describe(expect_basic_triplet) {

  float pi = PI;

  context("using the basic format but with commas") {

    context("tests succeed") {

      test("most basic equality check") {
        expect(2, == , 2);
      }

      test("boolean (aka, macroed) values") {
        expect(TRUE, != , FALSE);
      }

      test("float macro value") {
        expect(PI, > , 1);
      }

      test("float variable value") {
        expect(pi, > , 1);
      }

      test("using other operator") {
        expect(2, < , 3);
      }

    }

    context("tests fail") {

      expect(to_fail);

      test("most basic equality check") {
        expect(2, == , 3);
      }

      test("float macro value (compare with output in expect_basic)") {
        expect(PI, < , 1);
      }

      test("float variable value (compare with output in expect_basic)") {
        expect(pi, < , 1);
      }

      test("boolean (aka, macroed) values (compare with output in expect_basic)") {
        expect(TRUE, == , FALSE);
      }

      test("using other operator") {
        expect(2, > , 3);
      }

    }

  }

}

describe(expect_basic_var_output) {

  int incrementor = 1;
  float pi = PI;

  context("basic comparison expectations with value output") {
    float x = 0.5;

    context("tests succeed") {
      x = 10;

      test("incrementing context-scoped variable (starts at 1)") {
        expect(++incrementor, == , 2, int);
      }

      test("incrementing same variable, doesn't fail because context is reloaded") {
        expect(++incrementor, == , 2, int);
      }

      test("using global value") {
        expect(str_empty->size, == , 0, size_t);
      }

      test("using floating point values") {
        expect(PI, > , 1.0f, float);
      }

      test("floating point variable output") {
        expect(pi, > , 1.0f, float);
      }

      test("two floating point variables - x has context specific value") {
        expect(pi, < , x, float);
      }

      test("using boolean values") {
        expect(TRUE, != , FALSE, bool);
      }

      test("using different type specifiers") {
        expect(x, == , 10, float, int);
      }

    }

    context("tests fail") {

      expect(to_fail);

      test("incrementing context-scoped variable (starts at 1)") {
        expect(++incrementor, == , 3, int);
      }

      test("incrementing same variable, doesn't reach threshold because context is reloaded") {
        expect(++incrementor, == , 3, int);
      }

      test("using global value") {
        expect(str_empty->size, == , 1, size_t);
      }

      test("using floating point values") {
        expect(PI, == , 1.0f, float);
      }

      test("floating point variable output") {
        expect(pi, == , 1.0f, float);
      }

      test("two floating point variables - same test, context var reset after previous context") {
        expect(pi, < , x, float);
      }

      test("using boolean values") {
        expect(TRUE, == , FALSE, bool);
      }

      test("using different type specifiers") {
        expect(x, == , 10, float, int);
      }

    }

  }

}

describe(matchers) {

  context("compositions on singular values") {

    context("tests succeed") {

      it("has a positive value") {
        expect(3, to be_positive);
      }

      it("uses a to_not specifier") {
        expect(-3, to_not be_positive);
      }

      it("gives an incrementing value to a matcher that generates temporary values (i starts at 2)") {
        int i = 2;
        expect(++i, to be_between(2, 3));
        expect(i, ==, 3, int);
      }

      it("uses to_not on a matcher that generates temporary") {
        expect(4, to_not be_between(2, 3));
      }

      it("uses the 'be' matcher but on a single item") {
        expect(5, to be( > , 4));
      }

    }

    context("tests fail") {

      expect(to_fail);

      it("uses the simplest kind of matcher using no temporary values") {
        expect(-3, to be_positive);
      }

      it("uses a to_not modifier") {
        expect(3, to_not be_positive);
      }

      it("uses a matcher that generates temporary values (i starts at 2)") {
        int i = 2;
        expect(++i, to be_between(1, 2));
      }

      it("uses to_not on a matcher that generates temporary") {
        expect(4, to_not be_between(3, 5));
      }

      it("uses the 'be' matcher but on a single item") {
        expect(5, to be( < , 4));
      }

    }

  }

}

#include "array.h"

describe(container_matchers) {

  context("compositions on an int array [3, 5, 7]") {

    Array arr = array_new(int);

    array_push_back(arr, &(int){3});
    array_push_back(arr, &(int){5});
    array_push_back(arr, &(int){7});

    context("tests succeed") {

      it("contains only positive values") {
        expect(arr, to all(be_positive, int, array));
      }

      context("a negative number is added to the array [..., -1]") {
        array_push_back(arr, &(int){-1});

        it("does not contain only positive values") {
          expect(arr, to_not all(be_positive, int, array));
        }
      }

      it("contains values within 2 of 5") {
        expect(arr, to all(be_within(2 of 5), int, array));
      }

      it("contains values that are not all within 2 of 6") {
        expect(arr, to_not all(be_within(2 of 6), int, array));
      }

      it("contains all values which are not within 2 of 10") {
        expect(arr, to all(not be_within(2 of 10), int, array));
      }

      it("contains at least one value within 2 of 8") {
        expect(arr, to_not all(not be_within(2 of 8), int, array));
      }

      it("compares the values using the 'be' matcher") {
        expect(arr, to all(be( < , 10), int, array));
      }

      it("contains values not all equal to 3") {
        expect(arr, to_not all(be( == , 3), int, array));
      }

      it("contains values all not equal to 4") {
        expect(arr, to all(be(!= , 4), int, array));
      }

      it("contains only non-even values") {
        expect(arr, to all(be( %2 != , 0), int, array));
      }

    }

    context("tests fail") {

      expect(to_fail);

      context("a negative number is added to the array [..., -1]") {
        array_push_back(arr, &(int){-1});

        it("contains only positive values") {
          expect(arr, to all(be_positive, int, array));
        }

        it("wants ONLY values that are not positive") {
          expect(arr, to all(not be_positive, int, array));
        }
      }

      it("wants values only within 2 of 4") {
        expect(arr, to all(be_within(2 of 4), int, array));
      }

      it("wants values that are not all within 2 of 5") {
        expect(arr, to_not all(be_within(2 of 5), int, array));
      }

      it("wants only values which are not within 2 of 9") {
        expect(arr, to all(not be_within(2 of 9), int, array));
      }

      it("wants at least one value within 2 of 10") {
        expect(arr, to_not all(not be_within(2 of 10), int, array));
      }

      it("checks that all numbers are over 5") {
        expect(arr, to all(be( > , 5), int, array));
      }

      it("asks for not all numbers to be less than 10") {
        expect(arr, to_not all(be( < , 10), int, array));
      }

      it("contains at least one value equal to 4") {
        expect(arr, to_not all(be( != , 4), int, array));
      }

      it("wants at least one even value") {
        expect(arr, to_not all(be(% 2 != , 0), int, array));
      }

    }

    array_delete(&arr);
  }
}

describe(matcher_be_positive) {

}

describe(matcher_be_between) {

}

describe(matcher_be_within) {

}

test_suite_begin(tests_string) {
  test_group(tests),
  test_group(memory),
  test_group(contexts),
  test_group(expect_basic),
  test_group(expect_basic_triplet),
  test_group(expect_basic_var_output),
  test_group(matchers),
  test_group(container_matchers),
  test_group(matcher_be_positive),
  test_group(matcher_be_between),
  test_group(matcher_be_within),
  test_suite_end
};

// TODO:
//    - Update un-rolled example below and keep this in an "example" file.
//      - both for publishing, and let's be real, I'm forgetting this in a week
//    - Do memory tester!
//
//    - Eventually json...

#ifdef _MSC_VER
#pragma warning ( pop )
#endif
