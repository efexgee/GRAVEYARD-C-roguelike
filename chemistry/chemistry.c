#include <string.h>
#include <stdlib.h>

#include "chemistry.h"

constituents* make_constituents() {
    constituents *con = malloc(sizeof(constituents));
    for (int i = 0; i < ELEMENT_COUNT; i++) {
        con->elements[i] = 0;
    }
    con->stable = true;
    return con;
}

void destroy_constituents(constituents* con) {
    free((void*)con);
}

chemical_system* make_default_chemical_system() {
    chemical_system *sys = make_chemical_system(10);

    for (int j = 0; j < 10; j++) {
        for (int i = 0; i < ash; i++) {
            sys->reactions[j].input.elements[i] = -1;
            sys->reactions[j].output.elements[i] = -1;
        }
    }

    for (int i = 0; i < ELEMENT_COUNT; i++) sys->volitile[i] = false;
    sys->volitile[fire] = true;
    sys->volitile[air] = true;

    // Burning
    sys->reactions[1].input.elements[wood] = 1;
    sys->reactions[1].input.elements[fire] = 1;
    sys->reactions[1].output.elements[wood] = 0;
    sys->reactions[1].output.elements[fire] = 2;
    sys->reactions[1].output.elements[ash] = 1;

    //Fire dying
    sys->reactions[2].input.elements[fire] = 1;
    sys->reactions[2].output.elements[fire] = 0;

    // Anti-venom
    sys->reactions[0].input.elements[venom] = 1;
    sys->reactions[0].input.elements[banz] = 1;
    sys->reactions[0].output.elements[venom] = 0;
    sys->reactions[0].output.elements[banz] = 0;
    sys->reactions[0].output.elements[ash] = 1;

    return sys;
}

chemical_system* make_chemical_system(int num_reactions) {
    chemical_system *sys = malloc(sizeof(chemical_system));
    sys->reactions = malloc(num_reactions*sizeof(reaction));
    sys->num_reactions = num_reactions;
    return sys;
}

void destroy_chemical_system(chemical_system* sys) {
    free((void*)sys->reactions);
    free((void*)sys);
}

bool reaction_possible(reaction *re, constituents *input) {
    for (int i=0; i < ELEMENT_COUNT; i++) {
        if (re->input.elements[i] > 0 && re->input.elements[i] > input->elements[i]) {
            return false;
        }
    }
    return true;
}

void react(chemical_system *sys, constituents *input) {
    bool did_react = false;
    for (int i = 0; i < sys->num_reactions; i++) {
        reaction *re = &sys->reactions[i];
        if (apply_reaction(re, input)) {
            did_react = true;
            break;
        }
    }
    input->stable = !did_react;
}

bool apply_reaction(reaction *re, constituents *input) {
    if (reaction_possible(re, input)) {
        constituents result;
        for (int i=0; i < ELEMENT_COUNT; i++) {
            if (re->input.elements[i] > 0) {
                input->elements[i] -= re->input.elements[i];
            }
            if (re->output.elements[i] > 0) {
                input->elements[i] +=re->output.elements[i];
            }
        }
        return true;
    }
    return false;
}

void add_constituents(constituents *dest, constituents *src) {
        for (int i=0; i < ELEMENT_COUNT; i++) {
            dest->elements[i] += src->elements[i];
        }
}
