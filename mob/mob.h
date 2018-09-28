#ifndef INC_MOB_H
#define INC_MOB_H

enum Behavior {RandomWalk, KeyboardInput};

typedef struct Mobile {
    char display;
    int x;
    int y;
    bool active;
    bool stacks;
    enum Behavior behavior;
    char emote;
} mobile;

#define ICON_HUMAN '@'
#define ICON_GOBLIN 'o'
#define ICON_ORC 'O'

#define OUCH '!'

#endif
