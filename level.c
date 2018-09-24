#include <stdio.h>

#include "level.h"

void partition(level *lvl);


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

    partition(lvl);

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

int rec_partition(unsigned int **room_map, int x, int y, int w, int h, int rm) {
    if (w*h > 10*10 && rand()%100 < 80) {
        int hw = w/2;
        int hh = h/2;
        int max_rm, new_rm;

        max_rm = rec_partition(room_map, x, y, hw, hh, rm + 1);
        new_rm = rec_partition(room_map, x + hw, y, w-hw, hh, rm + 2);
        if (new_rm > max_rm) max_rm = new_rm;
        new_rm = rec_partition(room_map, x + hw, y + hh, w-hw, h-hh, rm + 3);
        if (new_rm > max_rm) max_rm = new_rm;
        new_rm = rec_partition(room_map, x, y + hh, hw, h-hh, rm + 4);
        if (new_rm > max_rm) max_rm = new_rm;
        return max_rm;
    } else {
        for (int xx = x; xx < x+w; xx++) {
            for (int yy = y; yy < y+h; yy++) {
                room_map[yy][xx] = rm;
            }
        }
        return rm;
    }
}

void partition(level *lvl) {
    unsigned int **partitioning = malloc(lvl->height * sizeof (unsigned int*));
    for (int i = 0 ; i != lvl->height ; i++) partitioning[i] = malloc (lvl->width*sizeof(unsigned int));

    int rooms = rec_partition(partitioning, 0, 0, lvl->width, lvl->height, 0);
    bool room_accessible[rooms];
    for (int i = 0; i < rooms; i++) room_accessible[i] = false;
    room_accessible[partitioning[0][0]] = true;

    for (int x = 0; x < lvl->width; x++) {
        for (int y = 0; y < lvl->height; y++) {
            lvl->tiles[y][x] = FLOOR;
            int placement = -1;
            for (int dx = -1; dx < 1; dx++) for (int dy = -1; dy < 1; dy++) {
                int xx = x+dx;
                int yy = y+dy;
                if (xx < 0 || yy < 0 || xx >= lvl->width -1 || yy >= lvl->height -1) {
                    lvl->tiles[y][x] = WALL;
                } else if (partitioning[yy][xx] != partitioning[y][x] && lvl->tiles[yy][xx] == FLOOR && lvl->tiles[y][x] == FLOOR) {
                    int rm_a = partitioning[y][x];
                    int rm_b = partitioning[yy][xx];
                    if (abs(dx+dy) == 1) {
                        if (!room_accessible[rm_a] && room_accessible[rm_b]) {
                            room_accessible[rm_a] = true;
                            placement = (int)DOOR;
                        } else if (room_accessible[rm_a] && !room_accessible[rm_b]) {
                            room_accessible[rm_b] = true;
                            placement = (int)DOOR;
                        }
                    }
                    if (lvl->tiles[y][x] == FLOOR && placement == -1) {
                        placement = (int)WALL;
                    }
                }
            }
            if (placement != -1) {
                lvl->tiles[y][x] = (unsigned char)placement;
            }
        }
    }

    free((void *)partitioning[0]);
    free((void *)partitioning);
}
