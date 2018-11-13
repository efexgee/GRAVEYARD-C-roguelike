#include "../curses/color.h"

// Tiles
#define TILE_WALL '#'
#define TILE_FLOOR '.'
#define TILE_UNSEEN ' '

// Scenery
#define DOOR_OPEN '-'
#define DOOR_CLOSED '+'
#define DOOR_LOCKED DOOR_CLOSED

// Effects
#define STATUS_BURNING '*' | TEXT_RED

// Mobs
#define ICON_HUMAN '@' | TEXT_YELLOW
#define ICON_GOBLIN 'o' | TEXT_GREEN
#define ICON_ORC 'O'
#define ICON_UMBER_HULK_AWAKE 'U'
#define ICON_UMBER_HULK_ASLEEP 'z'
#define ICON_MINOTAUR 'M' | TEXT_RED

#define ICON_CHARGING '>'

// Items
#define ICON_ASH '~'
#define ICON_POTION '!'
#define ICON_STICK '_'

// Emotes
#define EMOTE_ANGRY '!' | TEXT_RED

// Other

// Initialization value only and should never be seen
// May render as a black question mark on a white
// lozenge, or an upside down question mark.
#define ICON_UNDEFINED '\277'
