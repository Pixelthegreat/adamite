/* main file runs code from filename specified in arguments */
#include "adamite.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
	/* initialise storage */
	STORAGE_Init();
	/* initialise names */
	NAMES_Init();

	/* filename is first argument passed */
	if (argc < 2) {
		/* print error */
		printf("Filename not specified.\n");

		/* return code 2 */
		return 2;
	}
	/* otherwise, get filename */
	char *fname = argv[1];

	/* get error code */
	int code = run(fname);

	/* free storage */
	STORAGE_FreeAll();
	/* free names */
	NAMES_FreeAll();

	/* print error code */
	printf("Finished with code (%d)\n", code);

	return 0; /* return error code */
}