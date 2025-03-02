#include "list.h"
#include "alloc.h"
#include "data.h"

char JOY_LIST_END_SENTINEL;

static Joy_List *create_list_node(Joy_Data car, Joy_List *cdr)
{
	Joy_List *node = new_alloc(sizeof(Joy_List));
	node->car = car;
	node->cdr = cdr;

	return node;
}

Joy_List *new_list(Joy_Data car, ...)
{
	va_list args;
	va_start(args, car);
	Joy_List *list = new_list_v(car, args);
	va_end(args);

	return list;
}

Joy_List *new_list_v(Joy_Data car, va_list args)
{
	Joy_List *list = NULL;
	Joy_List *node = NULL;

	while (car != JOY_LIST_END) {
		if (list) {
			node->cdr = create_list_node(car, NULL);
			node = node->cdr;
		} else {
			list = create_list_node(car, NULL);
			node = list;
		}
		car = va_arg(args, Joy_Data);
	}

	return list;
}

void list_print(const Joy_List *list)
{
	printf("[");
	while (list) {
		data_print(list->car);
		list = list->cdr;
		if (list) {
			printf(" ");
		}
	}
	printf("]");
}

void list_mark(Joy_List *list)
{
	while (!alloc_mark(list)) {
		data_mark(list->car);
		list = list->cdr;
	}
}

Joy_List *list_push(Joy_List *list, Joy_Data value)
{
	return create_list_node(value, list);
}

Joy_Data list_pop(Joy_List **list_p)
{
	Joy_List *list = *list_p;
	if (!list) {
		return JOY_DATA_NIL;
	}
	*list_p = list->cdr;

	return list->car;
}
