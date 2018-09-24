#include "level.h"

level* make_level(void) {
    level *lvl = malloc(sizeof *lvl);
    int level_width = 200;
    int level_height = 100;

    lvl->width = level_width;
    lvl->height = level_height;
    lvl->mob_count = 101;
    lvl->mobs = malloc(lvl->mob_count * (sizeof(mobile)));
    lvl->tiles = malloc(level_height * sizeof(unsigned char*));
    lvl->tiles[0] = malloc(level_height * level_width * sizeof(unsigned char));
    for(int i = 1; i < level_height; i++)
        lvl->tiles[i] = lvl->tiles[0] + i * level_width;

    for (int x = 0; x < level_width; x++) {
        for (int y = 0; y < level_height; y++) {
            if (x == 0 || y == 0 || y == level_height-1 || x == level_width-1) {
                lvl->tiles[y][x] = WALL;
            } else {
                lvl->tiles[y][x] = FLOOR;
            }
        }
    }
    for (int i=0; i < lvl->mob_count; i++) {
        lvl->mobs[i].active = false;
        lvl->mobs[i].stacks = false;
    }

    lvl->player = &lvl->mobs[lvl->mob_count-1];
    lvl->player->x = lvl->player->y = 1;
    lvl->player->behavior = KeyboardInput;
    lvl->player->display = ICON_HUMAN;
    lvl->player->active = true;

    for (int i=0; i < lvl->mob_count-1; i++) {
        lvl->mobs[i].x = rand()%(level_width-2) + 1;
        lvl->mobs[i].y = rand()%(level_height-2) + 1;
        lvl->mobs[i].behavior = RandomWalk;
        lvl->mobs[i].active = true;

        if (rand()%2 == 0) {
            lvl->mobs[i].display = ICON_GOBLIN;
            lvl->mobs[i].stacks = true;
        } else {
            lvl->mobs[i].display = ICON_ORC;
        }
    }

    return lvl;
}

void destroy_level(level *lvl) {
    free((void *)lvl->tiles[0]);
    free((void *)lvl->tiles);
    free((void *)lvl->mobs);
    free((void *)lvl);
}
