#ifndef COLOR_H
#define COLOR_H

#include <ncurses.h>

// Colors
#define ORIG_COLORS -1
#define BLACK 0
#define RED 1
#define GREEN 2
#define YELLOW 3
#define BLUE 4
#define MAGENTA 5
#define CYAN 6
#define WHITE 7

// Text color pairs
#define TEXT_BLACK COLOR_PAIR(BLACK)
#define TEXT_RED COLOR_PAIR(RED)
#define TEXT_GREEN COLOR_PAIR(GREEN)
#define TEXT_YELLOW COLOR_PAIR(YELLOW)
#define TEXT_BLUE COLOR_PAIR(BLUE)
#define TEXT_MAGENTA COLOR_PAIR(MAGENTA)
#define TEXT_CYAN COLOR_PAIR(CYAN)
#define TEXT_WHITE COLOR_PAIR(WHITE)

bool init_colors();

#endif
