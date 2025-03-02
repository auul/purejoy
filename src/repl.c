#include "repl.h"
#include "parse.h"
#include "state.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include <readline/history.h>
#include <readline/readline.h>

static bool is_line_empty(const char *line)
{
	for (size_t i = 0; line[i]; i++) {
		if (!isspace(line[i])) {
			return false;
		}
	}
	return true;
}

void repl(Joy_State *J)
{
	J->running = true;

	while (J->running) {
		char *line = readline("joy:: ");
		if (!line) {
			perror("Fatal Error");
			exit(1);
		} else if (!is_line_empty(line)) {
			add_history(line);

			Joy_ParseToken ctrl = parse(line);
			if (parse_token_is_error(ctrl)) {
				parse_token_print(ctrl);
				printf("\n");
			} else {
				state_eval(J, parse_strip(ctrl));
				state_debug(J);
			}
		}
		free(line);
	}
}
