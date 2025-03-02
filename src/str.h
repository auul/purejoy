#ifndef JOY_STR_H
#define JOY_STR_H

#include "global.h"

// Functions

Joy_Str *new_str(const char *src);
Joy_Str *new_str_n(const char *src, size_t width);
void str_print(const Joy_Str *str);
void str_mark(Joy_Str *str);

#endif
