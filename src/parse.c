#include "parse.h"
#include "alloc.h"
#include "buffer.h"
#include "char.h"
#include "data.h"
#include "float.h"
#include "int.h"
#include "list.h"
#include "set.h"
#include "str.h"
#include "word.h"

#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <string.h>

static const char *delim_sequence_list[] = {
	"(*",
	"*)",
	"#",
	"{",
	"}",
	"'",
	"\"",
	"[",
	"]",
	NULL,
};

Joy_ParseList *new_parse_list_node(Joy_ParseToken car, Joy_ParseList *cdr)
{
	Joy_ParseList *node = new_alloc(sizeof(Joy_ParseList));
	node->car = car;
	node->cdr = cdr;

	return node;
}

void parse_list_print(const Joy_ParseList *list)
{
	while (list) {
		parse_token_print(list->car);
		list = list->cdr;
		if (list) {
			printf(" ");
		}
	}
}

void parse_list_mark(Joy_ParseList *list)
{
	while (!alloc_mark(list)) {
		parse_token_mark(list->car);
		list = list->cdr;
	}
}

Joy_ParseToken parse_list_pop(Joy_ParseList **list_p)
{
	Joy_ParseList *list = *list_p;
	*list_p = list->cdr;

	return list->car;
}

static Joy_ParseToken tag_error(const char *src, size_t len, Joy_ParseType type)
{
	Joy_ParseToken token = {
		.src = src,
		.len = len,
		.type = type,
		.ptr = NULL,
	};
	return token;
}

static Joy_ParseToken tag_empty(const char *src, size_t len)
{
	Joy_ParseToken token = {
		.src = src,
		.len = len,
		.type = JOY_PARSE_EMPTY,
		.ptr = NULL,
	};
	return token;
}

static Joy_ParseToken tag_int(const char *src, size_t len, Joy_Int inum)
{
	Joy_ParseToken token = {
		.src = src,
		.len = len,
		.type = JOY_PARSE_INT,
		.inum = inum,
	};
	return token;
}

static Joy_ParseToken tag_float(const char *src, size_t len, Joy_Float fnum)
{
	Joy_ParseToken token = {
		.src = src,
		.len = len,
		.type = JOY_PARSE_FLOAT,
		.fnum = fnum,
	};
	return token;
}

static Joy_ParseToken
tag_set_member(const char *src, size_t len, unsigned member)
{
	Joy_ParseToken token = {
		.src = src,
		.len = len,
		.type = JOY_PARSE_SET_MEMBER,
		.member = member,
	};
	return token;
}

static Joy_ParseToken tag_set(const char *src, size_t len, Joy_Set set)
{
	Joy_ParseToken token = {
		.src = src,
		.len = len,
		.type = JOY_PARSE_SET,
		.set = set,
	};
	return token;
}

static Joy_ParseToken tag_char(const char *src, size_t len, Joy_Char cnum)
{
	Joy_ParseToken token = {
		.src = src,
		.len = len,
		.type = JOY_PARSE_CHAR,
		.cnum = cnum,
	};
	return token;
}

static Joy_ParseToken tag_str(const char *src, size_t len, Joy_Str *str)
{
	Joy_ParseToken token = {
		.src = src,
		.len = len,
		.type = JOY_PARSE_STR,
		.str = str,
	};
	return token;
}

static Joy_ParseToken tag_word(const char *src, size_t len, Joy_Word *word)
{
	Joy_ParseToken token = {
		.src = src,
		.len = len,
		.type = JOY_PARSE_WORD,
		.word = word,
	};
	return token;
}

static Joy_ParseToken tag_list(const char *src, size_t len, Joy_ParseList *list)
{
	Joy_ParseToken token = {
		.src = src,
		.len = len,
		.type = JOY_PARSE_LIST,
		.list = list,
	};
	return token;
}

void parse_token_print(const Joy_ParseToken token)
{
	switch (token.type) {
	case JOY_PARSE_EMPTY:
		printf("empty(%zu)", token.len);
		break;
	case JOY_PARSE_INT:
		int_print(token.inum);
		break;
	case JOY_PARSE_FLOAT:
		float_print(token.fnum);
		break;
	case JOY_PARSE_SET:
		set_print(token.set);
		break;
	case JOY_PARSE_SET_MEMBER:
		printf("set_member(%u)", token.member);
		break;
	case JOY_PARSE_CHAR:
		char_print(token.cnum);
		break;
	case JOY_PARSE_STR:
		str_print(token.str);
		break;
	case JOY_PARSE_WORD:
		word_print(token.word);
		break;
	case JOY_PARSE_LIST:
		printf("[");
		parse_list_print(token.list);
		printf("]");
		break;
	default:
		printf("error(%u)", token.type);
		break;
	}
}

