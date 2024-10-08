#include <check.h>
#include <stdlib.h>

#include "str.h"

START_TEST(test_single_str_replace) {
  char* target = "put {} in this place right {}{}{} here {}";

  char* result = str_replace(target, "{}", "this");

  ck_assert_str_eq(
    result,
    "put this in this place right thisthisthis here this");

  free(result);
}
END_TEST

START_TEST(test_single_str_replace_target_null) {
  char* result = str_replace(NULL, "%", "arg");

  ck_assert_ptr_null(result);
}
END_TEST

START_TEST(test_single_str_replace_placeholder_null) {
  char* target = "switch me out";

  char* result = str_replace(target, NULL, "arg");

  ck_assert_str_eq(result, target);
  ck_assert_ptr_ne(result, target);

  free(result);
}
END_TEST

START_TEST(test_single_str_replace_replacement_null) {
  char* target = "switch % me % out";

  char* result = str_replace(target, "%", NULL);

  ck_assert_str_eq(result, "switch  me  out");
}
END_TEST

Suite* str_replace_suite() {
  Suite* suite = suite_create("string replacing");

  TCase* tc_str_replace = tcase_create("single string");
  tcase_add_test(tc_str_replace, test_single_str_replace);
  tcase_add_test(tc_str_replace, test_single_str_replace_target_null);
  tcase_add_test(tc_str_replace, test_single_str_replace_placeholder_null);
  tcase_add_test(tc_str_replace, test_single_str_replace_replacement_null);
  suite_add_tcase(suite, tc_str_replace);

  return suite;
}

int main() {
  Suite* suite = str_replace_suite();
  SRunner* runner = srunner_create(suite);

  int number_failed = 0;
  srunner_run_all(runner, CK_NORMAL);
  number_failed = srunner_ntests_failed(runner);
  srunner_free(runner);

  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
