#include <stdlib.h>
#include <stdbool.h>
#include <check.h>

#include "../check_check.h"

#include "../../chemistry/chemistry.h"

chemical_system *sys;

void chemistry_setup(void) {
    sys = make_chemical_system(3);

    for (int j = 0; j < 3; j++ ) {
        for (int i = 0; i < ELEMENT_COUNT; i++) {
            sys->reactions[j].input.elements[i] = -1;
            sys->reactions[j].output.elements[i] = -1;
        }
    }
    sys->reactions[0].input.elements[fire] = 1;
    sys->reactions[0].output.elements[fire] = 0;
    sys->reactions[0].output.elements[earth] = 1;

    sys->reactions[1].input.elements[earth] = 1;
    sys->reactions[1].output.elements[earth] = 0;
    sys->reactions[1].output.elements[water] = 1;

    sys->reactions[2].input.elements[air] = 1;
    sys->reactions[2].output.elements[wood] = 10;
};

void chemistry_teardown(void) {
    destroy_chemical_system(sys);
};

START_TEST(test_apply_reaction) {
    reaction rec;

    for (int i = 0; i < ELEMENT_COUNT; i++) {
        rec.input.elements[i] = -1;
        rec.output.elements[i] = -1;
    }

    rec.input.elements[fire] = 1;
    rec.output.elements[fire] = 0;
    rec.output.elements[earth] = 1;

    constituents *input = make_constituents();
    input->elements[fire] = 2;
    input->elements[earth] = 3;
    input->elements[water] = 4;
    input->elements[air] = 5;
    input->elements[wood] = 6;

    ck_assert(apply_reaction(&rec, input, NULL));

    ck_assert_int_eq(input->elements[fire], 1);
    ck_assert_int_eq(input->elements[earth], 4);
    ck_assert_int_eq(input->elements[water], 4);
    ck_assert_int_eq(input->elements[air], 5);
    ck_assert_int_eq(input->elements[wood], 6);

    ck_assert(apply_reaction(&rec, input, NULL));
    ck_assert_int_eq(input->elements[fire], 0);
    ck_assert_int_eq(input->elements[earth], 5);

    ck_assert(!apply_reaction(&rec, input, NULL));

    destroy_constituents(input);
} END_TEST

START_TEST(test_react) {
    constituents *thing = make_constituents();
    thing->elements[fire] = 1;

    react(sys, thing, NULL);
    ck_assert(!thing->stable);
    ck_assert_int_eq(thing->elements[fire], 0);
    ck_assert_int_eq(thing->elements[earth], 1);
    ck_assert_int_eq(thing->elements[water], 0);
    ck_assert_int_eq(thing->elements[air], 0);
    ck_assert_int_eq(thing->elements[wood], 0);

    react(sys, thing, NULL);
    ck_assert(!thing->stable);
    ck_assert_int_eq(thing->elements[fire], 0);
    ck_assert_int_eq(thing->elements[earth], 0);
    ck_assert_int_eq(thing->elements[water], 1);
    ck_assert_int_eq(thing->elements[air], 0);
    ck_assert_int_eq(thing->elements[wood], 0);

    react(sys, thing, NULL);
    ck_assert(thing->stable);

    destroy_constituents(thing);
} END_TEST

Suite * make_chemistry_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("Chemistry");

    /* Core test case */
    tc_core = tcase_create("Core");

    tcase_add_checked_fixture(tc_core, chemistry_setup, chemistry_teardown);
    tcase_add_test(tc_core, test_apply_reaction);
    tcase_add_test(tc_core, test_react);
    suite_add_tcase(s, tc_core);

    return s;
}
