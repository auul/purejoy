#include "state.h"
#include "bi.h"
#include "data.h"
#include "fn.h"
#include "list.h"
#include "map.h"

static Joy_Map *
add_def(Joy_Map *map, const char *key, bool (*built_in)(Joy_State *))
{
	return map_define(map, key, new_fn_built_in(built_in));
}

static Joy_Map *init_map(void)
{
	Joy_Map *map = NULL;
	map = add_def(map, "exit", bi_exit);
	map = add_def(map, "false", bi_false);
	map = add_def(map, "true", bi_true);

	return map;
}

void state_init(Joy_State *J)
{
	J->autoput = 0;
	J->undeferror = 1;

	J->map = init_map();

	J->ctrl = NULL;
	J->nest = NULL;
	J->stack = NULL;
}

void state_debug(const Joy_State *J)
{
	printf("\nStack: ");
	list_print(J->stack);
	printf("\n");
}

void state_exit(Joy_State *J)
{
	J->running = false;
}

unsigned state_get_autoput(Joy_State *J)
{
	return J->autoput;
}

unsigned state_get_undeferror(Joy_State *J)
{
	return J->undeferror;
}

unsigned state_get_echo(Joy_State *J)
{
	return J->echo;
}

Joy_Map *state_get_map(Joy_State *J)
{
	return J->map;
}

Joy_List *state_get_stack(Joy_State *J)
{
	return J->stack;
}

Joy_List *state_get_conts(Joy_State *J)
{
	return J->nest;
}

bool state_push_to_stack(Joy_State *J, Joy_Data data)
{
	J->stack = list_push(J->stack, data);
	return true;
}

Joy_Data state_pop_stack(Joy_State *J)
{
	return list_pop(&J->stack);
}

static bool eval_word(Joy_State *J, Joy_Word *word)
{
	Joy_Fn fn = map_lookup(J->map, word);
	if (fn == JOY_FN_NIL) {
		if (J->undeferror) {
			fprintf(stderr, "Joy Error: %s is undefined", word);
			return false;
		}
		return state_push_to_stack(J, data_word(word));
	}
	return fn_eval(J, fn);
}

bool state_eval(Joy_State *J, Joy_List *ctrl)
{
	J->ctrl = ctrl;
	J->nest = NULL;

	do {
		while (J->ctrl) {
			Joy_Data value = list_pop(&J->ctrl);
			switch (data_get_type(value)) {
			case JOY_DATA_WORD:
				if (!eval_word(J, data_get_ptr(value))) {
					return false;
				}
				break;
			default:
				J->stack = list_push(J->stack, value);
				break;
			}
		}

		while (J->nest && !J->ctrl) {
			J->ctrl = data_get_ptr(list_pop(&J->nest));
		}
	} while (J->ctrl);

	return true;
}
