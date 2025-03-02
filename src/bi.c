#include "bi.h"
#include "data.h"
#include "int.h"
#include "map.h"
#include "set.h"
#include "state.h"

#include <math.h>
#include <stdlib.h>
#include <time.h>

bool bi_exit(Joy_State *J)
{
	state_exit(J);
	return true;
}

bool bi_false(Joy_State *J)
{
	return state_push_to_stack(J, data_bool(false));
}

bool bi_true(Joy_State *J)
{
	return state_push_to_stack(J, data_bool(true));
}

bool bi_maxint(Joy_State *J)
{
	return state_push_to_stack(J, data_int(JOY_INT_MAX));
}

bool bi_setsize(Joy_State *J)
{
	return state_push_to_stack(J, data_int(JOY_SET_MAX + 1));
}

bool bi_stack(Joy_State *J)
{
	return state_push_to_stack(J, data_list(state_get_stack(J)));
}

bool bi_conts(Joy_State *J)
{
	return state_push_to_stack(J, data_list(state_get_conts(J)));
}

bool bi_autoput(Joy_State *J)
{
	return state_push_to_stack(J, data_int(state_get_autoput(J)));
}

bool bi_undeferror(Joy_State *J)
{
	return state_push_to_stack(J, data_int(state_get_undeferror(J)));
}

bool bi_undefs(Joy_State *J)
{
	return state_push_to_stack(J, data_list(map_get_undefs(state_get_map(J))));
}

bool bi_echo(Joy_State *J)
{
	return state_push_to_stack(J, data_int(state_get_echo(J)));
}

bool bi_clock(Joy_State *J)
{
	double usage_in_seconds = clock() / CLOCKS_PER_SEC;
	Joy_Int usage_in_centiseconds = (Joy_Int)round(usage_in_seconds * 100);
	return state_push_to_stack(J, data_int(usage_in_centiseconds));
}

bool bi_time(Joy_State *J)
{
	return state_push_to_stack(J, data_int(time(NULL)));
}

bool bi_rand(Joy_State *J)
{
	return state_push_to_stack(J, data_int(rand()));
}

bool bi_stdin(Joy_State *J)
{
	return state_push_to_stack(J, data_file(stdin));
}

bool bi_stdout(Joy_State *J)
{
	return state_push_to_stack(J, data_file(stdout));
}

bool bi_stderr(Joy_State *J)
{
	return state_push_to_stack(J, data_file(stderr));
}

bool bi_id(Joy_State *J)
{
	return true;
}

bool bi_dup(Joy_State *J)
{
	Joy_Data value = state_pop_stack(J);
	return state_push_to_stack(J, value) && state_push_to_stack(J, value);
}

bool bi_swap(Joy_State *J)
{
	Joy_Data b = state_pop_stack(J);
	Joy_Data a = state_pop_stack(J);
	return state_push_to_stack(J, b) && state_push_to_stack(J, a);
}

bool bi_rollup(Joy_State *J)
{
	Joy_Data c = state_pop_stack(J);
	Joy_Data b = state_pop_stack(J);
	Joy_Data a = state_pop_stack(J);
	return state_push_to_stack(J, c) && state_push_to_stack(J, a)
	    && state_push_to_stack(J, b);
}
