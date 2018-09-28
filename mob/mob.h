#ifndef INC_MOB_H
#define INC_MOB_H
#include <stdbool.h>
#include <stdlib.h>

enum Behavior {RandomWalk, KeyboardInput};

typedef struct Item {
    char display;
    char *name;
} item;

typedef struct InventoryItem {
    item* item;
    struct InventoryItem* next;
} inventory_item;

typedef struct Mobile {
    char display;
    char *name;
    int x;
    int y;
    bool active;
    bool stacks;
    enum Behavior behavior;
    char emote;
    inventory_item *inventory;
} mobile;

#define ICON_HUMAN '@'
#define ICON_GOBLIN 'o'
#define ICON_ORC 'O'

#define OUCH '!'

mobile* make_mob();
void destroy_mob(mobile *mob);
void push_inventory(mobile* mob, item* itm);
item* pop_inventory(mobile* mob);
char* inventory_string(mobile* mob, int len);
#endif
