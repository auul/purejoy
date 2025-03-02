#ifndef JOY_SET_H
#define JOY_SET_H

#include "global.h"

// Constants

static const Joy_Set JOY_SET_EMPTY = (Joy_Set)0;
static const unsigned JOY_SET_MAX = sizeof(Joy_Set) * 8 - 1;

// Functions

void set_print(Joy_Set set);
Joy_Set set_add(Joy_Set set, unsigned member);

#endif
