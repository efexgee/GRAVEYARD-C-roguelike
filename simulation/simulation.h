#ifndef SIMULATION_H
#define SIMULATION_H

#include <stdbool.h>

#include "min_heap.h"
#include "vector.h"

enum sensory_events {
    VISION_CHANGE = 0,
    DAMAGE,
};
#define SENSORY_EVENT_COUNT DAMAGE+1


struct agent;

struct event {
    bool valid;
    struct agent *agent;
};

struct event_listener {
    struct event *owner;
    bool (*handler)(void *);
};

struct agent {
    void *state;
    int (*next_firing)(void *, struct event_listener *listeners);
    void (*fire)(void *);
    struct event_listener *listeners;
};


struct simulation {
    vector *agents;
    mheap *queue;
    int current_clock;
};

struct simulation* make_simulation(void);
void destroy_simulation(struct simulation *sim);

void simulation_call_event_handler(struct simulation *sim, struct event_listener *listener);

void simulation_push_agent(struct simulation *sim, struct agent *a);
void schedule_event(struct simulation *sim, struct agent* a, int clock);
void sync_simulation(struct simulation *sim, int stop_time);

#endif
