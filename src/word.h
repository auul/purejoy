#ifndef JOY_WORD_H
#define JOY_WORD_H

#include "global.h"

#include <stddef.h>

// Functions

Joy_Word *new_word(const char *src);
Joy_Word *new_word_n(const char *src, size_t len);
void word_print(const Joy_Word *word);
void word_mark(Joy_Word *word);
int word_compare(const Joy_Word *left, const Joy_Word *right);
int word_compare_n(const Joy_Word *left,
                   const Joy_Word *right,
                   size_t right_len);

#endif
