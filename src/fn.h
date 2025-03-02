#ifndef JOY_FN_H
#define JOY_FN_H

#include "global.h"

// Functions

Joy_Fn new_fn_built_in(bool (*built_in)(Joy_State *));
void fn_print(const Joy_Fn fn);
void fn_mark(Joy_Fn fn);
bool fn_eval(Joy_State *J, Joy_Fn fn);

#endif
