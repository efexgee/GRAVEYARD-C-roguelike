#include <time.h>
#include <stdlib.h>
#include <ncurses.h>

#define LEVEL_WIDTH  200
#define LEVEL_HEIGHT 100
char level[LEVEL_HEIGHT][LEVEL_WIDTH];

enum Behavior {RandomWalk};
struct dood {
    char display;
    int x;
    int y;
    enum Behavior behavior;
};

struct dood player;

#define MAX_DOODS 1000
struct dood goblins[MAX_DOODS];

bool is_position_valid(int x, int y) {
    return level[y][x] != '#';
}

void draw_dood(struct dood *d, int dx, int dy) {
    mvprintw(dy+d->y, dx+d->x, "%c", d->display);
}

void draw() {
    int row,col;
    getmaxyx(stdscr,row,col);

    int dx = col/2 - player.x;
    int dy = row/2 - player.y;

    for (int x = 0; x < col; x++) {
        for (int y = 0; y < row; y++) {
            int xx = x - dx;
            int yy = y - dy;
            if (xx >= 0 && xx < LEVEL_WIDTH && yy >= 0 && yy < LEVEL_HEIGHT) {
                mvprintw(y, x, "%c", level[yy][xx]);
            } else {
                mvprintw(y, x, " ");
            }
        }
    }
    for (int i=0; i < MAX_DOODS; i++) {
        draw_dood(&goblins[i], dx, dy);
    }
    draw_dood(&player, dx, dy);
}

void setup_level() {
    for (int x = 0; x < LEVEL_WIDTH; x++) {
        for (int y = 0; y < LEVEL_HEIGHT; y++) {
            if (x == 0 || y == 0 || y == LEVEL_HEIGHT-1 || x == LEVEL_WIDTH-1) {
                level[y][x] = '#';
            } else {
                level[y][x] = '.';
            }
        }
    }
    player.x = player.y = 1;
    player.display = '@';
    for (int i=0; i < MAX_DOODS; i++) {
        goblins[i].x = rand()%(LEVEL_WIDTH-2) + 1;
        goblins[i].y = rand()%(LEVEL_HEIGHT-2) + 1;
        goblins[i].display = 'o';
        goblins[i].behavior = RandomWalk;
    }
}

int move_player() {
    int x = player.x;
    int y = player.y;
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
        case 'q':
            return -1;
    }

    if (is_position_valid(x, y)) {
        player.x = x;
        player.y = y;
    }
    return 0;
}

void move_dood(struct dood *d) {
    int x = d->x;
    int y = d->y;
    switch (d->behavior) {
        case RandomWalk:
            if (rand()%2 == 0) {
                x += rand()%3 - 1;
            } else {
                y += rand()%3 - 1;
            }
    }
    if (is_position_valid(x, y)) {
        d->x = x;
        d->y = y;
    }
}

void main() {
        int ch;
        srand(time(NULL));
        initscr();
        raw();
        noecho();
        cbreak();
        keypad(stdscr, TRUE);

        curs_set(0);

        setup_level();
        draw();
        while (move_player() == 0) {
            for (int i=0; i < MAX_DOODS; i++) {
                move_dood(&goblins[i]);
            }
            draw();
        }


        endwin();
}