void parse_token_mark(Joy_ParseToken token)
{
	switch (token.type) {
	case JOY_PARSE_STR:
		str_mark(token.str);
		break;
	case JOY_PARSE_WORD:
		word_mark(token.word);
		break;
	case JOY_PARSE_LIST:
		parse_list_mark(token.list);
		break;
	default:
		break;
	}
}

static bool is_token_type_error(Joy_ParseType type)
{
	return type >= JOY_PARSE_ERROR;
}

bool parse_token_is_error(const Joy_ParseToken token)
{
	return is_token_type_error(token.type);
}

static bool is_line_comment(const char *src)
{
	return src[0] == '#';
}

Joy_ParseToken parse_line_comment(const char *src)
{
	size_t len = 1;
	while (src[len] != '\n') {
		len++;
	}
	return tag_empty(src, len);
}

static bool is_comment_open(const char *src)
{
	return src[0] == '(' && src[1] == '*';
}

static bool is_comment_close(const char *src)
{
	return src[0] == '*' && src[1] == ')';
}

Joy_ParseToken parse_nested_comment(const char *src)
{
	unsigned level = 1;
	size_t len = 2;
	while (level) {
		if (!src[len]) {
			return tag_error(src, len, JOY_PARSE_ERROR_UNCLOSED_COMMENT);
		} else if (is_comment_open(src + len)) {
			level++;
			len += 2;
		} else if (is_comment_close(src + len)) {
			level--;
			len += 2;
		} else {
			len++;
		}
	}
	return tag_empty(src, len);
}

Joy_ParseToken parse_empty(const char *src)
{
	size_t len = 0;
	while (true) {
		if (isspace(src[len])) {
			len++;
		} else if (is_line_comment(src + len)) {
			Joy_ParseToken comment = parse_line_comment(src + len);
			len += comment.len;
		} else if (is_comment_open(src + len)) {
			Joy_ParseToken comment = parse_nested_comment(src + len);
			if (is_token_type_error(comment.type)) {
				return comment;
			}
			len += comment.len;
		} else {
			return tag_empty(src, len);
		}
	}
}

static bool is_partial_match(const char *target, const char *src)
{
	for (size_t i = 0; target[i]; i++) {
		if (target[i] != src[i]) {
			return false;
		}
	}
	return true;
}

static const char *get_delim_sequence(const char *src)
{
	for (unsigned i = 0; delim_sequence_list[i]; i++) {
		if (is_partial_match(delim_sequence_list[i], src)) {
			return delim_sequence_list[i];
		}
	}
	return NULL;
}

static bool is_delim(const char *src)
{
	return src[0] == 0 || isspace(src[0]) || get_delim_sequence(src);
}

static bool is_tribit_literal(char c)
{
	return c >= '0' && c <= '7';
}

static Joy_Int get_tribit_literal(char c)
{
	return (Joy_Int)(c - '0');
}

static bool is_octal_literal(const char *src)
{
	if (src[0] == '-') {
		src++;
	}
	return src[0] == '0' && is_tribit_literal(src[1]);
}

static bool is_octal_overflow(Joy_Int inum)
{
	// Tests if the highest bit can be shifted by 3 bits without overflow
	return inum >= (JOY_INT_MAX & ~(JOY_INT_MAX >> 1)) >> 2;
}

static size_t find_token_end(const char *src, size_t len)
{
	while (!is_delim(src + len)) {
		len++;
	}
	return len;
}

Joy_ParseToken parse_octal_literal(const char *src)
{
	bool negative;
	size_t len;

	if (src[0] == '-') {
		negative = true;
		len = 2;
	} else {
		negative = false;
		len = 1;
	}

	Joy_Int inum = 0;

	while (!is_delim(src + len)) {
		if (is_tribit_literal(src[len])) {
			if (is_octal_overflow(inum)) {
				return tag_error(src,
				                 find_token_end(src, len),
				                 JOY_PARSE_ERROR_NUM_OVERFLOW);
			}
			inum = (inum << 3) | get_tribit_literal(src[len]);
		} else {
			return tag_error(src, len + 1, JOY_PARSE_ERROR_BAD_NUM_CHAR);
		}
		len++;
	}

	if (negative) {
		inum *= -1;
	}

	return tag_int(src, len, inum);
}

