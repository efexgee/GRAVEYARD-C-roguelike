#ifndef INC_MOB_H
#define INC_MOB_H
#include <stdbool.h>
#include <stdlib.h>

#include "../chemistry/chemistry.h"

enum Behavior {RandomWalk, KeyboardInput};
enum item_type {Weapon, Potion, Creature};

struct InventoryItem;

typedef struct {
    char display;
    char *name;
    constituents *chemistry;
    struct InventoryItem *contents;
    int health;
    enum item_type type;
} item;

typedef struct InventoryItem {
    item* item;
    struct InventoryItem* next;
} inventory_item;

typedef struct Mobile {
    item base;
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

mobile* make_mob();
void destroy_mob(mobile *mob);
void push_inventory(mobile* mob, item* itm);
item* pop_inventory(mobile* mob);
char* inventory_string(mobile* mob, int len);
void destroy_item(item *itm);

void rotate_inventory(mobile* mob);
bool quaff(mobile* mob);
#endif
