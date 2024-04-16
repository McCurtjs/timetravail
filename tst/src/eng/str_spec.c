#include "str.h"

#include "cspec.h"

describe(str_range) {

	it("creates a range off a literal") {
		StringRange range = R("literal string range");

		expect(range.length, == , 20, size_t);
	}

	it("creates a range in a local fixed-size char array") {
		char c_str[] = "This is a stack string";
		StringRange range = R(c_str);

		expect(range.begin == c_str);
		expect(range.length, == , 22, size_t);
	}

	it("creates a range within the given c-string string") {
		char* c_str = "This is a c-string";
		StringRange range = str_range(c_str);

		expect(range.begin == c_str);
		expect(range.length, == , 18, size_t);
	}

	it("creates a range using the _s version that specifies length") {
		char* c_str = "This is a c-string";
		StringRange range = str_range_s(c_str, 4);

		expect(range.begin == c_str);
		expect(range.length, == , 4, size_t);
	}

	expect(malloc_count == 0);

}

describe(str_new) {
	String subject = NULL;

	char c_str[] = "This is a c-string";

	it("makes a copy of the c_string using malloc") {
		subject = str_new(c_str);
		expect(str_eq(subject->range, R(c_str)));
	}

	it("makes a copy using the _s version") {
		subject = str_new_s(c_str, 4);
		expect(str_eq(subject->range, R("This")));
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

	StringRange range = R("This is a string");

	it("allocates a new copy of the string") {
		subject = str_copy(range);
		expect(str_eq(subject->range, range));
		expect(subject->begin != range.begin);
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
	}

	it("gets a false value from a bool") {
		subject = str_from_bool(FALSE);
		expect(subject == str_false);
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
		expect(str_contains(range, R("is a")));
	}

	it("handles a basic false use case") {
		expect(not str_contains(range, R("not in")));
	}

	it("is case sensitive") {
		expect(not str_contains(range, R("IS A")));
	}

	it("returns true given an empty string") {
		expect(str_contains(range, R("")));
	}

	it("returns true given the full string") {
		expect(str_contains(range, R("This is a string")));
	}

	it("returns false given more than the full string") {
		expect(not str_contains(range, R("This is a string.")));
	}

}

describe(str_index_of) {
	StringRange range = R("This is a string");
	StringRange is = R("is");

	it("finds an index") {
		expect((int)str_index_of(range, is, 0), == , 2);
	}

	it("fails to find a substring that isn't present") {
		expect((int)str_index_of(range, R("Not present"), 0));
	}

	it("begins the search from a given position") {
		expect((int)str_index_of(range, is, 3), == , 5);
	}

	it("fails to find a word in the string that starts before the start_pos") {
		expect((int)str_index_of(range, is, 6), == , 16);
	}

	it("fails to find anything if the start pos is after the end of the string") {
		expect((int)str_index_of(range, is, 99), == , 16);
	}

}

describe(str_find) {
	StringRange range = R("This is a string");
	StringRange is = R("is");

	it("finds an index") {
		expect((int)str_find(range, is), == , 2);
	}

	it("fails to find a substring that isn't present") {
		expect((int)str_find(range, R("Not present")), == , 16);
	}

	it("can find the first word in the string") {
		expect((int)str_find(range, R("This")), == , 0);
	}

	expect(malloc_count == 0);

}

describe(str_substring) {

	StringRange range = R("This is a string");

	it("gets a substring from 0 to 0") {
		StringRange subject = str_substring(range, 0, 0);
		expect(str_eq(subject, str_empty->range));
	}

	it("gets substring of whole string (using default argument") {
		StringRange subject = str_substring(range, 0);
		expect(str_eq(subject, range));
	}

	it("gets a partial substring from the beginning") {
		StringRange subject = str_substring(range, 0, 4);
		StringRange _This = R("This");
		expect(str_eq(subject, _This));
	}

	it("gets a substring starting partway in the string") {
		StringRange subject = str_substring(range, 5, 9);
		StringRange _is_a = R("is a");
		expect(str_eq(subject, _is_a));
	}

	it("uses a negative offset for the start of the substring") {
		StringRange subject = str_substring(range, -6);
		StringRange _string = R("string");
		expect(str_eq(subject, _string));
	}

	it("uses negative offsets for the start and end") {
		StringRange subject = str_substring(range, -8, -3);
		StringRange _a_str = R("a str");
		expect(str_eq(subject, _a_str));
	}

	it("has a string start past the end") {
		StringRange subject = str_substring(range, 20);
		expect(str_eq(subject, str_empty->range));
	}

	it("has a string end before the beginning") {
		StringRange subject = str_substring(range, 5, 3);
		expect(str_eq(subject, str_empty->range));
	}

	it("has a string end before the beginning") {
		StringRange subject = str_substring(range, 1, 0);
		expect(str_eq(subject, str_empty->range));
	}

	expect(malloc_count == 0);

}

describe(str_trim) {

	StringRange range = R("\t  String with extra spaces   \n");

	it("trims all leading and trailing spaces") {
		StringRange subject = str_trim(range);
		StringRange expected = R("String with extra spaces");
		expect(str_eq(subject, expected));
	}

	it("trims all leading spaces") {
		StringRange subject = str_trim_start(range);
		StringRange expected = R("String with extra spaces   \n");
		expect(str_eq(subject, expected));
	}

	it("trims all trailing spaces") {
		StringRange subject = str_trim_end(range);
		StringRange expected = R("\t  String with extra spaces");
		expect(str_eq(subject, expected));
	}

	it("Does nothing to a string with no leading or trailing spaces") {
		StringRange subject = str_trim(str_true->range);
		expect(str_eq(subject, str_true->range));
	}

	expect(malloc_count == 0);

}

test_suite_begin(tests_string) {
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
	test_suite_end
};
