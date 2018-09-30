#ifndef INC_LEVEL_H
#define INC_LEVEL_H

#include <stdbool.h>
#include <stdlib.h>

#include "../mob/mob.h"
#include "../chemistry/chemistry.h"

#define WALL '#'
#define FLOOR '.'
#define OPEN_DOOR '-'
#define CLOSED_DOOR '+'
#define BURNING '*'

typedef struct Level {
    unsigned char **tiles;
    inventory_item ***items;
    constituents ***chemistry;
    chemical_system *chem_sys;
    int width;
    int height;
    mobile **mobs;
    int mob_count;
    mobile *player;
} level;

level* make_level(void);
void destroy_level(level *lvl);
void level_push_item(level *lvl, item *itm, int x, int y);
item* level_pop_item(level *lvl, int x, int y);

#endif
