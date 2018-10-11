#ifndef INC_LEVEL_H
#define INC_LEVEL_H

#include <stdbool.h>
#include <stdlib.h>

#include "../mob/mob.h"

#define LVL_WIDTH 80;
#define LVL_HEIGHT 40;

#define WALL '#'
#define FLOOR ' ' // floor is blank
#define OPEN_DOOR ' ' // doors are blank
#define CLOSED_DOOR '+'

typedef struct Level {
    unsigned char **tiles;
    inventory_item ***items;
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
