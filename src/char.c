#include "char.h"

#include <ctype.h>

static const uint8_t UTF8_XBYTE = 128;
static const uint8_t UTF8_2BYTE = 192;
static const uint8_t UTF8_3BYTE = 224;
static const uint8_t UTF8_4BYTE = 240;

static const Joy_Char UTF8_XMASK = 192;
static const Joy_Char UTF8_2MASK = 224;
static const Joy_Char UTF8_3MASK = 240;
static const Joy_Char UTF8_4MASK = 248;

static const Joy_Char UTF8_1MAX = 127;
static const Joy_Char UTF8_2MAX = 2047;
static const Joy_Char UTF8_3MAX = 65535;

static const unsigned UTF8_SHIFT = 6;

void char_print(Joy_Char cnum)
{
	if (cnum == '\'') {
		printf("'\\''");
		return;
	}

	printf("'");
	char_print_esc(cnum);
	printf("'");
}

static bool is_utf8_multibyte(Joy_Char cnum)
{
	return cnum > UTF8_1MAX;
}

void char_print_esc(Joy_Char cnum)
{
	switch (cnum) {
	case '\a':
		printf("\\a");
		break;
	case '\b':
		printf("\\b");
		break;
	case '\e':
		printf("\\e");
		break;
	case '\f':
		printf("\\f");
		break;
	case '\n':
		printf("\\n");
		break;
	case '\r':
		printf("\\r");
		break;
	case '\t':
		printf("\\t");
		break;
	case '\v':
		printf("\\v");
		break;
	case '\\':
		printf("\\\\");
		break;
	case ' ':
		printf(" ");
		break;
	default:
		if (is_utf8_multibyte(cnum)) {
			char_print_utf8(cnum);
		} else if (isgraph(cnum)) {
			printf("%c", cnum);
		} else {
			printf("%#2x", cnum);
		}
		break;
	}
}

void char_print_utf8(Joy_Char cnum)
{
	if (cnum > UTF8_3MAX) {
		printf("%c%c%c%c",
		       (uint8_t)(cnum >> (3 * UTF8_SHIFT)) | UTF8_4BYTE,
		       (uint8_t)((cnum >> (2 * UTF8_SHIFT)) & ~UTF8_XMASK) | UTF8_XBYTE,
		       (uint8_t)((cnum >> UTF8_SHIFT) & ~UTF8_XMASK) | UTF8_XBYTE,
		       (uint8_t)(cnum & ~UTF8_XMASK) | UTF8_XBYTE);
	} else if (cnum > UTF8_2MAX) {
		printf("%c%c%c",
		       (uint8_t)(cnum >> (2 * UTF8_SHIFT)) | UTF8_3BYTE,
		       (uint8_t)((cnum >> UTF8_SHIFT) & ~UTF8_XMASK) | UTF8_XBYTE,
		       (uint8_t)(cnum & ~UTF8_XMASK) | UTF8_XBYTE);
	} else if (cnum > UTF8_1MAX) {
		printf("%c%c",
		       (uint8_t)(cnum >> UTF8_SHIFT) | UTF8_2BYTE,
		       (uint8_t)(cnum & ~UTF8_XMASK) | UTF8_XBYTE);
	} else {
		printf("%c", cnum);
	}
}

char *char_write_utf8(char *dest, Joy_Char cnum)
{
	if (cnum > UTF8_3MAX) {
		dest[0] = (uint8_t)(cnum >> (3 * UTF8_SHIFT)) | UTF8_4BYTE;
		dest[1] =
			(uint8_t)((cnum >> (2 * UTF8_SHIFT)) & ~UTF8_XMASK) | UTF8_XBYTE;
		dest[2] = (uint8_t)((cnum >> UTF8_SHIFT) & ~UTF8_XMASK) | UTF8_XBYTE;
		dest[3] = (uint8_t)(cnum & ~UTF8_XMASK) | UTF8_XBYTE;
		return dest + 4;
	} else if (cnum > UTF8_2MAX) {
		dest[0] = (uint8_t)(cnum >> (2 * UTF8_SHIFT)) | UTF8_3BYTE;
		dest[1] = (uint8_t)((cnum >> UTF8_SHIFT) & ~UTF8_XMASK) | UTF8_XBYTE;
		dest[2] = (uint8_t)(cnum & ~UTF8_XMASK) | UTF8_XBYTE;
		return dest + 3;
	} else if (cnum > UTF8_1MAX) {
		dest[0] = (uint8_t)(cnum >> UTF8_SHIFT) | UTF8_2BYTE;
		dest[1] = (uint8_t)(cnum & ~UTF8_XMASK) | UTF8_XBYTE;
		return dest + 2;
	} else {
		dest[0] = (uint8_t)cnum;
		return dest + 1;
	}
}
