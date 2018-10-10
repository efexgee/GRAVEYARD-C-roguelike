#include <stdlib.h>
#include <check.h>

#include "check_check.h"

int main(void)
{
    int number_failed;
    Suite *s;
    SRunner *sr;

    sr = srunner_create(make_mheap_suite());
    srunner_add_suite(sr, make_simulation_suite());
    srunner_add_suite(sr, make_chemistry_suite());

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