static bool is_nibble_literal(char c)
{
	return (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F') || isdigit(c);
}

static Joy_Int get_nibble_literal(char c)
{
	if (c >= 'a' && c <= 'f') {
		return c - 'a' + 10;
	} else if (c >= 'A' && c <= 'F') {
		return c - 'A' + 10;
	} else {
		return c - '0';
	}
}

static bool is_hex_literal(const char *src)
{
	if (src[0] == '-') {
		src++;
	}
	return src[0] == '0' && (src[1] == 'x' || src[1] == 'X');
}

static bool is_hex_overflow(Joy_Int inum)
{
	// Tests if the highest bit can be shifted by 4 bits without overflow
	return inum >= (JOY_INT_MAX & ~(JOY_INT_MAX >> 1)) >> 3;
}

Joy_ParseToken parse_hex_literal(const char *src)
{
	bool negative;
	size_t len;

	if (src[0] == '-') {
		negative = true;
		len = 3;
	} else {
		negative = false;
		len = 2;
	}

	Joy_Int inum = 0;

	while (!is_delim(src + len)) {
		if (is_nibble_literal(src[len])) {
			if (is_hex_overflow(inum)) {
				return tag_error(src,
				                 find_token_end(src, len),
				                 JOY_PARSE_ERROR_NUM_OVERFLOW);
			}
			inum = (inum << 4) | get_nibble_literal(src[len]);
		} else {
			return tag_error(src, len + 1, JOY_PARSE_ERROR_BAD_NUM_CHAR);
		}
		len++;
	}

	if (negative) {
		inum *= -1;
	}

	return tag_int(src, len, inum);
}

static bool is_num_literal(const char *src)
{
	if (src[0] == '-') {
		src++;
	}
	if (src[0] == '.') {
		src++;
	}
	return isdigit(src[0]);
}

static bool is_sci_exponent(const char *src)
{
	return src[0] == 'e' || src[0] == 'E';
}

static unsigned get_digit(char c)
{
	return c - '0';
}

static bool is_int_overflow(Joy_Int inum)
{
	return inum > JOY_INT_MAX / 10;
}

static bool is_float_overflow(Joy_Float fnum)
{
	return fnum > JOY_FLOAT_MAX / 10.0;
}

Joy_ParseToken parse_num_literal(const char *src)
{
	bool negative;
	size_t len;

	if (src[0] == '-') {
		negative = true;
		len = 1;
	} else {
		negative = false;
		len = 0;
	}

	Joy_Int inum = 0;
	Joy_Float fnum = 0.0;
	uintmax_t place = 0;
	bool dot = false;
	bool sci = false;

	Joy_Float expo = 0;
	bool expo_negative = false;

	while (!is_delim(src + len)) {
		if (is_sci_exponent(src + len)) {
			if (sci) {
				return tag_error(src, len + 1, JOY_PARSE_ERROR_BAD_NUM_CHAR);
			} else if (src[len + 1] == '-') {
				expo_negative = true;
				len++;
			}
			dot = true;
			sci = true;
		} else if (src[len] == '.') {
			if (dot) {
				return tag_error(src, len + 1, JOY_PARSE_ERROR_BAD_NUM_CHAR);
			}
			dot = true;
		} else if (!isdigit(src[len])) {
			return tag_error(src, len + 1, JOY_PARSE_ERROR_BAD_NUM_CHAR);
		} else if (sci) {
			if (is_float_overflow(expo)) {
				return tag_error(src,
				                 find_token_end(src, len),
				                 JOY_PARSE_ERROR_NUM_OVERFLOW);
			}
			expo = (10.0 * expo) + (Joy_Float)get_digit(src[len]);
		} else if (dot) {
			if (place == UINTMAX_MAX || is_float_overflow(fnum)) {
				return tag_error(src,
				                 find_token_end(src, len),
				                 JOY_PARSE_ERROR_NUM_OVERFLOW);
			}
			fnum = (10.0 * fnum) + (Joy_Float)get_digit(src[len]);
			place++;
		} else if (is_int_overflow(inum)) {
			return tag_error(
				src, find_token_end(src, len), JOY_PARSE_ERROR_NUM_OVERFLOW);
		} else {
			inum = (10 * inum) + (Joy_Int)get_digit(src[len]);
		}
		len++;
	}

	if (!dot) {
		if (negative) {
			inum *= -1;
		}
		return tag_int(src, len, inum);
	}

	while (place) {
		fnum /= 10.0;
		place--;
	}
	fnum += (Joy_Float)inum;

	if (sci) {
		if (expo_negative) {
			expo *= -1;
		}
		fnum = pow(fnum, expo);
	}

	if (negative) {
		fnum *= -1.0;
	}

	return tag_float(src, len, fnum);
}

static bool is_set_open(const char *src)
{
	return src[0] == '{';
}

static bool is_set_close(const char *src)
{
	return src[0] == '}';
}

Joy_ParseToken parse_set_member(const char *src)
{
	unsigned member = 0;
	size_t len = 0;

	while (!is_delim(src + len)) {
		if (!isdigit(src[len])) {
			return tag_error(src, len + 1, JOY_PARSE_ERROR_BAD_SET_CHAR);
		}

		member = (10 * member) + get_digit(src[len]);
		if (member > JOY_SET_MAX) {
			return tag_error(src,
			                 find_token_end(src, len),
			                 JOY_PARSE_ERROR_SET_MEMBER_OUT_OF_RANGE);
		}
		len++;
	}
	return tag_set_member(src, len, member);
}

Joy_ParseToken parse_set_literal(const char *src)
{
	Joy_ParseToken empty = parse_empty(src + 1);
	if (is_token_type_error(empty.type)) {
		return empty;
	}

	size_t len = 1 + empty.len;
	Joy_Set set = JOY_SET_EMPTY;

	while (!is_set_close(src + len)) {
		if (!src[len]) {
			return tag_error(src, len, JOY_PARSE_ERROR_UNCLOSED_SET);
		}

		Joy_ParseToken token = parse_set_member(src + len);
		if (is_token_type_error(token.type)) {
			return token;
		}

		set = set_add(set, token.member);
		len += token.len;

		empty = parse_empty(src + len);
		if (is_token_type_error(empty.type)) {
			return empty;
		}
		len += empty.len;
	}
	len++;

	return tag_set(src, len, set);
}

static bool is_char_literal(const char *src)
{
	return src[0] == '\'';
}

static bool is_octal_esc(const char *src)
{
	return src[0] == '\\' && src[1] == '0' && is_tribit_literal(src[2]);
}

static bool is_ascii_overflow(Joy_Char cnum)
{
	return cnum > UCHAR_MAX;
}

Joy_ParseToken parse_octal_esc(const char *src)
{
	Joy_Char cnum = 0;
	for (size_t len = 2; len < 5; len++) {
		if (!is_tribit_literal(src[len])) {
			return tag_error(src, len + 1, JOY_PARSE_ERROR_BAD_OCTAL_ESC);
		}
		cnum = (cnum << 3) | get_tribit_literal(src[len]);
	}

	if (is_ascii_overflow(cnum)) {
		return tag_error(src, 5, JOY_PARSE_ERROR_OCTAL_ESC_OVERFLOW);
	}

	return tag_char(src, 5, cnum);
}

static bool is_hex_esc(const char *src)
{
	return src[0] == '\\' && src[1] == '0' && (src[2] == 'x' || src[2] == 'X');
}

Joy_ParseToken parse_hex_esc(const char *src)
{
	Joy_Char cnum = 0;
	for (size_t len = 3; len < 5; len++) {
		if (!is_nibble_literal(src[len])) {
			return tag_error(src, len + 1, JOY_PARSE_ERROR_BAD_HEX_ESC);
		}
		cnum = (cnum << 4) | get_nibble_literal(src[len]);
	}
	return tag_char(src, 5, cnum);
}

static bool is_decimal_esc(const char *src)
{
	return src[0] == '\\' && isdigit(src[1]);
}

Joy_ParseToken parse_decimal_esc(const char *src)
{
	Joy_Char cnum = 0;
	size_t len;

	for (len = 1; len < 4 && isdigit(src[len]); len++) {
		cnum = (10 * cnum) + (Joy_Char)get_digit(src[len]);
	}
	return tag_char(src, len, cnum);
}

static bool is_small_utf_esc(const char *src)
{
	return src[0] == '\\' && src[1] == 'u';
}

Joy_ParseToken parse_small_utf_esc(const char *src)
{
	Joy_Char cnum = 0;
	for (size_t len = 2; len < 6; len++) {
		if (!is_nibble_literal(src[len])) {
			return tag_error(src, len + 1, JOY_PARSE_ERROR_BAD_SMALL_UTF_ESC);
		}
		cnum = (cnum << 4) | get_nibble_literal(src[len]);
	}
	return tag_char(src, 6, cnum);
}

static bool is_large_utf_esc(const char *src)
{
	return src[0] == '\\' && src[1] == 'U';
}

Joy_ParseToken parse_large_utf_esc(const char *src)
{
	Joy_Char cnum = 0;
	for (size_t len = 2; len < 10; len++) {
		if (!is_nibble_literal(src[len])) {
			return tag_error(src, len + 1, JOY_PARSE_ERROR_BAD_LARGE_UTF_ESC);
		}
		cnum = (cnum << 4) | get_nibble_literal(src[len]);
	}

	if (cnum > JOY_CHAR_MAX) {
		return tag_error(src, 10, JOY_PARSE_ERROR_UTF_ESC_OUT_OF_RANGE);
	}

	return tag_char(src, 10, cnum);
}

static bool is_char_esc(const char *src)
{
	return src[0] == '\\';
}

Joy_ParseToken parse_char_esc(const char *src)
{
	Joy_Char cnum;

	switch (src[1]) {
	case 0:
		return tag_error(src, 1, JOY_PARSE_ERROR_UNFINISHED_ESC);
	case 'a':
		cnum = '\a';
		break;
	case 'b':
		cnum = '\b';
		break;
	case 'e':
		cnum = '\e';
		break;
	case 'f':
		cnum = '\f';
		break;
	case 'n':
		cnum = '\n';
		break;
	case 'r':
		cnum = '\r';
		break;
	case 't':
		cnum = '\t';
		break;
	case 'v':
		cnum = '\v';
		break;
	default:
		cnum = src[1];
		break;
	}

	return tag_char(src, 2, cnum);
}

static const uint8_t UTF8_XBYTE = 128;
static const uint8_t UTF8_2BYTE = 192;
static const uint8_t UTF8_3BYTE = 224;
static const uint8_t UTF8_4BYTE = 240;

static const Joy_Char UTF8_XMASK = 192;
static const Joy_Char UTF8_2MASK = 224;
static const Joy_Char UTF8_3MASK = 240;
static const Joy_Char UTF8_4MASK = 248;

static const unsigned UTF8_SHIFT = 6;

Joy_ParseToken parse_utf8(const char *src)
{
	if ((src[0] & UTF8_4MASK) == UTF8_4BYTE) {
		if (!src[1]) {
			return tag_error(src, 1, JOY_PARSE_ERROR_BAD_UTF8);
		} else if ((src[1] & UTF8_XMASK) != UTF8_XBYTE || !src[2]) {
			return tag_error(src, 2, JOY_PARSE_ERROR_BAD_UTF8);
		} else if ((src[2] & UTF8_XMASK) != UTF8_XBYTE || !src[3]) {
			return tag_error(src, 3, JOY_PARSE_ERROR_BAD_UTF8);
		} else if ((src[3] & UTF8_XMASK) != UTF8_XBYTE) {
			return tag_error(src, 4, JOY_PARSE_ERROR_BAD_UTF8);
		}

		Joy_Char cnum = (((Joy_Char)src[0] & ~UTF8_4MASK) << (3 * UTF8_SHIFT))
		              | (((Joy_Char)src[1] & ~UTF8_XMASK) << (2 * UTF8_SHIFT))
		              | (((Joy_Char)src[2] & ~UTF8_XMASK) << UTF8_SHIFT)
		              | ((Joy_Char)src[3] & ~UTF8_XMASK);
		if (cnum > JOY_CHAR_MAX) {
			return tag_error(src, 4, JOY_PARSE_ERROR_UTF8_OUT_OF_RANGE);
		}
		return tag_char(src, 4, cnum);
	} else if ((src[0] & UTF8_3MASK) == UTF8_3BYTE) {
		if (!src[1]) {
			return tag_error(src, 1, JOY_PARSE_ERROR_BAD_UTF8);
		} else if ((src[1] & UTF8_XMASK) != UTF8_XBYTE || !src[2]) {
			return tag_error(src, 2, JOY_PARSE_ERROR_BAD_UTF8);
		} else if ((src[2] & UTF8_XMASK) != UTF8_XBYTE) {
			return tag_error(src, 3, JOY_PARSE_ERROR_BAD_UTF8);
		}
		return tag_char(src,
		                3,
		                (((Joy_Char)src[0] & ~UTF8_3MASK) << (2 * UTF8_SHIFT))
		                    | (((Joy_Char)src[1] & ~UTF8_XMASK) << UTF8_SHIFT)
		                    | ((Joy_Char)src[2] & ~UTF8_XMASK));
	} else if ((src[0] & UTF8_2MASK) == UTF8_2BYTE) {
		if (!src[1]) {
			return tag_error(src, 1, JOY_PARSE_ERROR_BAD_UTF8);
		} else if ((src[1] & UTF8_XMASK) != UTF8_XBYTE) {
			return tag_error(src, 2, JOY_PARSE_ERROR_BAD_UTF8);
		}
		return tag_char(src,
		                2,
		                (((Joy_Char)src[0] & ~UTF8_2MASK) << UTF8_SHIFT)
		                    || ((Joy_Char)src[1] & ~UTF8_XMASK));
	}
	return tag_char(src, 1, (Joy_Char)src[0]);
}

Joy_ParseToken parse_char(const char *src)
{
	if (is_octal_esc(src)) {
		return parse_octal_esc(src);
	} else if (is_hex_esc(src)) {
		return parse_hex_esc(src);
	} else if (is_decimal_esc(src)) {
		return parse_decimal_esc(src);
	} else if (is_small_utf_esc(src)) {
		return parse_small_utf_esc(src);
	} else if (is_large_utf_esc(src)) {
		return parse_large_utf_esc(src);
	} else if (is_char_esc(src)) {
		return parse_char_esc(src);
	}
	return parse_utf8(src);
}

Joy_ParseToken parse_char_literal(const char *src)
{
	if (!src[1]) {
		return tag_error(src, 1, JOY_PARSE_ERROR_UNCLOSED_CHAR_LITERAL);
	}

	Joy_ParseToken ch = parse_char(src + 1);
	if (is_token_type_error(ch.type)) {
		return ch;
	}
	size_t len = ch.len + 1;

	if (!src[len]) {
		return tag_error(src, len, JOY_PARSE_ERROR_UNCLOSED_CHAR_LITERAL);
	} else if (src[len] != '\'') {
		return tag_error(src, len, JOY_PARSE_ERROR_BAD_CHAR_IN_CHAR_LITERAL);
	}
	return tag_char(src, len + 1, ch.cnum);
}

static bool is_str_literal(const char *src)
{
	return src[0] == '"';
}

static Joy_Buffer *push_char(Joy_Buffer *buffer, Joy_Char cnum)
{
	char ch[5] = {0};
	char_write_utf8(ch, cnum);

	return buffer_push(buffer, ch, strlen(ch));
}

Joy_ParseToken parse_str_literal(const char *src)
{
	Joy_Buffer *buffer = new_buffer(JOY_BUFFER_INIT_CAP);
	size_t len = 1;

	while (src[len] != '"') {
		if (!src[len]) {
			return tag_error(src, len, JOY_PARSE_ERROR_UNCLOSED_STR_LITERAL);
		}

		Joy_ParseToken ch = parse_char(src + len);
		if (is_token_type_error(ch.type)) {
			return ch;
		}

		buffer = push_char(buffer, ch.cnum);
		len += ch.len;
	}
	len++;

	Joy_Str *str;
	if (buffer_get_len(buffer) == 0) {
		str = NULL;
	} else {
		str = new_str_n(buffer_get_raw(buffer), buffer_get_len(buffer));
	}
	buffer_destroy(buffer);

	return tag_str(src, len, str);
}

Joy_ParseToken parse_word(const char *src)
{
	size_t len = 1;
	while (!is_delim(src + len)) {
		len++;
	}
	return tag_word(src, len, new_word_n(src, len));
}

static bool is_list_open(const char *src)
{
	return src[0] == '[';
}

static bool is_list_close(const char *src)
{
	return src[0] == ']';
}

Joy_ParseToken parse_list(const char *src)
{
	Joy_ParseToken empty = parse_empty(src + 1);
	if (is_token_type_error(empty.type)) {
		return empty;
	}
	size_t len = empty.len + 1;

	Joy_ParseList *list = NULL;
	Joy_ParseList *node = NULL;

	while (!is_list_close(src + len)) {
		if (!src[len]) {
			return tag_error(src, len, JOY_PARSE_ERROR_UNCLOSED_LIST);
		}

		Joy_ParseToken token = parse_token(src + len);
		if (is_token_type_error(token.type)) {
			return token;
		} else if (list) {
			node->cdr = new_parse_list_node(token, NULL);
			node = node->cdr;
		} else {
			list = new_parse_list_node(token, NULL);
			node = list;
		}
		len += token.len;

		empty = parse_empty(src + len);
		if (is_token_type_error(token.type)) {
			return token;
		}
		len += empty.len;
	}
	len++;

	return tag_list(src, len, list);
}

Joy_ParseToken parse_token(const char *src)
{
	if (is_comment_close(src)) {
		return tag_error(src, 2, JOY_PARSE_ERROR_UNOPENED_COMMENT);
	} else if (is_set_close(src)) {
		return tag_error(src, 1, JOY_PARSE_ERROR_UNOPENED_SET);
	} else if (is_list_close(src)) {
		return tag_error(src, 1, JOY_PARSE_ERROR_UNOPENED_LIST);
	} else if (is_octal_literal(src)) {
		return parse_octal_literal(src);
	} else if (is_hex_literal(src)) {
		return parse_hex_literal(src);
	} else if (is_num_literal(src)) {
		return parse_num_literal(src);
	} else if (is_set_open(src)) {
		return parse_set_literal(src);
	} else if (is_char_literal(src)) {
		return parse_char_literal(src);
	} else if (is_str_literal(src)) {
		return parse_str_literal(src);
	} else if (is_list_open(src)) {
		return parse_list(src);
	}
	return parse_word(src);
}

Joy_ParseToken parse(const char *src)
{
	Joy_ParseToken empty = parse_empty(src);
	if (is_token_type_error(empty.type)) {
		return empty;
	}
	size_t len = empty.len;

	Joy_ParseList *list = NULL;
	Joy_ParseList *node = NULL;

	while (src[len]) {
		Joy_ParseToken token = parse_token(src + len);
		if (is_token_type_error(token.type)) {
			return token;
		} else if (list) {
			node->cdr = new_parse_list_node(token, NULL);
			node = node->cdr;
		} else {
			list = new_parse_list_node(token, NULL);
			node = list;
		}
		len += token.len;

		empty = parse_empty(src + len);
		if (is_token_type_error(token.type)) {
			return token;
		}
		len += empty.len;
	}
	len++;

	if (!list) {
		return tag_empty(src, len);
	} else if (!list->cdr) {
		return parse_list_pop(&list);
	}
	return tag_list(src, len, list);
}

static Joy_Data strip_token(Joy_ParseToken token)
{
	switch (token.type) {
	case JOY_PARSE_INT:
		return data_int(token.inum);
	case JOY_PARSE_FLOAT:
		return data_float(token.fnum);
	case JOY_PARSE_SET:
		return data_set(token.set);
	case JOY_PARSE_CHAR:
		return data_char(token.cnum);
	case JOY_PARSE_STR:
		return data_str(token.str);
	case JOY_PARSE_WORD:
		return data_word(token.word);
	case JOY_PARSE_LIST:
		return data_list(parse_strip(token));
	default:
		return JOY_DATA_NIL;
	}
}

Joy_List *parse_strip(Joy_ParseToken token)
{
	if (is_token_type_error(token.type)) {
		return NULL;
	} else if (token.type != JOY_PARSE_LIST) {
		return list_push(NULL, strip_token(token));
	}

	Joy_ParseList *src = token.list;
	Joy_List *dest = NULL;
	Joy_List *node = NULL;

	while (src) {
		Joy_ParseToken token = parse_list_pop(&src);
		if (dest) {
			node->cdr = list_push(NULL, strip_token(token));
			node = node->cdr;
		} else {
			dest = list_push(NULL, strip_token(token));
			node = dest;
		}
	}

	return dest;
}
