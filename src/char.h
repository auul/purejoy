#ifndef JOY_CHAR_H
#define JOY_CHAR_H

#include "global.h"

// Constants

static const Joy_Char JOY_CHAR_MAX = 2097151;

// Functions

void char_print(Joy_Char cnum);
void char_print_esc(Joy_Char cnum);
void char_print_utf8(Joy_Char cnum);
char *char_write_utf8(char *dest, Joy_Char cnum);

#endif
