#include <time.h>
#include <stdlib.h>
#include <ncurses.h>

#include "level/level.h"
#include "mob/mob.h"

int keyboard_x, keyboard_y = 0;
char message_banner[200];


bool is_position_valid(level *lvl, int x, int y) {
    if (lvl->tiles[y][x] == WALL || lvl->tiles[y][x] == CLOSED_DOOR) return false;
    else {
        for (int i=0; i < lvl->mob_count; i++) {
            if (lvl->mobs[i]->active && !lvl->mobs[i]->stacks && lvl->mobs[i]->x == x && lvl->mobs[i]->y == y) {
                return false;
            }
        }
    }
    return true;
}

bool move_if_valid(level *lvl, mobile *mob, int x, int y) {
    if (is_position_valid(lvl, x, y)) {
        mob->x = x;
        mob->y = y;
        return true;
    } else {
        return false;
    }
}

void draw_mobile(mobile *mob, int dx, int dy) {
    char display = ((item*)mob)->display;

    if (mob->emote) {
        display = mob->emote;
        mob->emote = false;
    }

    mvprintw(dy+mob->y, dx+mob->x, "%c", display);
}

void draw(level *lvl) {
    int row,col;
    getmaxyx(stdscr,row,col);
    row -= 1;

    int dx = col/2 - lvl->player->x;
    int dy = row/2 - lvl->player->y;

    for (int x = 0; x < col; x++) {
        for (int y = 0; y < row; y++) {
            int xx = x - dx;
            int yy = y - dy;
            if (xx >= 0 && xx < lvl->width && yy >= 0 && yy < lvl->height) {
                char display;
                if (lvl->chemistry[yy][xx]->elements[fire] > 0) {
                    display = BURNING;
                } else if (lvl->items[yy][xx] != NULL) {
                    display = lvl->items[yy][xx]->item->display;
                } else {
                    display = lvl->tiles[yy][xx];
                }
                mvprintw(y, x, "%c", display);
            } else {
                mvprintw(y, x, " ");
            }
        }
    }
    for (int i=0; i < lvl->mob_count; i++) {
        if (lvl->mobs[i]->active) {
            mobile* mob = lvl->mobs[i];
            if (mob->y+dy > 0 && mob->y+dy <= row && mob->x+dx > 0 && mob->x+dx <= col) {
                draw_mobile(lvl->mobs[i], dx, dy);
            }
        }
    }
    draw_mobile(lvl->player, dx, dy);
    move(row, 0);
    clrtoeol();
    mvprintw(row, 0, message_banner);
}

void print_message(char *msg) {
    snprintf(message_banner, 200, msg);
}

void drop_item(level *lvl, mobile *mob) {
    item *item = pop_inventory(mob);
    if (item != NULL) {
        level_push_item(lvl, item, mob->x, mob->y);
        if (mob == lvl->player) {
            char msg[200];
            snprintf(msg, 200, "You drop the %s", item->name);
            print_message(msg);
        }
    }
}

void pickup_item(level *lvl, mobile *mob) {
    item *item = level_pop_item(lvl, mob->x, mob->y);
    if (item != NULL) {
        push_inventory(mob, item);
        if (mob == lvl->player) {
            char msg[200];
            snprintf(msg, 200, "You pick up the %s", item->name);
            print_message(msg);
        }
    }
}

void smash(level *lvl, mobile *mob) {
    item *potion = ((item*)mob)->contents->item;
    add_constituents(lvl->chemistry[mob->y][mob->x], potion->chemistry);
    inventory_item *inv = ((item*)mob)->contents;
    ((item*)mob)->contents = inv->next;
    free((void*)inv);
    destroy_item(potion);
}

void print_location_elements(level *lvl, mobile *mob) {
    char *message = malloc(sizeof(char)*200);
    constituents *chem = lvl->chemistry[mob->y][mob->x];
    snprintf(message, 200, "wood: %d air: %d fire: %d", chem->elements[wood], chem->elements[air], chem->elements[fire]);
    print_message(message);
    free((void*)message);
}

