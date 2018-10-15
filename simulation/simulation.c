#include <time.h>
#include <limits.h>
#include <stdlib.h>
#include <math.h>

#include "simulation.h"

struct simulation* make_simulation(void) {
    struct simulation *sim;
    sim = malloc(sizeof(struct simulation));
    sim->agents = make_vector(sizeof(struct agent));
    sim->queue = make_mheap(sizeof(struct event));
    sim->current_clock = 0;
    return sim;
}

void destroy_simulation(struct simulation *sim) {
    struct event *e;
    int clock;

    destroy_vector(sim->agents);
    while (sim->queue->length > 0) {
        mheap_pop(sim->queue, (void**)&e, &clock);
        free((void*)e);
    }
    destroy_mheap(sim->queue);
    free((void*)sim);
}

void simulation_push_agent(struct simulation *sim, struct agent *a) {
    vector_push(sim->agents, (void*)a);
}

void schedule_event(struct simulation *sim, struct agent* a, int clock) {
    for (int i = 0; i < SENSORY_EVENT_COUNT; i++) a->listeners[i].handler = NULL;

    int next_firing = a->next_firing(a->state, a->listeners);
    // FIXME: This doesn't guarentee that it won't overflow
    if (next_firing < INT_MAX) next_firing += clock;
    struct event *e = malloc(sizeof(struct event));
    e->valid = true;
    e->agent = a;
    for (int i = 0; i < SENSORY_EVENT_COUNT; i++) a->listeners[i].owner = e;

    mheap_push(sim->queue, (void*)e, next_firing);
}

void sync_simulation(struct simulation *sim, int stop_time) {
    struct event *e;
    int event_count = 0;

    if (sim->queue->length > 0) {
        mheap_peek(sim->queue, (void**)&e, &sim->current_clock);
        while (sim->current_clock <= stop_time) {
            event_count++;
            mheap_pop(sim->queue, (void**)&e, &sim->current_clock);
            if (e->valid) {
                e->agent->fire(e->agent->state);
                schedule_event(sim, e->agent, sim->current_clock);
            }
            free((void*)e);
            mheap_peek(sim->queue, (void**)&e, &sim->current_clock);
        }
    }
}

void simulation_call_event_handler(struct simulation *sim, struct event_listener *listener) {
    bool do_cancel = listener->handler((void*)listener->owner->agent->state);

    if (do_cancel) {
        listener->owner->valid = false;
        schedule_event(sim, listener->owner->agent, sim->current_clock);
    }
}
