#ifndef SIMULATION_H
#define SIMULATION_H

#include "min_heap.h"

struct agent {
    void *state;
    int (*next_firing)(void *);
    void (*fire)(void *);
};

struct simulation {
    struct agent* agents;
    int num_agents;
    mheap *queue;
};

void sync_simulation(struct simulation *sim, int stop_time);

#endif
