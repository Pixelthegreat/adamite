/* error handling */
#include "error.h" /* header */
#include "memory.h" /* memory management */

#include <string.h> /* string utils */
#include <stdio.h> /* sprintf */
#include <stdlib.h> /* malloc */

#ifdef __cplusplus /* c++ check */
extern "C" {
#endif

error *ERROR_NewError(int e_errno, char *err_name, char *msg, int lineno, int colno) {
	/* allocate error */
	error *e = MEMORY_Malloc(error);
	/* bad allocation */
	if (!e || e == NULL)
		return NULL;

	/* assign values */
	e->e_errno = e_errno; /* number of error */
	e->err_name = err_name; /* name of error */
	e->msg = msg; /* message of error */
	e->lineno = lineno; /* line number */
	e->colno = colno; /* column number */

	/* return error */
	return e;
}

char *ERROR_AsString(error *e) {
	/* use sprintf to get a new error string */
	char *s = (char*)malloc(1024); /* new buffer */
	if (!s || s == NULL) /* bad allocation */
		return NULL;

	sprintf(s, "%s (%d, %d): %s\n", e->err_name, e->lineno, e->colno, e->msg);
	/* return string buffer */
	return s;
}

error *ERROR_InvalidSyntax(char *msg, int lineno, int colno) {
	/* allocate and return error */
	return ERROR_NewError(INVALID_SYNTAX, "Invalid Syntax", msg, lineno, colno);
}

error *ERROR_RuntimeError(char *msg, int lineno, int colno) {
	/* allocate and return new error */
	return ERROR_NewError(RUNTIME_ERROR, "Runtime Error", msg, lineno, colno);
}

void ERROR_FreeError(error *e) {
	/* free error */
	MEMORY_Free(e);
}

#ifdef __cplusplus /* c++ check */
}
#endif