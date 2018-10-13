#include <time.h>
#include <stdlib.h>
#include <math.h>

#include "simulation.h"

void schedule_event(struct simulation *sim, struct agent* a, int clock) {
    enum sensory_events *invalidation_list = NULL;

    int next_firing = clock + a->next_firing(a->state, &invalidation_list);
    mheap_push(sim->queue, (void*)a, next_firing);
}

void sync_simulation(struct simulation *sim, int stop_time) {
    int clock;
    struct agent *a;
    int event_count = 0;

    if (sim->queue->length > 0) {
        mheap_peek(sim->queue, (void**)&a, &clock);
        while (clock <= stop_time) {
            event_count++;
            mheap_pop(sim->queue, (void**)&a, &clock);
            a->fire(a->state);
            schedule_event(sim, a, clock);
            mheap_peek(sim->queue, (void**)&a, &clock);
        }
    }
}