void toggle_door(level *lvl, mobile *mob) {
    int x = mob->x;
    int y = mob->y;
    switch (getch()) {
        case KEY_UP:
            y -= 1;
            break;
        case KEY_DOWN:
            y += 1;
            break;
        case KEY_RIGHT:
            x += 1;
            break;
        case KEY_LEFT:
            x -= 1;
            break;
        default:
            return;
    }
    if (lvl->tiles[y][x] == OPEN_DOOR) {
        lvl->tiles[y][x] = CLOSED_DOOR;
    } else if (lvl->tiles[y][x] == CLOSED_DOOR) {
        lvl->tiles[y][x] = OPEN_DOOR;
    }
}

int get_input(level *lvl) {
    char *inventory;
    char *message = malloc(sizeof(char)*200);
    int input = getch();
    int return_code = 0;


    switch (input) {
        case KEY_UP:
            keyboard_y = -1;
            break;
        case KEY_DOWN:
            keyboard_y = 1;
            break;
        case KEY_RIGHT:
            keyboard_x = 1;
            break;
        case KEY_LEFT:
            keyboard_x = -1;
            break;
        case 'i':
            inventory = inventory_string(lvl->player, 200);
            snprintf(message, 200, "Your inventory contains: %s", inventory);
            print_message(message);
            free((void*)inventory);
            break;
        case 'd':
            drop_item(lvl, lvl->player);
            break;
        case '.':
            pickup_item(lvl, lvl->player);
            break;
        case 'r':
            rotate_inventory(lvl->player);
            inventory = inventory_string(lvl->player, 200);
            snprintf(message, 200, "Your inventory contains: %s", inventory);
            print_message(message);
            free((void*)inventory);
            break;
        case 'q':
            if(quaff(lvl->player)) {
                print_message("You drink a potion.");
            } else {
                print_message("That isn't a potion.");
            }
            break;
        case 'v':
            if (((item*)lvl->player)->contents != NULL && ((item*)lvl->player)->contents->item->type == Potion) {
                smash(lvl, lvl->player);
                print_message("You smash the potion on the floor.");
            } else print_message("That isn't a potion.");
            break;
        case 's':
            snprintf(message, 200, "You have %d hit points. venom: %d banz: %d life: %d", lvl->player->health, ((item*)lvl->player)->chemistry->elements[venom], ((item*)lvl->player)->chemistry->elements[banz], ((item*)lvl->player)->chemistry->elements[life]);
            print_message(message);
            break;
        case 't':
            print_location_elements(lvl, lvl->player);
            break;
        case 'o':
            toggle_door(lvl, lvl->player);
            break;
        case 'Q':
            return_code = -1;
    }
    free((void*)message);
    return return_code;
}

void move_mobile(level *lvl, mobile *mob) {
    int x = mob->x;
    int y = mob->y;
    switch (mob->behavior) {
        case RandomWalk:
            if (rand()%2 == 0) {
                x += rand()%3 - 1;
            } else {
                y += rand()%3 - 1;
            }
            break;
        case KeyboardInput:
            x += keyboard_x;
            y += keyboard_y;
            keyboard_x = 0;
            keyboard_y = 0;
            break;
    }
    if (x != mob->x || y != mob->y) {
        if (!(move_if_valid(lvl, mob, x, y))) {
            mob->emote = OUCH;
            if (rand()%100 > 95) {
                char *msg;
                switch (rand()%4) {
                    case 0:
                        msg = "Mother fucker!";
                        break;
                    case 1:
                        msg = "Shitting shit!";
                        break;
                    case 2:
                        msg = "Jesus H. Christ!";
                        break;
                    case 3:
                        msg = "Poop balls!";
                        break;
                }
                char *full_msg = malloc(sizeof(char)*200);
                snprintf(full_msg, 200, "You hear the %s say \"%s\"", ((item*)mob)->name, msg);
                print_message(full_msg);
                free(full_msg);
            }
        }
    }
}

void step_chemistry(chemical_system *sys, constituents *chem) {
    for (int i = 0; i < 3; i++) {
        if (!chem->stable) {
            react(sys, chem);
        }
    }
}

void step_inventory_chemistry(chemical_system *sys, inventory_item *inv) {
    while (inv != NULL) {
        step_chemistry(sys, inv->item->chemistry);
        inv = inv->next;
    }
}

