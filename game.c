#include <time.h>
#include <stdlib.h>
#include <ncurses.h>
#include <string.h>

#include "level/level.h"
#include "mob/mob.h"
#include "los/los.h"
#include "colors/colors.h"

int keyboard_x = 0, keyboard_y = 0;
char message_banner[200];

bool is_position_valid(level *lvl, int x, int y) {
    if (x >= lvl->width || x < 0) {
        fprintf(stderr, "ERROR %s: %s: %d\n", "is_position_valid", "x is out of bounds", x);
        return false;
    } else if (y >= lvl->height || y < 0) {
        fprintf(stderr, "ERROR %s: %s: %d\n", "is_position_valid", "y is out of bounds", y);
        return false;
    } else if (lvl->tiles[x][y] == WALL || lvl->tiles[x][y] == CLOSED_DOOR) {
        // from a performance standpoint, this should be the first test
        return false;
    } else {
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

void set_steps(int *x_step, float *slope, int a_x, int a_y, int b_x, int b_y) {
    int dx = b_x - a_x;
    int dy = b_y - a_y;

    *slope = (float) dy / dx;
    *x_step = dx < 0 ? -1 : 1;

    //fprintf(stderr, "[%s] xs %2d m %6.2f (%2d/%2d) ax %2d ay %2d bx %2d by %2d\n", "set_steps", *x_step, *slope, dy, dx, a_x, a_y, b_x, b_y);
}

bool approach(level *lvl, mobile *actor, int target_x, int target_y) {
    int new_x = actor->x;
    int new_y = actor->y;

    // TODO how to do bools?
    step_towards(&new_x, &new_y, target_x, target_y, false);

    return(move_if_valid(lvl, actor, new_x, new_y));
}

bool line_of_sight(level *lvl, int a_x, int a_y, int b_x, int b_y) {
    // This is between two positions, theoretically non-directional
    int x_step;
    float slope;
    float acc_err = 0;

    set_steps(&x_step, &slope, a_x, a_y, b_x, b_y);

    //while (!(a_x == b_x && a_y == b_y)) {
    while (true) {
        next_square(&a_x, &a_y, x_step, slope, &acc_err);

        if (a_x == b_x && a_y == b_y) return true;

        if (!(is_position_valid(lvl, a_x, a_y))) return false;

        // lvl->tiles[a_x][a_y] = '+';
        //fprintf(stderr, "Placed a '+' at (%2d, %2d)\n", a_x, a_y);
    }
}

bool can_see(level *lvl, mobile *actor, int target_x, int target_y) {
    // This is between a thing and a position
    // It just wraps line_of_sight for easier English reading
    // Making a thing-to-thing function seems too specific
    return (line_of_sight(lvl, actor->x, actor->y, target_x, target_y));
}

void draw_mobile(mobile *mob, int dx, int dy) {
    char display = ((item*)mob)->display;

    if (mob->emote) {
        display = mob->emote;
        mob->emote = false;
    }

    //fprintf(stderr, "Draw me at (%d,%d)\n", mob->x, mob->y);

    mvprintw(dy+mob->y, dx+mob->x, "%c", display);
}

void draw(level *lvl) {
    int row,col;
    getmaxyx(stdscr,row,col);
    row -= 1;

    int dx = col/2 - lvl->player->x;
    int dy = row/2 - lvl->player->y;

    // declarations inside loops?
    for (int x = 0; x < col; x++) {
        for (int y = 0; y < row; y++) {
            int xx = x - dx;
            int yy = y - dy;
            // Positions which are part of the map
            if (xx >= 0 && xx < lvl->width && yy >= 0 && yy < lvl->height) {
                // Borders are always visible
                char display;
                //if ( xx == 0 || yy == 0 || xx == lvl->width - 1 || yy == lvl->height -1 || can_see(lvl, lvl->player, xx, yy) || lvl->tiles[xx][yy] == '+') {
                if ( xx == 0 || yy == 0 || xx == lvl->width - 1 || yy == lvl->height -1 || lvl->tiles[xx][yy] != ' ' ) {
                    // Burning supercedes everything!
                    if (lvl->chemistry[xx][yy]->elements[fire] > 0) {
                        display = BURNING;
                    } else {
                        // Draw items
                        if (lvl->items[xx][yy] != NULL) {
                            display = lvl->items[xx][yy]->item->display;
                        } else {
                            // Draw the square
                            display = lvl->tiles[xx][yy];
                            // HACK: Draw red pluses
                            if (lvl->tiles[xx][yy] == '+') {
                                //fprintf(stderr, "Prepped a '+' to be drawn at (%2d, %2d)\n", xx, yy);
                                attron(COLOR_PAIR(RED));
                                // HACK: Remove the plus because... hack
                                lvl->tiles[xx][yy] = ' ';
                            }
                        }
                    }
                } else {
                    // should be using a constant here for the char to print
                    if (can_see(lvl, lvl->player, xx, yy)) {
                        display = '.';
                    } else {
                        display = ' ';
                    }
                }
                mvprintw(y, x, "%c", display);
            } else {
                // should be using a constant here for the char to print
                mvprintw(y, x, "%c", ' ');
            }
            // this should be a general unsetter, not this
            attroff(COLOR_PAIR(RED));
            // this does not work:
            //attron(COLOR_PAIR(DEFAULT));
        }
    }
    for (int i=0; i < lvl->mob_count; i++) {
        if (lvl->mobs[i]->active) {
            mobile* mob = lvl->mobs[i];
            if (mob->y+dy > 0 && mob->y+dy <= row && mob->x+dx > 0 && mob->x+dx <= col) {
                //fprintf(stderr, "Drawing mob #%d\n", i);
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
    strncpy(message_banner, msg, 200);
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
    add_constituents(lvl->chemistry[mob->x][mob->y], potion->chemistry);
    inventory_item *inv = ((item*)mob)->contents;
    ((item*)mob)->contents = inv->next;
    free((void*)inv);
    destroy_item(potion);
}

void print_location_elements(level *lvl, mobile *mob) {
    char *message = malloc(sizeof(char)*200);
    constituents *chem = lvl->chemistry[mob->x][mob->y];
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
    if (lvl->tiles[x][y] == OPEN_DOOR) {
        lvl->tiles[x][y] = CLOSED_DOOR;
    } else if (lvl->tiles[x][y] == CLOSED_DOOR) {
        lvl->tiles[x][y] = OPEN_DOOR;
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
        case 'e':
            if (((item*)lvl->player)->contents != NULL) {
                snprintf(message, 200, "wood: %d fire: %d ash: %d",
                        ((item*)lvl->player)->contents->item->chemistry->elements[wood],
                        ((item*)lvl->player)->contents->item->chemistry->elements[fire],
                        ((item*)lvl->player)->contents->item->chemistry->elements[ash]
                        );
                print_message(message);
            }
            break;
        case 's':
            snprintf(message, 200, "You have %d hit points. venom: %d banz: %d life: %d", ((item*)lvl->player)->health, ((item*)lvl->player)->chemistry->elements[venom], ((item*)lvl->player)->chemistry->elements[banz], ((item*)lvl->player)->chemistry->elements[life]);
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
        case Minotaur:
            if (can_see(lvl, mob, lvl->player->x, lvl->player->y)) {
                print_message("You are spotted by a minotaur!");
                // use pointers as aliases to make this less messy?
                approach(lvl, mob, lvl->player->x, lvl->player->y);
                //fprintf(stderr, "I'm at (%d,%d) now\n", mob->x, mob->y);

                // Horrible speudo-inheritance syntax
                ((item*) mob)->display = '>';
                // mob->base->display = '>';

                // approach() already did the move
                // this ruins a lot of things
                return;
            } else {
                print_message("The minotaur can't find you.");
                ((item*)mob)->display = ICON_MINOTAUR;
                if (rand()%2 == 0) {
                    x += rand()%3 - 1;
                } else {
                    y += rand()%3 - 1;
                }
            }
            break;
    }
    if (x != mob->x || y != mob->y) {
        //fprintf(stderr, "%s's (x,y) has changed: (%2d,%2d)\n", ((item*)mob)->name, mob->x, mob->y);
        if (!(move_if_valid(lvl, mob, x, y))) {
            mob->emote = EMOTE_OUCH;
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
                        msg = "Poop weasels!";
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

void step_chemistry(chemical_system *sys, constituents *chem, constituents *context) {
    for (int i = 0; i < 3; i++) {
        bool is_stable = chem->stable;
        if (context != NULL) is_stable = is_stable && context->stable;
        if (!is_stable) {
            react(sys, chem, context);
        }
    }
}

void step_inventory_chemistry(chemical_system *sys, inventory_item *inv, constituents *context) {
    while (inv != NULL) {
        step_chemistry(sys, inv->item->chemistry, context);
        inv = inv->next;
    }
}

void step_item(level *lvl, item *itm, constituents *chem_ctx) {
    step_chemistry(lvl->chem_sys, itm->chemistry, chem_ctx);
    step_inventory_chemistry(lvl->chem_sys, itm->contents, itm->chemistry);
    bool burning = itm->chemistry->elements[fire] > 0;
    if (chem_ctx != NULL) burning = burning || chem_ctx->elements[fire] > 0;
    if (burning && itm->health > 0) itm->health -= 1;
}

void step_mobile(level *lvl, mobile *mob) {
    constituents *chemistry = ((item*)mob)->chemistry;
    move_mobile(lvl, mob);
    if (lvl->chemistry[mob->x][mob->y]->elements[air] > 5) {
        lvl->chemistry[mob->x][mob->y]->elements[air] -= 5;
    } else {
        ((item*)mob)->health -= 1;
    }
    if (chemistry->elements[life] > 0) {
        chemistry->elements[life] -= 10;
        ((item*)mob)->health += 1;
    }
    if (chemistry->elements[venom] > 0) {
        chemistry->elements[venom] -= 10;
        ((item*)mob)->health -= 1;
    }
    step_item(lvl, (item*)mob, lvl->chemistry[mob->x][mob->y]);
    if (((item*)mob)->health <= 0) {
        mob->active = false;
    }
}

void level_step_chemistry(level* lvl) {
    for (int x = 0; x < lvl->width; x++) {
        for (int y = 0; y < lvl->height; y++) {
            step_chemistry(lvl->chem_sys, lvl->chemistry[x][y], NULL);
            inventory_item *inv = lvl->items[x][y];
            while (inv != NULL) {
                step_item(lvl, inv->item, lvl->chemistry[x][y]);
                if (inv->item->health <= 0) {
                    inv->item->name = "Ashy Remnants";
                    inv->item->display = '~';
                }
                inv = inv->next;
            }
            if (lvl->tiles[x][y] != WALL && lvl->tiles[x][y] != CLOSED_DOOR && lvl->chemistry[x][y]->elements[air] < 20) lvl->chemistry[x][y]->elements[air] += 3;
        }
    }
    for (int element = 0; element < ELEMENT_COUNT; element++) {
        if (lvl->chem_sys->volitile[element]) {
            int **added_element = malloc(lvl->width * sizeof(int*));
            added_element[0] = malloc(lvl->height * lvl->width * sizeof(int));
            int **removed_element = malloc(lvl->width * sizeof(int*));
            removed_element[0] = malloc(lvl->height * lvl->width * sizeof(int));
            for(int i = 1; i < lvl->width; i++) {
                added_element[i] = added_element[0] + i * lvl->height;
                removed_element[i] = removed_element[0] + i * lvl->height;
            }
            for (int x = 0; x < lvl->width; x++) {
                for (int y = 0; y < lvl->height; y++) {
                    added_element[x][y] = 0;
                    removed_element[x][y] = 0;
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
                                if (lvl->tiles[xx][yy] != WALL && lvl->tiles[xx][yy] != CLOSED_DOOR && lvl->chemistry[x][y]->elements[element] - removed_element[x][y] > lvl->chemistry[xx][yy]->elements[element] + added_element[xx][yy]) {
                                    removed_element[x][y] += 1;
                                    added_element[xx][yy] += 1;
                                }
                            }
                        }
                    }
                }
            }
            for (int x = 0; x < lvl->width; x++) {
                for (int y = 0; y < lvl->height; y++) {
                    if (added_element[x][y] > 0 || removed_element[x][y] > 0) {
                        lvl->chemistry[x][y]->elements[element] += added_element[x][y] - removed_element[x][y];
                        lvl->chemistry[x][y]->stable = false;
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

        if (! init_colors()) {
            // would be nice to print terminal var here
            fprintf(stderr, "Terminal does not support color.\n");
            exit(1);
        }

        raw();
        noecho();
        cbreak();
        keypad(stdscr, TRUE);

        curs_set(0);

        lvl = make_level();
        draw(lvl);

        int turn = 0;

        // Main Loop
        do {
            fprintf(stderr, "=== Turn %3d ============================================\n", turn++);

            // Update the player before any of the mobs so they
            // react to the position where the player will be drawn
            move_mobile(lvl, lvl->mobs[lvl->mob_count - 1]);

            // Update all mobs other than the player
            for (int i=0; i < lvl->mob_count - 1; i++) {
                if (lvl->mobs[i]->active) {
                    //fprintf(stderr, "Moving mob #%d - %s\n", i, ((item*)lvl->mobs[i])->name);
                    step_mobile(lvl, lvl->mobs[i]);
                    //fprintf(stderr, "Mob #%d at (%d,%d) now\n", i, lvl->mobs[i]->x, lvl->mobs[i]->y);

                }

            }

            // Update chemistry model
            level_step_chemistry(lvl);

            // Prep death message if player is dead
            if (!lvl->player->active) {
                print_message("You die");
            }

            // Update the screen
            draw(lvl);

            // If the player is dead, wait for input
            if (!lvl->player->active) {
                getch();
                break;
            }

            // Reset message
            print_message("");

        } while (get_input(lvl) == 0);

        destroy_level(lvl);
        endwin();
        return 0;
}
