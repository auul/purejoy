#include "map.h"
#include "alloc.h"
#include "data.h"
#include "fn.h"
#include "list.h"
#include "word.h"

static void print_node(const Joy_Map *node)
{
	word_print(node->key);
	printf(": ");
	fn_print(node->value);
	printf("\n");
}

void map_print(const Joy_Map *map)
{
	while (map) {
		if (map->left) {
			map_print(map->left);
		}
		print_node(map);

		map = map->right;
	}
}

void map_mark(Joy_Map *map)
{
	while (!alloc_mark(map)) {
		word_mark(map->key);
		fn_mark(map->value);
		map_mark(map->left);

		map = map->right;
	}
}

Joy_Map *map_get_first(Joy_Map *map)
{
	if (!map) {
		return NULL;
	}

	while (map->left) {
		map = map->left;
	}
	return map;
}

Joy_Map *map_get_next(Joy_Map *map, Joy_Map *node)
{
	if (!map) {
		return NULL;
	} else if (node->right) {
		return map_get_first(node->right);
	}

	Joy_Map *next = map;

	while (true) {
		int cmp = word_compare(map->key, node->key);
		if (cmp < 0) {
			next = map;
			map = map->left;
		} else if (cmp > 0) {
			map = map->right;
		} else {
			return next;
		}
	}
}

Joy_Fn map_lookup(const Joy_Map *map, const char *key)
{
	while (map) {
		int cmp = word_compare(map->key, key);
		if (cmp < 0) {
			map = map->left;
		} else if (cmp > 0) {
			map = map->right;
		} else {
			return map->value;
		}
	}
	return JOY_FN_NIL;
}

static Joy_Map *
create_node(Joy_Word *key, Joy_Fn value, Joy_Map *left, Joy_Map *right)
{
	Joy_Map *node = new_alloc(sizeof(Joy_Map));
	node->key = key;
	node->value = value;
	node->left = left;
	node->right = right;

	return node;
}

static Joy_Map *clone_node(Joy_Map *src)
{
	return create_node(src->key, src->value, src->left, src->right);
}

static Joy_Map *new_node(const char *key, Joy_Fn value)
{
	return create_node(new_word(key), value, NULL, NULL);
}

Joy_Map *map_define(Joy_Map *map, const char *key, Joy_Fn value)
{
	if (!map) {
		return new_node(key, value);
	}

	map = clone_node(map);
	Joy_Map *node = map;

	while (true) {
		int cmp = word_compare(node->key, key);
		if (cmp < 0) {
			if (!node->left) {
				node->left = new_node(key, value);
				return map;
			}

			node->left = clone_node(node->left);
			node = node->left;
		} else if (cmp > 0) {
			if (!node->right) {
				node->right = new_node(key, value);
				return map;
			}

			node->right = clone_node(node->right);
			node = node->right;
		} else {
			node->value = value;
		}
	}
}

static Joy_Map *touch_node_n(const char *key, size_t len)
{
	return create_node(new_word_n(key, len), JOY_FN_NIL, NULL, NULL);
}

Joy_Word *map_touch_n(Joy_Map **map_p, const char *key, size_t len)
{
	Joy_Map *map = *map_p;
	while (map) {
		int cmp = word_compare_n(map->key, key, len);
		if (cmp < 0) {
			map = map->left;
		} else if (cmp > 0) {
			map = map->right;
		} else {
			return map->key;
		}
	}

	map = *map_p;
	if (!map) {
		map = touch_node_n(key, len);
		*map_p = map;

		return map->key;
	}

	map = clone_node(map);
	*map_p = map;

	while (true) {
		int cmp = word_compare_n(map->key, key, len);
		if (cmp < 0) {
			if (!map->left) {
				map->left = touch_node_n(key, len);
				return map->left->key;
			}

			map->left = clone_node(map->left);
			map = map->left;
		} else {  // cmp > 0
			if (!map->right) {
				map->right = touch_node_n(key, len);
				return map->right->key;
			}

			map->right = clone_node(map->right);
			map = map->right;
		}
	}
}

Joy_List *map_get_undefs(Joy_Map *map)
{
	Joy_List *list = NULL;
	Joy_List *end = NULL;
	Joy_Map *node = map_get_first(map);

	while (node) {
		if (node->value == JOY_FN_NIL) {
			if (list) {
				end->cdr = list_push(NULL, data_word(node->key));
				end = end->cdr;
			} else {
				list = list_push(NULL, data_word(node->key));
				end = list;
			}
		}
		node = map_get_next(map, node);
	}
	return list;
}
