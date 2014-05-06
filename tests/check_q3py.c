#include <stdlib.h>
#include <check.h>
#include "../src/q3py_p.h"


START_TEST(test_dllEntry)
{
	dllEntry(NULL);
}
END_TEST


START_TEST(test_vmMain)
{
	int cmd = 0, dummyArg = 0;
	intptr_t res = vmMain(cmd, dummyArg, dummyArg, dummyArg, dummyArg,
			dummyArg, dummyArg, dummyArg, dummyArg, dummyArg, dummyArg,
			dummyArg, dummyArg);
}
END_TEST


Suite* q3py_suite(void) {
	Suite *suite = suite_create("q3py");

	TCase *tc_core = tcase_create("Core");

	tcase_add_test(tc_core, test_dllEntry);
	tcase_add_test(tc_core, test_vmMain);

	suite_add_tcase(suite, tc_core);

	return suite;
}


int main(void) {
	int number_failed = 0;

	Suite *suite = q3py_suite();
	SRunner *suite_runner = srunner_create(suite);
	srunner_run_all(suite_runner, CK_NORMAL);

	number_failed = srunner_ntests_failed(suite_runner);
	srunner_free(suite_runner);
	return (0 == number_failed) ? EXIT_SUCCESS : EXIT_FAILURE;
}

