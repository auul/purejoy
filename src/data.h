#ifndef JOY_DATA_H
#define JOY_DATA_H

#include "global.h"

// Data Types

enum joy_data_type {
	JOY_DATA_NIL,
	JOY_DATA_BOOL,
	JOY_DATA_INT,
	JOY_DATA_FLOAT,
	JOY_DATA_SET,
	JOY_DATA_CHAR,
	JOY_DATA_STR,
	JOY_DATA_FILE,
	JOY_DATA_LIST,
	JOY_DATA_WORD,
	JOY_DATA_MAP,
};

// Constants

static const Joy_Data JOY_DATA_MAX = UINTPTR_MAX;

// Functions

Joy_Data data_bool(Joy_Bool bnum);
Joy_Data data_int(Joy_Int inum);
Joy_Data data_float(Joy_Float fnum);
Joy_Data data_set(Joy_Set set);
Joy_Data data_char(Joy_Char cnum);
Joy_Data data_str(Joy_Str *str);
Joy_Data data_file(Joy_File *file);
Joy_Data data_list(Joy_List *list);
Joy_Data data_word(Joy_Word *word);
Joy_Data data_map(Joy_Map *map);
Joy_DataType data_get_type(const Joy_Data data);
Joy_Bool data_get_bool(const Joy_Data data);
Joy_Int data_get_int(const Joy_Data data);
Joy_Float data_get_float(const Joy_Data data);
Joy_Set data_get_set(const Joy_Data data);
Joy_Char data_get_char(const Joy_Data data);
void *data_get_ptr(const Joy_Data data);
void data_print(const Joy_Data data);
void data_mark(Joy_Data data);

#endif
