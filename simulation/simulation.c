#include <time.h>
#include <stdlib.h>
#include <math.h>

#include "simulation.h"

void sync_simulation(struct simulation *sim, int stop_time) {
    int clock;
    struct agent *a;

    if (sim->queue->length > 0) {
        mheap_peek(sim->queue, (void*)&a, &clock);
        while (clock < stop_time) {
            mheap_pop(sim->queue, (void*)&a, &clock);
            a->fire(a->state);
            int next_firing = clock + a->next_firing(a->state);
            mheap_push(sim->queue, a, next_firing);
            mheap_peek(sim->queue, (void*)&a, &clock);
        }
    }
}
