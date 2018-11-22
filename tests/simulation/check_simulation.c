#include <stdbool.h>
#include <stdlib.h>
#include <check.h>

#include "../../helpers.h"
#include "../check_check.h"

#include "../../simulation/simulation.h"

void simulation_setup(void) {
};

void simulation_teardown(void) {
};

int mock_next_firing(void* st, enum sensory_events **ilist) {
    float rate = 1.0;
    float r = frand();
    return (1 - powf(M_E, -rate*r)) * 1000;
}

void mock_fire(void* st) {
    *(int*)st += 1;
}

START_TEST(development_target) {
    struct simulation sim;
    sim.num_agents = 10000;
    sim.agents = malloc(sim.num_agents*sizeof(struct agent));
    sim.queue = make_mheap();

    for (int i = 0; i < sim.num_agents; i++) {
        sim.agents[i].state = malloc(sizeof(int));
        *(int*)sim.agents[i].state = 0;
        sim.agents[i].next_firing = mock_next_firing;
        sim.agents[i].fire = mock_fire;

        enum sensory_events *ilist;
        int next_firing = sim.agents[i].next_firing(&sim.agents[i].state, &ilist);
        mheap_push(sim.queue, &sim.agents[i], next_firing);
    }

    sync_simulation(&sim, 3000);

    destroy_mheap(sim.queue);
    for (int i = 0; i < sim.num_agents; i++) {
        free((void*)sim.agents[i].state);
    }
    free((void*)sim.agents);


} END_TEST

Suite * make_simulation_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("Simulation");

    /* Core test case */
    tc_core = tcase_create("Core");

    tcase_add_checked_fixture(tc_core, simulation_setup, simulation_teardown);
    tcase_add_test(tc_core, development_target);
    suite_add_tcase(s, tc_core);

    return s;
}
