#ifndef JOY_STATE_H
#define JOY_STATE_H

#include "global.h"

// Data Types

struct joy_state {
	unsigned running    : 1;
	unsigned autoput    : 2;
	unsigned undeferror : 1;
	unsigned echo       : 2;

	Joy_Map *map;

	Joy_List *ctrl;
	Joy_List *nest;
	Joy_List *stack;
};

// Functions

void state_init(Joy_State *J);
void state_debug(const Joy_State *J);
void state_exit(Joy_State *J);
unsigned state_get_autoput(Joy_State *J);
unsigned state_get_undeferror(Joy_State *J);
unsigned state_get_echo(Joy_State *J);
Joy_List *state_get_stack(Joy_State *J);
Joy_List *state_get_conts(Joy_State *J);
Joy_Map *state_get_map(Joy_State *J);
bool state_push_to_stack(Joy_State *J, Joy_Data data);
Joy_Data state_pop_stack(Joy_State *J);
bool state_eval(Joy_State *J, Joy_List *ctrl);

#endif