void step_mobile(level *lvl, mobile *mob) {
    constituents *chemistry = ((item*)mob)->chemistry;
    move_mobile(lvl, mob);
    if (lvl->chemistry[mob->y][mob->x]->elements[air] > 5) {
        lvl->chemistry[mob->y][mob->x]->elements[air] -= 5;
    } else {
        mob->health -= 1;
    }
    if (chemistry->elements[life] > 0) {
        chemistry->elements[life] -= 10;
        mob->health += 1;
    }
    if (chemistry->elements[venom] > 0) {
        chemistry->elements[venom] -= 10;
        mob->health -= 1;
    }
    step_chemistry(lvl->chem_sys, chemistry);
    step_inventory_chemistry(lvl->chem_sys, ((item*)mob)->contents);
    if (mob->health <= 0) {
        mob->active = false;
    }
}

void level_step_chemistry(level* lvl) {
    for (int x = 0; x < lvl->width; x++) {
        for (int y = 0; y < lvl->height; y++) {
            step_chemistry(lvl->chem_sys, lvl->chemistry[y][x]);
            if (lvl->tiles[y][x] == FLOOR && lvl->chemistry[y][x]->elements[air] < 20) lvl->chemistry[y][x]->elements[air] += 3;
        }
    }
    for (int element = 0; element < ELEMENT_COUNT; element++) {
        if (lvl->chem_sys->volitile[element]) {
            int **added_element = malloc(lvl->height * sizeof(int*));
            added_element[0] = malloc(lvl->height * lvl->width * sizeof(int));
            int **removed_element = malloc(lvl->height * sizeof(int*));
            removed_element[0] = malloc(lvl->height * lvl->width * sizeof(int));
            for(int i = 1; i < lvl->height; i++) {
                added_element[i] = added_element[0] + i * lvl->width;
                removed_element[i] = removed_element[0] + i * lvl->width;
            }
            for (int x = 0; x < lvl->width; x++) {
                for (int y = 0; y < lvl->height; y++) {
                    added_element[y][x] = 0;
                    removed_element[y][x] = 0;
                }
            }

            for (int x = 0; x < lvl->width; x++) {
                for (int y = 0; y < lvl->height; y++) {
                    int rx = rand();
                    int ry = rand();
                    for (int dx = 0; dx < 2; dx++) {
                        int xx = x + (((dx+rx)%3)-1);
                        for (int dy = 0; dy < 2; dy++) {
                            int yy = y + (((dy+ry)%3)-1);
                            if (xx >= 0 && xx < lvl->width && yy >= 0 && yy < lvl->height) {
                                if (lvl->tiles[yy][xx] != WALL && lvl->tiles[yy][xx] != CLOSED_DOOR && lvl->chemistry[y][x]->elements[element] - removed_element[y][x] > lvl->chemistry[yy][xx]->elements[element] + added_element[yy][xx]) {
                                    removed_element[y][x] += 1;
                                    added_element[yy][xx] += 1;
                                }
                            }
                        }
                    }
                }
            }
            for (int x = 0; x < lvl->width; x++) {
                for (int y = 0; y < lvl->height; y++) {
                    if (added_element[y][x] > 0 || removed_element[y][x] > 0) {
                        lvl->chemistry[y][x]->elements[element] += added_element[y][x] - removed_element[y][x];
                        lvl->chemistry[y][x]->stable = false;
                    }
                }
            }
            free((void*)added_element[0]);
            free((void*)added_element);
            free((void*)removed_element[0]);
            free((void*)removed_element);
        }
    }


}

int main() {
        int ch;
        level *lvl;

        srand(time(NULL));
        initscr();
        raw();
        noecho();
        cbreak();
        keypad(stdscr, TRUE);

        curs_set(0);

        lvl = make_level();
        draw(lvl);
        do {

            if (!lvl->player->active) {
                getch();
                break;
            }

            for (int i=0; i < lvl->mob_count; i++) {
                if (lvl->mobs[i]->active) {
                    step_mobile(lvl, lvl->mobs[i]);
                }
            }
            level_step_chemistry(lvl);
            if (!lvl->player->active) {
                print_message("You die");
            }
            draw(lvl);
            print_message("");
        } while (get_input(lvl) == 0);

        destroy_level(lvl);
        endwin();
        return 0;
}
