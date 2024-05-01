#include "str.h"

#define CSPEC_CUSTOM_TYPES                                                    \
  StringRange: "StringRange", StringRange*: "StringRange*", String: "String", //

#include "cspec.h"

describe(str_range) {

  it("creates a range from a literal") {
    StringRange range = R("literal string range");

    expect(range.length, == , 20u);
    expect(range.size, == , 20u);
  }

#ifdef __GNUC__
#pragma GCC diagnostic push
  // warns that the length and size are a union, which is the point
#pragma GCC diagnostic ignored "-Waddress"
#endif
  it("validates that length and size are union aliases") {
    StringRange range = R("literal string range");

    expect(&range.length, == , &range.size, csBool);
  }
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

  it("creates a range in a local fixed-size char array") {
    char c_str[] = "This is a stack string";
    StringRange range = R(c_str);

    expect(range.begin == c_str);
    expect(range.length, == , 22u);
  }

  it("creates a range within the given c-string string") {
    char* c_str = "This is a c-string";
    StringRange range = str_range(c_str);

    expect(range.begin == c_str);
    expect(range.length, == , 18u);
  }

  it("creates a range using the _s version that specifies length") {
    char* c_str = "This is a c-string";
    StringRange range = str_range_s(c_str, 4);

    expect(range.begin == c_str);
    expect(range.length, == , 4u);
    expect(range to match(str_eq, "This"));
  }

  expect(malloc_count, == , 0);

}

describe(str_new) {
  String subject = NULL;

  char c_str[] = "This is a c-string";

  it("makes a copy of the c_string using malloc") {
    subject = str_new(c_str);
    expect(str_eq(subject, c_str));
  }

  it("makes a copy using the _s version") {
    subject = str_new_s(c_str, 4);
    expect(str_eq(subject, "This"));
  }

  it("ensures the direct and range pointers are the same/shared in union") {
    subject = str_new(c_str);
    expect(&subject->begin == &subject->range.begin);
    expect(&subject->size == &subject->range.size);
    expect(&subject->size == &subject->length);
  }

  if (subject) {
    expect(subject->begin != c_str);
    expect(malloc_count == 1);
    str_delete(&subject);
  }

}

describe(str_copy) {
  String subject = NULL;

  char c_str[] = "This is a string";
  char* p_str = c_str;
  StringRange range = R(c_str);

  it("allocates a new copy of the string directly from the c string") {
    subject = str_copy(c_str);
    expect(subject to match(str_eq, p_str));
    expect(subject->begin != c_str);
  }

  it("allocates a new copy of the string from a range") {
    subject = str_copy(range);
    expect(subject to match(str_eq, range));
    expect(subject->begin != range.begin);
  }

  it("allocates a new copy of the string from another dynamic string") {
    String str = str_new(c_str);
    subject = str_copy(str);
    expect(subject to match(str_eq, str));
    str_delete(&str);
  }

  if (subject) {
    expect(subject->begin != range.begin);
    str_delete(&subject);
  }

}

describe(str_from_bool) {
  String subject = NULL;

  it("gets a true value from a bool") {
    subject = str_from_bool(TRUE);
    expect(subject == str_true);
    expect(subject to match(str_eq, "true"));
  }

  it("gets a false value from a bool") {
    subject = str_from_bool(FALSE);
    expect(subject == str_false);
    expect(subject to match(str_eq, "false"));
  }

  if (subject) {
    expect(malloc_count == 0);
  }

}

describe(str_from_int) {
  String subject = NULL;

  it("gets an int from 0") {
    subject = str_from_int(0);
    expect(str_eq(subject->range, R("0")));
  }

  it("converts from a positive integer") {
    subject = str_from_int(7);
    expect(str_eq(subject->range, R("7")));
  }

  it("converts from a negative integer") {
    subject = str_from_int(-4);
    expect(str_eq(subject->range, R("-4")));
  }

  it("tries a bigger number") {
    subject = str_from_int(1746);
    expect(str_eq(subject->range, R("1746")));
  }

  it("tries a bigger negative number") {
    subject = str_from_int(-84756);
    expect(str_eq(subject->range, R("-84756")));
  }

  if (subject) {
    str_delete(&subject);
  }

}

