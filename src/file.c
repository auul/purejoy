#include "file.h"
#include "alloc.h"

void file_print(const Joy_File *file)
{
	if (file == stdin) {
		printf("stdin");
	} else if (file == stdout) {
		printf("stdout");
	} else if (file == stderr) {
		printf("stderr");
	} else {
		printf("file:%i", fileno((FILE *)file));
	}
}
