#ifndef INC_LEVEL_H
#define INC_LEVEL_H

#include <stdbool.h>
#include <stdlib.h>

#include "../mob/mob.h"

#define WALL '#'
#define FLOOR '.'
#define OPEN_DOOR '-'
#define CLOSED_DOOR '+'

typedef struct Level {
    unsigned char **tiles;
    int width;
    int height;
    mobile *mobs;
    int mob_count;
    mobile *player;
} level;

level* make_level(void);
void destroy_level(level *lvl);

#endif
