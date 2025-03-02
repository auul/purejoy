#ifndef JOY_LIST_H
#define JOY_LIST_H

#include "global.h"

#include <stdarg.h>

// Data Types

struct joy_list {
	Joy_Data car;
	Joy_List *cdr;
};

// Constants

extern char JOY_LIST_END_SENTINEL;
#define JOY_LIST_END data_list((Joy_List *)&JOY_LIST_END_SENTINEL)

// Functions

Joy_List *new_list(Joy_Data car, ...);
Joy_List *new_list_v(Joy_Data car, va_list args);
void list_print(const Joy_List *list);
void list_mark(Joy_List *list);
Joy_List *list_push(Joy_List *list, Joy_Data value);
Joy_Data list_pop(Joy_List **list_p);

#endif
