#include "set.h"

void set_print(Joy_Set set)
{
	if (!set) {
		printf("{}");
		return;
	}

	unsigned member = 0;
	while (!(set & 1)) {
		set >>= 1;
		member++;
	}

	printf("{%u", member);
	set >>= 1;
	member++;

	while (set) {
		if (set & 1) {
			printf(" %u", member);
		}
		set >>= 1;
		member++;
	}

	printf("}");
}

Joy_Set set_add(Joy_Set set, unsigned member)
{
	return set | ((Joy_Set)1 << member);
}
