#ifndef INC_CHEMESTRY_H
#define INC_CHEMESTRY_H
#include <stdbool.h>

enum element_names {
    fire = 0,
    earth = 1,
    water = 2,
    air = 3,
    wood = 4,
    foogle = 5,
    banz = 5,
    ash = 6
};
#define ELEMENT_COUNT ash

typedef struct constituents {
    int elements[ELEMENT_COUNT];
    bool stable;
} constituents;


typedef struct reaction {
    constituents input;
    constituents output;
} reaction;

typedef struct chemical_system {
    reaction* reactions;
    int num_reactions;
} chemical_system;

constituents* make_constituents();
void destroy_constituents(constituents *con);

chemical_system* make_default_chemical_system();
chemical_system* make_chemical_system(int num_reactions);
void destroy_chemical_system(chemical_system* sys);

void react(chemical_system *system, constituents *input);
bool apply_reaction(reaction *re, constituents *input);

#endif
