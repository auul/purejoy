#include "bool.h"

#include <stdio.h>

void bool_print(Joy_Bool bnum)
{
	switch (bnum) {
	case false:
		printf("false");
		break;
	case true:
		printf("true");
		break;
	}
}
