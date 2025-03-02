#ifndef JOY_MAP_H
#define JOY_MAP_H

#include "global.h"

// Data Types

struct joy_map {
	Joy_Word *key;
	Joy_Fn value;
	Joy_Map *left;
	Joy_Map *right;
};

// Constants

static const Joy_Fn JOY_FN_NIL = (Joy_Fn)0;

// Functions

void map_print(const Joy_Map *map);
void map_mark(Joy_Map *map);
Joy_Map *map_get_first(Joy_Map *map);
Joy_Map *map_get_next(Joy_Map *map, Joy_Map *node);
Joy_Fn map_lookup(const Joy_Map *map, const char *key);
Joy_Map *map_define(Joy_Map *map, const char *key, Joy_Fn value);
Joy_Word *map_touch_n(Joy_Map **map_p, const char *key, size_t len);
Joy_List *map_get_undefs(Joy_Map *map);

#endif
