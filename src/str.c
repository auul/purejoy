#include "str.h"
#include "alloc.h"
#include "char.h"

#include <ctype.h>
#include <string.h>

Joy_Str *new_str(const char *src)
{
	if (!src || !src[0]) {
		return NULL;
	}
	return new_str_n(src, strlen(src));
}

Joy_Str *new_str_n(const char *src, size_t width)
{
	Joy_Str *str = new_alloc(width + 1);
	memcpy(str, src, width);
	str[width] = 0;

	return str;
}

static bool is_esc_char(const char ch)
{
	switch (ch) {
	case '\a':
	case '\b':
	case '\e':
	case '\f':
	case '\n':
	case '\r':
	case '\t':
	case '\v':
	case '\\':
	case '"':
		return true;
	default:
		return !isgraph(ch);
	}
}

static int get_print_span(const Joy_Str *str)
{
	int span = 0;
	while (!is_esc_char(str[span])) {
		span++;
	}
	return span;
}

void str_print(const Joy_Str *str)
{
	if (!str) {
		printf("\"\"");
		return;
	}

	printf("\"");
	while (*str) {
		int span = get_print_span(str);
		if (span) {
			printf("%.*s", span, str);
			str += span;
		} else {
			char_print_esc(*str);
			str++;
		}
	}
	printf("\"");
}

void str_mark(Joy_Str *str)
{
	alloc_mark(str);
}
