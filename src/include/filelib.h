/* wrapper for c's FILE object system */
#include <stdio.h> /* FILE, fopen, fclose, etc. */

#ifndef FILELIB_H
#define FILELIB_H

#ifdef __cplusplus /* c++ check */
extern "C" {
#endif

typedef struct _ADAMITE_Lib_File {
	FILE *fp; /* pointer to the c FILE object */
	const char *fname; /* filename */
	char *text;
	int line; /* for readline */
} file;

file *open(const char *fname, const char *mode); /* open a file */
void close(file *f); /* close a file */
int readline(file *f, char *buf, int buf_sz); /* read a line into an already existant buffer */
void seekline(int i); /* seek to a certain line */
char *read(file *f); /* read all the contents of a file */
void write(file *f, char *text); /* unimplemented */

#ifdef __cplusplus /* c++ check */
}
#endif

#endif /* FILELIB_H */