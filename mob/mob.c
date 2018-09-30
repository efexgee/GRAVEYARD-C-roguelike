#include <stdio.h>

#include "mob.h"

mobile* make_mob() {
    mobile *mob = malloc(sizeof(mobile));
    mob->display = ' ';
    mob->chemistry = make_constituents();
    mob->x = 0;
    mob->y = 0;
    mob->health = 1;
    mob->active = false;
    mob->stacks = false;
    mob->behavior = RandomWalk;
    mob->emote = false;
    mob->inventory = NULL;
    return mob;
}
void destroy_mob(mobile *mob) {
    destroy_constituents(mob->chemistry);
    inventory_item *inv = mob->inventory;
    while (inv != NULL) {
        inventory_item *next = inv->next;
        free((void*)inv);
        inv = next;
    }
    free((void*)mob->name);
    free((void*)mob);
}

void push_inventory(mobile* mob, item* itm) {
    inventory_item *new_entry = malloc(sizeof(inventory_item));
    new_entry->next = NULL;
    new_entry->item = itm;

    if (mob->inventory == NULL) {
        mob->inventory = new_entry;
    } else {
        inventory_item *inv = mob->inventory;
        while (inv->next != NULL) inv = inv->next;
        inv->next = new_entry;
    }
}

item* pop_inventory(mobile *mob) {
    if (mob->inventory == NULL) {
        return NULL;
    } else {
        inventory_item *old = mob->inventory;
        mob->inventory = old->next;
        item *itm = old->item;
        free(old);
        return itm;
    }
}

char* inventory_string(mobile* mob, int len) {
    char *str = malloc(len*sizeof(char));
    str[0] = '\0';
    int total = 0;
    inventory_item *inv = mob->inventory;
    bool first = true;

    while (inv != NULL) {
        inventory_item *next = inv->next;
        total += snprintf(str+total, len-total, first ? "%s" : ", %s", inv->item->name);
        first = false;
        if (total >= len) break;
        inv = next;
    }
    return str;
}

void rotate_inventory(mobile* mob) {
    if (mob->inventory != NULL && mob->inventory->next != NULL) {
        inventory_item *itm = mob->inventory;
        while (itm->next->next != NULL) itm = itm->next;
        inventory_item *last = itm->next;
        itm->next = NULL;
        last->next = mob->inventory;
        mob->inventory = last;
    }
}

void destroy_item(item *itm) {
    destroy_constituents(itm->chemistry);
    free((void*)itm);
}

bool quaff(mobile* mob) {
    if (mob->inventory != NULL && mob->inventory->item->type == Potion) {
        inventory_item* inv = mob->inventory;
        add_constituents(mob->chemistry, inv->item->chemistry);
        destroy_item(inv->item);
        mob->inventory = inv->next;
        mob->chemistry->stable = false;
        free((void*)inv);
        return true;
    }
    return false;
}
