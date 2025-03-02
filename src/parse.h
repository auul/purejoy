#ifndef JOY_PARSE_H
#define JOY_PARSE_H

#include "global.h"

// Forward Declarations

typedef enum joy_parse_type Joy_ParseType;
typedef struct joy_parse_token Joy_ParseToken;
typedef struct joy_parse_list Joy_ParseList;

// Data Types

enum joy_parse_type {
	JOY_PARSE_EMPTY,
	JOY_PARSE_INT,
	JOY_PARSE_FLOAT,
	JOY_PARSE_SET,
	JOY_PARSE_SET_MEMBER,
	JOY_PARSE_CHAR,
	JOY_PARSE_STR,
	JOY_PARSE_WORD,
	JOY_PARSE_LIST,

	JOY_PARSE_ERROR,
	JOY_PARSE_ERROR_UNCLOSED_COMMENT,
	JOY_PARSE_ERROR_UNOPENED_COMMENT,
	JOY_PARSE_ERROR_BAD_NUM_CHAR,
	JOY_PARSE_ERROR_NUM_OVERFLOW,
	JOY_PARSE_ERROR_UNCLOSED_SET,
	JOY_PARSE_ERROR_UNOPENED_SET,
	JOY_PARSE_ERROR_BAD_SET_CHAR,
	JOY_PARSE_ERROR_SET_MEMBER_OUT_OF_RANGE,
	JOY_PARSE_ERROR_BAD_OCTAL_ESC,
	JOY_PARSE_ERROR_OCTAL_ESC_OVERFLOW,
	JOY_PARSE_ERROR_BAD_HEX_ESC,
	JOY_PARSE_ERROR_BAD_SMALL_UTF_ESC,
	JOY_PARSE_ERROR_BAD_LARGE_UTF_ESC,
	JOY_PARSE_ERROR_UTF_ESC_OUT_OF_RANGE,
	JOY_PARSE_ERROR_UNFINISHED_ESC,
	JOY_PARSE_ERROR_BAD_UTF8,
	JOY_PARSE_ERROR_UTF8_OUT_OF_RANGE,
	JOY_PARSE_ERROR_UNCLOSED_CHAR_LITERAL,
	JOY_PARSE_ERROR_BAD_CHAR_IN_CHAR_LITERAL,
	JOY_PARSE_ERROR_UNCLOSED_STR_LITERAL,
	JOY_PARSE_ERROR_UNCLOSED_LIST,
	JOY_PARSE_ERROR_UNOPENED_LIST,
};

struct joy_parse_token {
	const char *src;
	size_t len;
	Joy_ParseType type;
	union {
		void *ptr;
		Joy_Int inum;
		Joy_Float fnum;
		Joy_Set set;
		unsigned member;
		Joy_Char cnum;
		Joy_Str *str;
		Joy_Word *word;
		Joy_ParseList *list;
	};
};

struct joy_parse_list {
	Joy_ParseToken car;
	Joy_ParseList *cdr;
};

// Functions

Joy_ParseList *new_parse_list_node(Joy_ParseToken car, Joy_ParseList *cdr);
void parse_list_print(const Joy_ParseList *list);
void parse_list_mark(Joy_ParseList *list);
void parse_token_print(const Joy_ParseToken token);
void parse_token_mark(Joy_ParseToken token);
bool parse_token_is_error(const Joy_ParseToken token);
Joy_ParseToken parse_line_comment(const char *src);
Joy_ParseToken parse_nested_comment(const char *src);
Joy_ParseToken parse_empty(const char *src);
Joy_ParseToken parse_octal_literal(const char *src);
Joy_ParseToken parse_hex_literal(const char *src);
Joy_ParseToken parse_num_literal(const char *src);
Joy_ParseToken parse_set_member(const char *src);
Joy_ParseToken parse_set_literal(const char *src);
Joy_ParseToken parse_octal_esc(const char *src);
Joy_ParseToken parse_hex_esc(const char *src);
Joy_ParseToken parse_decimal_esc(const char *src);
Joy_ParseToken parse_small_utf_esc(const char *src);
Joy_ParseToken parse_large_utf_esc(const char *src);
Joy_ParseToken parse_char_esc(const char *src);
Joy_ParseToken parse_utf8(const char *src);
Joy_ParseToken parse_char(const char *src);
Joy_ParseToken parse_char_literal(const char *src);
Joy_ParseToken parse_str_literal(const char *src);
Joy_ParseToken parse_word(const char *src);
Joy_ParseToken parse_token(const char *src);
Joy_ParseToken parse(const char *src);
Joy_List *parse_strip(Joy_ParseToken token);

#endif
