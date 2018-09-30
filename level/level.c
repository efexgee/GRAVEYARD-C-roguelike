#include <stdio.h>
#include <string.h>

#include "level.h"
#include "../mob/mob.h"

void partition(level *lvl);


level* make_level(void) {
    level *lvl = malloc(sizeof *lvl);
    int level_width = 50;
    int level_height = 50;

    lvl->width = level_width;
    lvl->height = level_height;
    lvl->mob_count = 101;
    lvl->mobs = malloc(lvl->mob_count * (sizeof(mobile*)));
    for (int i=0; i < lvl->mob_count; i++) lvl->mobs[i] = make_mob();
    lvl->tiles = malloc(level_height * sizeof(unsigned char*));
    lvl->tiles[0] = malloc(level_height * level_width * sizeof(unsigned char));
    for(int i = 1; i < level_height; i++)
        lvl->tiles[i] = lvl->tiles[0] + i * level_width;

    lvl->items = malloc(level_height * sizeof(inventory_item**));
    lvl->items[0] = malloc(level_height * level_width * sizeof(inventory_item*));
    for(int i = 1; i < level_height; i++)
        lvl->items[i] = lvl->items[0] + i * level_width;
    for (int x = 0; x < lvl->width; x++) for (int y = 0; y < lvl->height; y++) {
        lvl->items[y][x] = NULL;
    }

    lvl->chemistry = malloc(level_height * sizeof(inventory_item**));
    lvl->chemistry[0] = malloc(level_height * level_width * sizeof(inventory_item*));
    for(int i = 1; i < level_height; i++)
        lvl->chemistry[i] = lvl->chemistry[0] + i * level_width;
    for (int x = 0; x < lvl->width; x++) for (int y = 0; y < lvl->height; y++) {
        lvl->chemistry[y][x] = make_constituents();
        lvl->chemistry[y][x]->elements[wood] = 300;
        lvl->chemistry[y][x]->elements[air] = 20;
    }


    lvl->chem_sys = make_default_chemical_system();


    partition(lvl);

    lvl->player = lvl->mobs[lvl->mob_count-1];
    lvl->player->x = lvl->player->y = 1;
    lvl->player->behavior = KeyboardInput;
    lvl->player->health = 10;
    ((item*)lvl->player)->display = ICON_HUMAN;
    ((item*)lvl->player)->name = malloc(sizeof(char)*9);
    strcpy(((item*)lvl->player)->name, "yourself");
    lvl->player->active = true;

    item* potion = malloc(sizeof(item)); // FIXME leaks
    potion->display = 'p';
    potion->chemistry = make_constituents();
    potion->chemistry->elements[life] = 30;
    potion->chemistry->elements[fire] = 30;
    potion->name = "Healing Potion";
    potion->type = Potion;
    push_inventory(lvl->player, potion);

    item* poison = malloc(sizeof(item)); // FIXME leaks
    poison->display = 'p';
    poison->chemistry = make_constituents();
    poison->chemistry->elements[venom] = 30;
    poison->name = "Poison";
    poison->type = Potion;
    push_inventory(lvl->player, poison);

    item* antidote = malloc(sizeof(item)); // FIXME leaks
    antidote->display = 'p';
    antidote->chemistry = make_constituents();
    antidote->chemistry->elements[banz] = 30;
    antidote->name = "Antidote";
    antidote->type = Potion;
    push_inventory(lvl->player, antidote);

    item* lamp = malloc(sizeof(item)); // FIXME leaks
    lamp->display = 'l';
    lamp->chemistry = make_constituents();
    lamp->chemistry->elements[earth] = 30;
    lamp->name = "Lamp";
    lamp->type = Weapon;
    push_inventory(lvl->player, lamp);

    for (int i=0; i < lvl->mob_count-1; i++) {
        lvl->mobs[i]->x = rand()%(level_width-2) + 1;
        lvl->mobs[i]->y = rand()%(level_height-2) + 1;
        lvl->mobs[i]->behavior = RandomWalk;
        lvl->mobs[i]->active = true;

        if (rand()%2 == 0) {
            ((item*)lvl->mobs[i])->display = ICON_GOBLIN;
            lvl->mobs[i]->stacks = true;
            ((item*)lvl->mobs[i])->name = malloc(sizeof(char)*7);
            strcpy(((item*)lvl->mobs[i])->name, "goblin");
        } else {
            ((item*)lvl->mobs[i])->display = ICON_ORC;
            ((item*)lvl->mobs[i])->name = malloc(sizeof(char)*4);
            strcpy(((item*)lvl->mobs[i])->name, "orc");
        }
    }

    return lvl;
}

void destroy_level(level *lvl) {
    free((void *)lvl->tiles[0]);
    free((void *)lvl->tiles);
    free((void *)lvl->items[0]);
    free((void *)lvl->items);
    for (int x = 0; x < lvl->width; x++) for (int y = 0; y < lvl->height; y++) {
        destroy_constituents(lvl->chemistry[y][x]);
    }
    free((void *)lvl->chemistry[0]);
    free((void *)lvl->chemistry);
    destroy_chemical_system(lvl->chem_sys);
    for (int i = 0; i < lvl->mob_count; i++) destroy_mob(lvl->mobs[i]);
    free((void *)lvl->mobs);
    free((void *)lvl);
}

