#include "data.h"
#include "bool.h"
#include "char.h"
#include "file.h"
#include "float.h"
#include "int.h"
#include "list.h"
#include "map.h"
#include "set.h"
#include "str.h"
#include "word.h"

static const size_t TYPE_BITS = 4;
static const size_t DATA_BITS = sizeof(Joy_Data) * 8;
static const size_t VALUE_BITS = DATA_BITS - TYPE_BITS;
static const Joy_Data VALUE_MASK = JOY_DATA_MAX >> TYPE_BITS;

static Joy_Data tag_value(Joy_DataType type, Joy_Data value)
{
	return ((Joy_Data)type << VALUE_BITS) | (value & VALUE_MASK);
}

Joy_Data data_bool(Joy_Bool bnum)
{
	return tag_value(JOY_DATA_BOOL, (Joy_Data)bnum);
}

Joy_Data data_int(Joy_Int inum)
{
	return tag_value(JOY_DATA_INT, (Joy_Data)inum);
}

Joy_Data data_float(Joy_Float fnum)
{
	Joy_Data data;
	*((Joy_Float *)&data) = fnum;
	return data >> TYPE_BITS;
}

Joy_Data data_set(Joy_Set set)
{
	return tag_value(JOY_DATA_SET, (Joy_Data)set);
}

Joy_Data data_char(Joy_Char cnum)
{
	return tag_value(JOY_DATA_CHAR, (Joy_Data)cnum);
}

Joy_Data data_str(Joy_Str *str)
{
	return tag_value(JOY_DATA_STR, (Joy_Data)str);
}

Joy_Data data_file(Joy_File *file)
{
	return tag_value(JOY_DATA_FILE, (Joy_Data)file);
}

Joy_Data data_list(Joy_List *list)
{
	return tag_value(JOY_DATA_LIST, (Joy_Data)list);
}

Joy_Data data_word(Joy_Word *word)
{
	return tag_value(JOY_DATA_WORD, (Joy_Data)word);
}

Joy_Data data_map(Joy_Map *map)
{
	return tag_value(JOY_DATA_MAP, (Joy_Data)map);
}

Joy_DataType data_get_type(const Joy_Data data)
{
	return (Joy_DataType)(data >> VALUE_BITS);
}

static Joy_Data get_value(const Joy_Data data)
{
	return data & VALUE_MASK;
}

Joy_Bool data_get_bool(const Joy_Data data)
{
	return (Joy_Bool)get_value(data);
}

Joy_Int data_get_int(const Joy_Data data)
{
	return (Joy_Int)get_value(data);
}

Joy_Float data_get_float(const Joy_Data data)
{
	const Joy_Data data_shifted = data << TYPE_BITS;
	return *((Joy_Float *)&data);
}

Joy_Set data_get_set(const Joy_Data data)
{
	return (Joy_Set)get_value(data);
}

Joy_Char data_get_char(const Joy_Data data)
{
	return (Joy_Char)get_value(data);
}

void *data_get_ptr(const Joy_Data data)
{
	return (void *)get_value(data);
}

void data_print(const Joy_Data data)
{
	switch (data_get_type(data)) {
	case JOY_DATA_NIL:
		printf("nil");
		break;
	case JOY_DATA_BOOL:
		bool_print(data_get_int(data));
		break;
	case JOY_DATA_INT:
		int_print(data_get_int(data));
		break;
	case JOY_DATA_FLOAT:
		float_print(data_get_float(data));
		break;
	case JOY_DATA_SET:
		set_print(data_get_set(data));
		break;
	case JOY_DATA_CHAR:
		char_print(data_get_char(data));
		break;
	case JOY_DATA_STR:
		str_print(data_get_ptr(data));
		break;
	case JOY_DATA_FILE:
		file_print(data_get_ptr(data));
		break;
	case JOY_DATA_LIST:
		list_print(data_get_ptr(data));
		break;
	case JOY_DATA_WORD:
		word_print(data_get_ptr(data));
		break;
	case JOY_DATA_MAP:
		map_print(data_get_ptr(data));
		break;
	default:
		printf("%zu", data);
		break;
	}
}

void data_mark(Joy_Data data)
{
	switch (data_get_type(data)) {
	case JOY_DATA_STR:
		str_mark(data_get_ptr(data));
		break;
	case JOY_DATA_LIST:
		list_print(data_get_ptr(data));
		break;
	case JOY_DATA_WORD:
		word_print(data_get_ptr(data));
		break;
	case JOY_DATA_MAP:
		map_print(data_get_ptr(data));
		break;
	default:
		break;
	}
}
