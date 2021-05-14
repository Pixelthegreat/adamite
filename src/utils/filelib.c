/* Wrapper for c's FILE object system */
#include "filelib.h" /* our header */
#include "memory.h" /* memory management */
#include <stdio.h> /* actual file stuff */
#include <stdlib.h> /* extras */
#include <assert.h> /* assertion stuff */
#include <string.h> /* strlen, strcpy */

#ifdef __cplusplus /* c++ check */
extern "C" {
#endif

file *open(const char *fname, const char *mode) {
	/* open the file */
	FILE *fp = fopen(fname, mode);

	/* failed allocation or didn't find file */
	if (!fp || fp == NULL) {
		/* return null */
		return NULL;
	}

	file *f = MEMORY_Malloc(file);
	if (!f || f == NULL) { /* failed allocation */
		fclose(fp); /* always close */
		free(fp); /* free from memory */
		return NULL; /* return a null pointer */
	}

	/* set our file's name and file pointer */
	f->fp = fp;
	f->fname = fname;
	f->text = "";

	/* return file */
	return f;
}

void close(file *f) {
	/* close the file */
	fclose(f->fp);
}

int readline(file *f, char *buf, int buf_sz) {
	/* line by line from file */
	if (f->text == "")
		f->text = read(f);

	int n_line_no = 0; /* line number */
	int j; /* index of where the newline is at */

	/* now read next line */
	for (j = 0; j < strlen(f->text); j++) {
		/* get to the next line */
		if (f->text[j] == '\n')
			n_line_no++; /* increase line number */
		if (f->line == n_line_no) /* get to the correct line */
			break;
	}

	/* buffer for the line */
	char *new_buf = (char *)malloc(buf_sz);

	/* read the text into the buffer */
	int i;
	for (i = 0; i < buf_sz; i++) {
		if (f->text[i] == '\n')
			break;

		new_buf[i] = f->text[i+j]; /* will get correct pos */
	}

	f->line++; /* increment the line number */

	/* add null term */
	if (i >= buf_sz)
		new_buf[buf_sz-1] = '\0';
	else
		new_buf[i] = '\0';

	/* copy the buffer */
	strcpy(buf, new_buf);

	/* return the result */
	if (i+j >= strlen(f->text))
		return 0;

	return 1; /* continue reading */
}

char *read(file *f) {
	/* create a buffer */
	char *buf = (char *)malloc(100);
	char c; /* character to append to buffer */

	int buf_sz = 100; /* buffer size */
	int buf_pos = 0; /* buffer position */

	/* failed allocation */
	if (!buf || buf == NULL)
		return NULL;

	c = fgetc(f->fp); /* get the first char */

	while (c != EOF) {
		if (buf_pos >= buf_sz) { /* make buffer bigger */
			buf = (char*)realloc(buf, buf_sz*2);
			buf_sz *= 2;
		}
		buf[buf_pos++] = c; /* add char */

		/* get next char */
		c = getc(f->fp);
	}

	/* realloc if not big enough for null term */
	if (buf_pos >= buf_sz) {
		buf = (char*)realloc(buf, buf_sz*2);
		buf_sz *= 2;
	}
	buf[buf_pos] = '\0'; /* add null term char */

	/* return the text */
	return buf;
}

#ifdef __cplusplus /* c++ check */
}
#endif