int rec_partition(int **room_map, int x, int y, int w, int h, int rm) {
    if (w*h > 10*10 && rand()%100 < 50) {
        int hw = w/2;
        int hh = h/2;
        int max_rm, new_rm;

        max_rm = rec_partition(room_map, x, y, hw, hh, rm);
        new_rm = rec_partition(room_map, x + hw, y, w-hw, hh, max_rm);
        if (new_rm > max_rm) max_rm = new_rm;
        new_rm = rec_partition(room_map, x + hw, y + hh, w-hw, h-hh, max_rm);
        if (new_rm > max_rm) max_rm = new_rm;
        new_rm = rec_partition(room_map, x, y + hh, hw, h-hh, max_rm);
        if (new_rm > max_rm) max_rm = new_rm;
        return max_rm;
    } else {
        rm += 1;
        for (int xx = x; xx < x+w; xx++) {
            for (int yy = y; yy < y+h; yy++) {
                room_map[yy][xx] = rm;
            }
        }
        return rm;
    }
}

void partition(level *lvl) {
    int **partitioning = malloc(lvl->height * sizeof (int*));
    for (int i = 0 ; i != lvl->height ; i++) partitioning[i] = malloc (lvl->width*sizeof(int));
    int **potential_doors= malloc(lvl->height * sizeof (int*));
    for (int i = 0 ; i != lvl->height ; i++) potential_doors[i] = malloc (lvl->width*sizeof(int));

    int rooms = rec_partition(partitioning, 0, 0, lvl->width, lvl->height, 0);

    for (int x = 0; x < lvl->width; x++) {
        for (int y = 0; y < lvl->height; y++) {
            lvl->tiles[y][x] = FLOOR;
        }
    }

    for (int x = 0; x < lvl->width; x++) {
        for (int y = 0; y < lvl->height; y++) {
            for (int dx = -1; dx < 1; ++dx) for (int dy = -1; dy < 1; ++dy) {
                int xx = x+dx;
                int yy = y+dy;
                if (xx < 0 || yy < 0 || xx >= lvl->width -1 || yy >= lvl->height -1) {
                    lvl->tiles[y][x] = WALL;
                } else if (partitioning[yy][xx] != partitioning[y][x]) {
                    lvl->tiles[y][x] = WALL;
                    if (abs(dx+dy) == 1) {
                        potential_doors[y][x] = true;
                    }
                }
            }
        }
    }

    bool room_accessible[rooms];
    for (int i = 0; i < rooms; i++) room_accessible[i] = false;
    room_accessible[partitioning[0][0]] = true;
    for (int x = 0; x < lvl->width; x++) {
        for (int y = 0; y < lvl->height; y++) {
            if (potential_doors[y][x]) {
                bool door_needed = false;
                bool door_possible = false;
                int rm_a, rm_b;
                if (x+1 < lvl->width && x-1 >= 0 && lvl->tiles[y][x+1] != WALL && lvl->tiles[y][x-1] != WALL) {
                    rm_a = partitioning[y][x+1];
                    rm_b = partitioning[y][x-1];
                    door_possible = true;
                }
                if (y+1 < lvl->height && y-1 >= 0 && lvl->tiles[y+1][x] != WALL && lvl->tiles[y-1][x] != WALL) {
                    rm_a = partitioning[y+1][x];
                    rm_b = partitioning[y-1][x];
                    door_possible = true;
                }
                if (door_possible && rand()%100 > 0) {
                    if (room_accessible[rm_a] + !room_accessible[rm_b] != 1) {
                        door_needed = true;
                        room_accessible[rm_b]=true;
                        room_accessible[rm_a]=true;
                    }
                }
                if (door_needed) {
                    lvl->tiles[y][x] = OPEN_DOOR;
                } else {
                    lvl->tiles[y][x] = WALL;
                }
            }
        }
    }


    free((void *)partitioning[0]);
    free((void *)partitioning);
    free((void *)potential_doors[0]);
    free((void *)potential_doors);
}

void level_push_item(level *lvl, item *itm, int x, int y) {
    inventory_item *new_inv = malloc(sizeof(inventory_item));
    new_inv->next = NULL;
    new_inv->item = itm;
    if (lvl->items[y][x] == NULL) {
        lvl->items[y][x] = new_inv;
    } else {
        inventory_item *inv = lvl->items[y][x];
        while (inv->next != NULL) inv = inv->next;
        inv->next = new_inv;
    }
}

item* level_pop_item(level *lvl, int x, int y) {
    if (lvl->items[y][x] == NULL) {
        return NULL;
    } else {
        inventory_item *old = lvl->items[y][x];
        lvl->items[y][x] = old->next;
        item *itm = old-> item;
        free(old);
        return itm;
    }
}
