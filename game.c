#include <time.h>
#include <stdlib.h>
#include <ncurses.h>

#include "level/level.h"
#include "mob/mob.h"
#include "los.h"

int keyboard_x, keyboard_y = 0;
char message_banner[200];

bool is_position_valid(level *lvl, int x, int y) {
    if (lvl->tiles[y][x] == WALL) return false;
    else {
        for (int i=0; i < lvl->mob_count; i++) {
            if (!lvl->mobs[i]->stacks && lvl->mobs[i]->x == x && lvl->mobs[i]->y == y) {
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

bool approach(level *lvl, mobile *actor, int target_x, int target_y) {
    // Takes one step towards the target position if possible
    int acc_err; // Doesn't actually get used
    int dy = (b_y - actor->y) / (b_x - actor->x);

    int new_x = actor->x;
    int new_y = actor->y;

    next_square(&new_x, &new_y, dy, &acc_err);

    if (is_position_valid(lvl, new_x, new_y)) {
        actor->x = new_x;
        actor->y = new_y;
        return true;
    } else {
        return false;
    }
}

bool line_of_sight(level *lvl, int a_x, int a_y, int b_x, int b_y) {
    // This is between two positions, theoretically non-directional
    int acc_err = 0;
    int dy = (b_y - a_y) / (b_x - a_x);

    while (!(a_x == b_x && a_y == b_y)) {
        next_square(&a_x, &a_y, dy, &acc_err);

        if (!(is_position_valid(lvl, a_x, a_y))) {
            return false;
        }

    return true;
}

bool can_see(mobile *actor, target_x, target_y) {
    // This is between a thing and a position
    // It just wraps line_of_sight for easier English reading
    // Making a thing-to-thing function seems too specific
    return (line_of_sight(actor->x, actor->y, target_x, target_y));
}

void draw_mobile(mobile *mob, int dx, int dy) {
    char display = mob->display;

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
                if (lvl->items[yy][xx] != NULL) {
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

int get_input(level *lvl) {
    char *inventory;
    char *message = malloc(sizeof(char)*200);
    int input = getch();
    int return_code = 0;

    print_message("");

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
        case 'q':
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
                snprintf(full_msg, 200, "You hear the %s say \"%s\"", mob->name, msg);
                print_message(full_msg);
                free(full_msg);
            }
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
            for (int i=0; i < lvl->mob_count; i++) {
                if (lvl->mobs[i]->active) {
                    move_mobile(lvl, lvl->mobs[i]);
                }
            }
            draw(lvl);
        } while (get_input(lvl) == 0);

        destroy_level(lvl);
        endwin();
        return 0;
}
