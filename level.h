#include <stdbool.h>
#include <stdlib.h>

enum Behavior {RandomWalk, KeyboardInput};

#define WALL '#'
#define FLOOR '.'
#define DOOR '-'

#define ICON_HUMAN '@'
#define ICON_GOBLIN 'o'
#define ICON_ORC 'O'

#define OUCH '!'

typedef struct Mobile {
    char display;
    int x;
    int y;
    bool active;
    bool stacks;
    enum Behavior behavior;
    char emote;
} mobile;

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
