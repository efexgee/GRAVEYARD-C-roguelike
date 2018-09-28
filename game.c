#include <time.h>
#include <stdlib.h>
#include <ncurses.h>

#include "level/level.h"
#include "mob/mob.h"

int keyboard_x, keyboard_y = 0;


bool is_position_valid(level *lvl, int x, int y) {
    if (lvl->tiles[y][x] == WALL) return false;
    else {
        for (int i=0; i < lvl->mob_count; i++) {
            if (!lvl->mobs[i].stacks && lvl->mobs[i].x == x && lvl->mobs[i].y == y) {
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

    int dx = col/2 - lvl->player->x;
    int dy = row/2 - lvl->player->y;

    for (int x = 0; x < col; x++) {
        for (int y = 0; y < row; y++) {
            int xx = x - dx;
            int yy = y - dy;
            if (xx >= 0 && xx < lvl->width && yy >= 0 && yy < lvl->height) {
                mvprintw(y, x, "%c", lvl->tiles[yy][xx]);
            } else {
                mvprintw(y, x, " ");
            }
        }
    }
    for (int i=0; i < lvl->mob_count; i++) {
        if (lvl->mobs[i].active) {
            draw_mobile(&lvl->mobs[i], dx, dy);
        }
    }
    draw_mobile(lvl->player, dx, dy);
}

int get_input() {
    switch (getch()) {
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
        case 'q':
            return -1;
    }
    return 0;
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
        }
    }
}

void main() {
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
        while (get_input() == 0) {
            for (int i=0; i < lvl->mob_count; i++) {
                if (lvl->mobs[i].active) {
                    move_mobile(lvl, &lvl->mobs[i]);
                }
            }
            draw(lvl);
        }

        destroy_level(lvl);
        endwin();
}
