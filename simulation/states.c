#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "min_heap.h"

struct agent {
    void *state;
    int (*activation)(void *);
    float (*probability)(void *);
    void (*fire)(void *);
};

struct simulation {
    struct agent* agents;
    int num_agents;
    mheap *queue;
};

struct state {
    int counter;
};

int activation(void* st) {
    return 0;
}

float probability(void* st) {
    //return 1.0 - st->counter/10.0;
    return 1.0;
}

void fire(void* st) {
    *(int*)st += 1;
}

int draw(float prob) {
    float r = ((float)rand()) / RAND_MAX;
    return (1 - powf(M_E, -prob*r)) * 1000;
}

int main(void) {
    srand(time(NULL));
    struct simulation sim;
    sim.num_agents = 10000;
    sim.agents = malloc(sim.num_agents*sizeof(struct agent));
    sim.queue = make_mheap();

    for (int i = 0; i < sim.num_agents; i++) {
        sim.agents[i].state = malloc(sizeof(int));
        *(int*)sim.agents[i].state = 0;
        sim.agents[i].activation = activation;
        sim.agents[i].probability = probability;
        sim.agents[i].fire = fire;

        int next_firing = sim.agents[i].activation(&sim.agents[i].state) + sim.agents[i].probability(&sim.agents[i].state);
        mheap_push(sim.queue, &sim.agents[i], next_firing);
    }

    int clock = 0;
    int event_count = 0;

    while (clock < 30000) {
        struct agent *a;
        mheap_pop(sim.queue, (void*)&a, &clock);
        a->fire(a->state);
        int next_firing = clock + a->activation(a->state) + draw(a->probability(a->state));
        mheap_push(sim.queue, a, next_firing);
        event_count++;
    }

    printf("Num agents: %d, Num events: %d\n", sim.num_agents, event_count);

    destroy_mheap(sim.queue);
    for (int i = 0; i < sim.num_agents; i++) {
        free((void*)sim.agents[i].state);
    }
    free((void*)sim.agents);
    return 0;
}
