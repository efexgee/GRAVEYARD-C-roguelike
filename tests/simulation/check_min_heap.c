#include <stdbool.h>
#include <check.h>

#include "../check_check.h"

#include "../../simulation/min_heap.h"

void mheap_setup(void) {
};

void mheap_teardown(void) {
};

START_TEST(development_target) {
    int data = 42;
    int data2 = 43;
    mheap *h = make_mheap();
    mheap_push(h, &data, 10);
    mheap_push(h, &data, 9);
    mheap_push(h, &data2, 8);
    mheap_push(h, &data, 20);
    int *new_data;
    int priority;

    mheap_peek(h, (void*)&new_data, &priority);
    ck_assert(*new_data == 43);
    ck_assert(priority == 8);
    priority = -1;
    new_data = NULL;

    mheap_pop(h, (void*)&new_data, &priority);
    ck_assert(*new_data == 43);
    ck_assert(priority == 8);
    priority = -1;
    new_data = NULL;

    mheap_pop(h, (void*)&new_data, &priority);
    ck_assert(*new_data == 42);
    ck_assert(priority == 9);
    priority = -1;
    new_data = NULL;

    mheap_pop(h, (void*)&new_data, &priority);
    ck_assert(*new_data == 42);
    ck_assert(priority == 10);
    priority = -1;
    new_data = NULL;

    mheap_pop(h, (void*)&new_data, &priority);
    ck_assert(*new_data == 42);
    ck_assert(priority == 20);

    destroy_mheap(h);
} END_TEST

Suite * make_mheap_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("Min Heap");

    /* Core test case */
    tc_core = tcase_create("Core");

    tcase_add_checked_fixture(tc_core, mheap_setup, mheap_teardown);
    tcase_add_test(tc_core, development_target);
    suite_add_tcase(s, tc_core);

    return s;
}