describe(str_from_float) {
  String subject = NULL;

  it("gets a float from 0") {
    subject = str_from_float(0.0f);
    expect(str_eq(subject->range, R("0")));
  }

  it("gets pretty exact values from whole numbers") {
    subject = str_from_float(4.0f);
    expect(str_eq(subject->range, R("4")));
  }

  it("gets pretty exact values from negative whole numbers") {
    subject = str_from_float(-9.0f);
    expect(str_eq(subject->range, R("-9")));
  }

  it("gets decimal values") {
    subject = str_from_float(2.73f);
    expect(str_eq(subject->range, R("2.73")));
  }

  if (subject) {
    str_delete(&subject);
  }

}

describe(str_delete) {

  it("frees the memory and zeroes the pointer") {
    String subject = str_new("Test string");
    expect(subject != NULL);
    expect(malloc_count == 1);
    str_delete(&subject);
    expect(subject == NULL);
    expect(free_count == 1);
  }

  it("does a no-op when trying to free empty string") {
    String subject = str_empty;
    expect(subject != NULL);
    str_delete(&subject);
    expect(free_count == 0);
    expect(subject == NULL);
  }

  it("does a no-op when trying to free true string") {
    String subject = str_true;
    expect(subject != NULL);
    str_delete(&subject);
    expect(free_count == 0);
    expect(subject == NULL);
  }

  it("does a no-op when trying to free false string") {
    String subject = str_false;
    expect(subject != NULL);
    str_delete(&subject);
    expect(free_count == 0);
    expect(subject == NULL);
  }

  it("does a no-op when trying to free pointer to NULL") {
    String subject = NULL;
    str_delete(&subject);
    expect(free_count == 0);
  }

  it("does a no-op when trying to free NULL") {
    str_delete(NULL);
    expect(free_count == 0);
  }

}

describe(str_eq) {

  it("tests that two strings are bitwise equal") {
    char str_1[] = "String";
    char str_2[] = "String";

    // strings made on the stack shouldn't have the same address
    expect(&str_1[0] != &str_2[0]);

    StringRange range1 = str_range(str_1);
    StringRange range2 = str_range(str_2);

    expect(str_eq(range1, range2));
  }

  it("tests that two strings are not equal") {
    char str_1[] = "String";
    char str_2[] = "StrinG";

    // strings made on the stack shouldn't have the same address
    expect(&str_1[0] != &str_2[0]);

    StringRange range1 = str_range(str_1);
    StringRange range2 = str_range(str_2);

    expect(not str_eq(range1, range2));
  }

}

describe(str_starts_with) {
  StringRange range = R("This is a string");

  it("handles a basic true use case") {
    expect(str_starts_with(range, R("This")));
  }

  it("handles a basic false use case") {
    expect(not str_starts_with(range, R("Thos")));
  }

  it("is case sensitive") {
    expect(not str_starts_with(range, R("THIS")));
  }

  it("returns true given an empty string") {
    expect(str_starts_with(range, R("")));
  }

  it("returns true given the full string") {
    expect(str_starts_with(range, R("This is a string")));
  }

  it("returns false given more than the full string") {
    expect(not str_starts_with(range, R("This is a string with more")));
  }

}

describe(str_ends_with) {
  StringRange range = R("This is a string");

  it("handles a basic true use case") {
    expect(str_ends_with(range, R("string")));
  }

  it("handles a basic false use case") {
    expect(not str_ends_with(range, R("strong")));
  }

  it("is case sensitive") {
    expect(not str_ends_with(range, R("STRING")));
  }

  it("returns true given an empty string") {
    expect(str_ends_with(range, R("")));
  }

  it("returns true given the full string") {
    expect(str_ends_with(range, R("This is a string")));
  }

  it("returns false given more than the full string") {
    expect(not str_ends_with(range, R("And This is a string")));
  }

}

describe(str_contains) {
  StringRange range = R("This is a string");

  it("handles a basic true use case") {
    expect(range to match(str_contains, "is a"));
  }

  it("handles a basic false use case") {
    expect(range to not match(str_contains, "not in"));
  }

  it("is case sensitive") {
    expect(range to not match(str_contains, "IS A"));
  }

  it("returns true given an empty string") {
    expect(range to match(str_contains, ""));
  }

  it("returns true given the full string") {
    expect(range to match(str_contains, "This is a string"));
  }

  it("returns false given more than the full string") {
    expect(range to not match(str_contains, "This is a string."));
  }

}

