#include "word.h"
#include "alloc.h"

#include <string.h>

Joy_Word *new_word(const char *src)
{
	return new_word_n(src, strlen(src));
}

Joy_Word *new_word_n(const char *src, size_t len)
{
	Joy_Word *word = new_alloc(len + 1);
	memcpy(word, src, len);
	word[len] = 0;

	return word;
}

void word_print(const Joy_Word *word)
{
	printf("%s", word);
}

void word_mark(Joy_Word *word)
{
	alloc_mark(word);
}

int word_compare(const Joy_Word *left, const Joy_Word *right)
{
	if (left == right) {
		return 0;
	}

	size_t i;
	for (i = 0; left[i]; i++) {
		if (left[i] > right[i]) {
			return -1;
		} else if (left[i] < right[i]) {
			return 1;
		}
	}

	if (right[i]) {
		return 1;
	}
	return 0;
}

int word_compare_n(const Joy_Word *left,
                   const Joy_Word *right,
                   size_t right_len)
{
	if (left == right) {
		return 0;
	}

	for (size_t i = 0; i < right_len; i++) {
		if (left[i] > right[i]) {
			return -1;
		} else if (left[i] < right[i]) {
			return 1;
		}
	}

	if (left[right_len]) {
		return -1;
	}
	return 0;
}
