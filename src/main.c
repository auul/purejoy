#include "repl.h"
#include "state.h"

int main(int argc, char **args)
{
	Joy_State J;
	state_init(&J);
	repl(&J);

	return 0;
}