describe(str_index_of) {
  StringRange range = R("This is a string");
  StringRange is = R("is");

  it("finds an index") {
    expect(str_index_of(range, is, 0), == , 2u);
  }

  it("fails to find a substring that isn't present") {
    expect(str_index_of(range, R("Not present"), 0), == , range.size);
  }

  it("begins the search from a given position") {
    expect(str_index_of(range, is, 3), == , 5u);
  }

  it("fails to find a word in the string that starts before the start_pos") {
    expect(str_index_of(range, is, 6), == , range.size);
  }

  it("fails to find anything if the start pos is after the end of the string") {
    expect(str_index_of(range, is, 99), == , range.size);
  }

}

describe(str_find) {
  StringRange range = R("This is a string");

  it("finds an index") {
    StringRange is = R("is");
    expect(istr_find(range, is), == , 2u);
  }

  it("fails to find a substring that isn't present") {
    StringRange np = R("not present");
    expect(istr_find(range, np), == , range.size);
  }

  it("can find the first word in the string") {
    StringRange This = R("This");
    expect(istr_find(range, This), == , 0u);
  }

  expect(malloc_count == 0);

}

describe(str_substring) {

  StringRange range = R("This is a string");

  context("with only ranges, no allocations") {

    it("gets a substring from 0 to 0") {
      StringRange subject = str_substring(range, 0, 0);
      expect(subject to match(str_eq, str_empty));
    }

    it("gets substring of whole string (using default argument") {
      StringRange subject = str_substring(range, 0);
      expect(subject to match(str_eq, range));
    }

    it("gets a partial substring from the beginning") {
      StringRange subject = str_substring(range, 0, 4);
      expect(subject to match(str_eq, "This"));
      //expect(str_eq to not succeed_with(subject, "This"));
    }

    it("gets a substring starting partway in the string") {
      StringRange subject = str_substring(range, 5, 9);
      expect(subject to match(str_eq, "is a"));
    }

    it("uses a negative offset for the start of the substring") {
      StringRange subject = str_substring(range, -6);
      expect(subject to match(str_eq, "string"));
    }

    it("uses negative offsets for the start and end") {
      StringRange subject = str_substring(range, -8, -3);
      expect(subject to match(str_eq, "a str"));
    }

    it("has a string start past the end") {
      StringRange subject = str_substring(range, 20);
      expect(subject to match(str_eq, str_empty));
    }

    it("has a string end before the beginning") {
      StringRange subject = str_substring(range, 5, 3);
      expect(subject to match(str_eq, str_empty));
    }

    it("has a string end before the beginning") {
      StringRange subject = str_substring(range, 1, 0);
      expect(subject to match(str_eq, str_empty));
    }

    it("can accept a basic c-string") {
      StringRange subject = str_substring("This is a string", -6);
      expect(subject to match(str_eq, "string"));
    }

    expect(malloc_count == 0);

  }

  it("can accept a dynamic String") {
    String str = str_copy(range);
    StringRange subject = str_substring(str, -6);
    expect(subject to match(str_eq, "string"));
    str_delete(&str);
  }

}

describe(str_trim) {

  StringRange range = R("\t  String with extra spaces   \n");
  StringRange subject = str_empty->range;
  StringRange expected = str_empty->range;

  it("trims all leading and trailing spaces") {
    subject = str_trim(range);
    expected = R("String with extra spaces");
  }

  it("trims all leading spaces") {
    subject = str_trim_start(range);
    expected = R("String with extra spaces   \n");
  }

  it("trims all trailing spaces") {
    subject = str_trim_end(range);
    expected = R("\t  String with extra spaces");
  }

  it("Does nothing to a string with no leading or trailing spaces") {
    subject = str_trim(str_true);
    expected = R("true");
  }

  after{
    expect(subject to match(str_eq, expected));
    expect(malloc_count == 0);
  }

}

