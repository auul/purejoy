#ifndef JOY_BI_H
#define JOY_BI_H

#include "global.h"

// Functions

bool bi_exit(Joy_State *J);
bool bi_false(Joy_State *J);
bool bi_true(Joy_State *J);
bool bi_maxint(Joy_State *J);
bool bi_setsize(Joy_State *J);
bool bi_stack(Joy_State *J);
bool bi_conts(Joy_State *J);
bool bi_autoput(Joy_State *J);
bool bi_undeferror(Joy_State *J);
bool bi_undefs(Joy_State *J);
bool bi_echo(Joy_State *J);
bool bi_clock(Joy_State *J);
bool bi_time(Joy_State *J);
bool bi_rand(Joy_State *J);
bool bi_stdin(Joy_State *J);
bool bi_stdout(Joy_State *J);
bool bi_stderr(Joy_State *J);
bool bi_id(Joy_State *J);
bool bi_dup(Joy_State *J);
bool bi_swap(Joy_State *J);
bool bi_rollup(Joy_State *J);

#endif
