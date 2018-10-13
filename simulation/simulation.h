#ifndef SIMULATION_H
#define SIMULATION_H

#include "min_heap.h"

enum sensory_events {
    VISION_CHANGE = 0,
    DAMAGE,
};
#define SENSORY_EVENT_COUNT DAMAGE+1


struct agent {
    void *state;
    int (*next_firing)(void *, enum sensory_events **invalidation_list);
    void (*fire)(void *);
};

struct simulation {
    struct agent* agents;
    int num_agents;
    mheap *queue;
    
};

void schedule_event(struct simulation *sim, struct agent* a, int clock);
void sync_simulation(struct simulation *sim, int stop_time);

#endif
