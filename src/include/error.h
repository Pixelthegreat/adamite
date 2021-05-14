/* error handling */
#ifndef ERROR_H
#define ERROR_H

#ifdef __cplusplus /* c++ check */
extern "C" {
#endif

#define INVALID_SYNTAX 0		/* invalid syntax error			*/
#define RUNTIME_ERROR 1			/* runtime error				*/

typedef struct _ADAMITE_Lib_Error { /* struct for errors */
	int e_errno; /* error number */
	char *err_name; /* error name */
	char *msg; /* error message */
	int lineno; /* line number */
	int colno; /* column number */
} error;

error *ERROR_NewError(int e_errno, char *err_name, char *msg, int lineno, int colno); /* create a new error */
char *ERROR_AsString(error *e); /* get the string representation of an error */
error *ERROR_InvalidSyntax(char *msg, int lineno, int colno); /* invalid syntax error */
error *ERROR_RuntimeError(char *msg, int lineno, int colno); /* runtime error */
void ERROR_FreeError(error *e); /* free an error from memory */

#ifdef __cplusplus /* c++ check */
}
#endif

#endif /* ERROR_H */