describe(str_split) {

  StringRange range = R("This is, a collection, of strings");
  Array result = NULL;

  it("performs a basic split on commas") {
    result = str_split(range, ",");
    expect(result->size, == , 3u);

    StringRange expected[3] = { R("This is"), R(" a collection"), R(" of strings") };
    expect(result to all(str_eq, expected[n], StringRange, array));
  }

  it("performs a multi-char split") {
    result = str_split(range, ", ");
    expect(result->size, == , 3u);

    StringRange expected[3] = { R("This is"), R("a collection"), R("of strings") };
    expect(result to all(str_eq, expected[n], StringRange, array));
  }

  it("splits on an empty string") {
    result = str_split(range, str_empty->range);
    expect(result->size, == , range.size);

    expect(result to all(1 == str_size, StringRange, array));
    expect(result to all(str_eq, str_substring(range, n, n+1), StringRange, array));
  }

  it("tries to split on a delimiter that isn't present") {
    result = str_split(range, "NOT_INCLUDED");
    expect(result->size, == , 1u);
    expect(result to all(str_eq, range, StringRange, array));
  }

  it("tries to split with a delimiter that is too big") {
    result = str_split(range, "This is, a collection, of strings, but more");
    expect(result->size, == , 1u);
    expect(result to all(str_eq, range, StringRange, array));
  }

  it("splits using the same string as the delimiter") {
    result = str_split(range, range);
    expect(result->size, == , 2u);
    expect(result to all(str_eq, str_empty->range, StringRange, array));
  }

  if (result) {
    array_delete(&result);
  }

}

describe(str_join) {

  Array tokens = NULL; 
  String result = NULL;

  context("basic set of StringRange tokens to form a sentence") {

    StringRange range = R("These are the test tokens");
    tokens = str_split(range, " ");

    it("recreates the original string") {
      result = str_join(" ", tokens);
      expect(result to match(str_eq, range));
    }

    it("puts together the string without spaces ") {
      result = str_join("", tokens);
      expect(result to match(str_eq, "Thesearethetesttokens"));
    }

    it("gives a multi-char deliminiter between the tokens") {
      result = str_join(" - ", tokens);
      expect(result to match(str_eq, "These - are - the - test - tokens"));
    }

  }

  context("given an empty array of String") {

    tokens = array_new_reserve(String, 0);

    it("produces an empty string") {
      result = str_join("!", tokens);
      expect(result->size, == , 0u);
      expect(result == str_empty);
    }

  }

  context("can join an array of dynamic String objects") {

    String str1 = str_new("Str 1");
    String str2 = str_new("Str 2");
    String str3 = str_new("Str 3");
    tokens = array_new_reserve(String, 3);
    array_push_back(tokens, &str1);
    array_push_back(tokens, &str2);
    array_push_back(tokens, &str3);

    it("properly joins the strings") {
      result = str_join(", ", tokens);
      expect(result->range to match(str_eq, "Str 1, Str 2, Str 3"));
    }

    it("can mix String and StringRange* in the same array") {
      StringRange range = R("Range 4");
      StringRange* rp = &range;
      array_push_back(tokens, &rp);

      result = str_join("|", tokens);
      expect(result->range to match(str_eq, "Str 1|Str 2|Str 3|Range 4"));
    }

    str_delete(&str1);
    str_delete(&str2);
    str_delete(&str3);
  }

  if (result) str_delete(&result);
  if (tokens) array_delete(&tokens);

}

describe(str_concat) {

  String result = NULL;

  it("joins two strings together") {
    result = str_concat("LHS ", "RHS");
    expect(to_pass(str_eq, result, "LHS RHS"));
  }

  it("still copies the string if joining with an empty (rhs)") {
    char* lhs = "LHS";
    result = str_concat(lhs, str_empty);
    expect(to_pass(str_eq, result, "LHS"));
    expect(result->begin, != , lhs);
  }

  it("still copies the string if joining with an empty (lhs)") {
    char* rhs = "RHS";
    result = str_concat(str_empty, rhs);
    expect(to_pass(str_eq, result, "RHS"));
    expect(result->begin, != , rhs);
  }

  it("makes an empty string when both are empty") {
    result = str_concat(str_empty, str_empty);
    expect(to_pass(str_eq, result, str_empty));
    expect(result, == , str_empty);
  }

  if (result) str_delete(&result);

}

test_suite(tests_string) {
  test_group(str_range),
  test_group(str_new),
  test_group(str_copy),
  test_group(str_from_bool),
  test_group(str_from_int),
  test_group(str_from_float),
  test_group(str_delete),
  test_group(str_eq),
  test_group(str_starts_with),
  test_group(str_ends_with),
  test_group(str_contains),
  test_group(str_index_of),
  test_group(str_find),
  test_group(str_substring),
  test_group(str_trim),
  test_group(str_split),
  test_group(str_join),
  test_group(str_concat),
  test_suite_end
};
