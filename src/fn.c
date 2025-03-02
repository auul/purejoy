#include "fn.h"
#include "alloc.h"
#include "data.h"
#include "list.h"
#include "state.h"

Joy_Fn new_fn_built_in(bool (*built_in)(Joy_State *))
{
	return ((Joy_Fn)built_in << 1) | 1;
}

static bool is_built_in(const Joy_Fn fn)
{
	return fn & 1;
}

static void *get_ptr(const Joy_Fn fn)
{
	return (void *)(fn >> 1);
}

void fn_print(const Joy_Fn fn)
{
	if (is_built_in(fn)) {
		printf("built-in:%p", get_ptr(fn));
	} else {
		printf("fn:%p", get_ptr(fn));
	}
}

void fn_mark(Joy_Fn fn)
{
	if (is_built_in(fn)) {
		return;
	}
	alloc_mark(get_ptr(fn));
}

bool fn_eval(Joy_State *J, Joy_Fn fn)
{
	if (is_built_in(fn)) {
		bool (*built_in)(Joy_State *) = get_ptr(fn);
		return built_in(J);
	}

	J->nest = list_push(J->nest, data_list(J->ctrl));
	J->ctrl = get_ptr(fn);

	return true;
}
