

#include "str.h"
#include "tst.h"
#include <stdlib.h>

#ifdef _MSC_VER
#pragma warning ( push )
// Disable MSVC warning "conditional expression is constant"
#pragma warning ( disable : 4127 )

#pragma warning ( push )
// Disable MSVC warning about the break; test causing unreachable code.
#pragma warning ( disable : 4702 ) // yeah... that's the point.
#endif
describe(tests) {

  test("an empty test that succeeds") { }

  test("doesn't fail because a break; saves us from the fail statement") {
    break;
    test_fail("Can't reach this");
  }

  it("prints a warning but doesn't fail") {
    test_warn("The warning has been given. Their fate is now their own.");
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

#ifdef _MSC_VER
#pragma warning ( push )
// Disable MSVC warning about reading from unallocatd memory
// Note: Why does it warn for _reading_ but not for _writing_, like wut.
#pragma warning ( disable : 6385 )
#endif
describe(memory) {

  context("tests succeed") {

    it("properly frees the memory after allocating") {
      String s = str_new("This is a string being allocated");
      str_delete(&s);
    }

    it("fills memory without overrunning") {
      char* buffer = malloc(5);
      expect(buffer != NULL);
      for (int i = 0; i < 5; ++i) {
        buffer[i] = '!';
      }
      expect(buffer[0], == , '!', char);
      expect(buffer[4], == , '!', char);
      expect(buffer[5], != , '!', char);
      free(buffer);
    }

    it("makes malloc return NULL once") {
      expect(null_malloc);

      char* buffer = malloc(5);
      expect(buffer == NULL);

      buffer = malloc(5);
      expect(buffer != NULL);
      free(buffer);
    }

    it("makes malloc return NULL for the rest of the test") {
      expect(null_mallocs);

      char* buffer = malloc(5);
      expect(buffer == NULL);

      buffer = malloc(5);
      expect(buffer == NULL);
    }

    it("makes sure malloc sets non-zero memory") {
      int* buffer = malloc(sizeof(int) * 5);
      for (int i = 0; i < 5; ++i) {
        expect(buffer[i] != 0);
      }
      free(buffer);
    }

    it("ensures calloc returns zero-initialized memory") {
      int* buffer = calloc(5, sizeof(int));
      for (int i = 0; i < 5; ++i) {
        expect(buffer[i] == 0);
      }
      free(buffer);
    }

  }

  context("tests fail due to memory errors") {

    expect(memory_errors);

    it("allocates memory and never frees") {
      str_new("This allocates a string without deleting");
    }

    it("causes a buffer overrun") {
      char* buffer = malloc(5);
      assert(buffer);
      for (int i = 0; i <= 5; ++i) {
        buffer[i] = '!';
      }
      free(buffer);
    }

    it("double-frees") {
      char* buffer = malloc(5);
      free(buffer);
      free(buffer);
    }

    it("passes a bad pointer to realloc") {
      char* buffer = realloc((void*)1, 5);
      free(buffer);
    }

    it("tries to free memory outside of the sandbox") {
      int x = 0;
      free(&x);
    }

    it("tries to free the wrong address within allocated memory") {
      char* buffer = malloc(5);
      free(buffer + 1);
      free(buffer);
    }

    it("modifies allocated memory after free") {
      char* buffer = malloc(5);
      expect(buffer != NULL);
      free(buffer);
      buffer[2] = '!';
    }

    //it("tries to allocate too much memory (can't be ignored with directive)") {
    //  char* buffer = malloc(999999);
    //  if (buffer) free(buffer);
    //}

  }

  context ("tests fail (requesting but not calling is a test issue, not memory)") {

    expect(to_fail);

    it("requests a null malloc, but doesn't call malloc") {
      expect(null_malloc);
    }

    it("requests rest of allocations to be NULL, but doesn't call malloc again") {
      expect(null_mallocs);
    }

  }

}
#ifdef _MSC_VER
#pragma warning ( pop )
#endif

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
        expect(3 to be_positive);
      }

      it("uses a to_not specifier") {
        expect(-3 to not be_positive);
      }

      it("gives an incrementing value to a matcher that generates temporary values (i starts at 2)") {
        int i = 2;
        expect(++i to be_between(2, 3));
        expect(i, ==, 3, int);
      }

      it("uses to_not on a matcher that generates temporary") {
        expect(4 to not be_between(2, 3));
      }

    }

    context("tests fail") {

      expect(to_fail);

      it("uses the simplest kind of matcher using no temporary values") {
        expect(-3 to be_positive);
      }

      it("uses a to_not modifier") {
        expect(3 to not be_positive);
      }

      it("uses a matcher that generates temporary values (i starts at 2)") {
        int i = 2;
        expect(++i to be_between(1, 2));
      }

      it("uses to_not on a matcher that generates temporary") {
        expect(4 to not be_between(3, 5));
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
        expect(arr to all(be_positive, int, array));
      }

      context("a negative number is added to the array [..., -1]") {
        array_push_back(arr, &(int){-1});

        it("does not contain only positive values") {
          expect(arr to not all(be_positive, int, array));
        }
      }

      it("contains values within 2 of 5") {
        expect(arr to all(be_within(2 of 5), int, array));
      }

      it("contains values that are not all within 2 of 6") {
        expect(arr to not all(be_within(2 of 6), int, array));
      }

      it("contains all values which are not within 2 of 10") {
        expect(arr to all(not be_within(2 of 10), int, array));
      }

      it("contains at least one value within 2 of 8") {
        expect(arr to not all(not be_within(2 of 8), int, array));
      }

      it("compares the values using the 'be' matcher") {
        expect(arr to all_be( < , 10, int, array));
      }

      it("contains values not all equal to 3") {
        expect(arr to not all_be( == , 3, int, array));
      }

      it("contains values all not equal to 4") {
        expect(arr to all_be( != , 4, int, array));
      }

      it("contains only non-even values") {
        expect(arr to all_be( %2 != , 0, int, array));
      }

    }

    context("tests fail") {

      expect(to_fail);

      context("a negative number is added to the array [..., -1]") {
        array_push_back(arr, &(int){-1});

        it("contains only positive values") {
          expect(arr to all(be_positive, int, array));
        }

        it("wants ONLY values that are not positive") {
          expect(arr to all(not be_positive, int, array));
        }
      }

      it("wants values only within 2 of 4") {
        expect(arr to all(be_within(2 of 4), int, array));
      }

      it("wants values that are not all within 2 of 5") {
        expect(arr to not all(be_within(2 of 5), int, array));
      }

      it("wants only values which are not within 2 of 9") {
        expect(arr to all(not be_within(2 of 9), int, array));
      }

      it("wants at least one value within 2 of 10") {
        expect(arr to not all(not be_within(2 of 10), int, array));
      }

      it("checks that all numbers are over 12") {
        expect(arr to all_be( > , 12, int, array));
      }

      it("asks for not all numbers to be less than 10") {
        expect(arr to not all_be( < , 10, int, array));
      }

      it("contains at least one value equal to 4") {
        expect(arr to not all_be( != , 4, int, array));
      }

      it("wants at least one even value") {
        expect(arr to not all_be(% 2 != , 0, int, array));
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

#ifdef _MSC_VER
#pragma warning ( pop )
#endif
