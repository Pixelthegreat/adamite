#include "token.h" /* our header file */
#include "memory.h" /* memory management */
#include "os.h" /* LINUX/WINDOWS */

#include <stdio.h> /* debugging */

#ifdef __cplusplus /* c++ check */
extern "C" {
#endif

token *TOKEN_NewToken(int type, const char *value, int lineno, int colno) {
	token *t = MEMORY_Malloc(token); /* allocate token */

	if (!t || t == NULL) /* allocation failed */
		return NULL;

	t->type = type; /* type of token */
	t->value = value; /* value of token */
	t->lineno = lineno; /* line of token */
	t->colno = colno; /* column of token */

	return t;
}

void TOKEN_FreeToken(token *t) {
	/* free only dynamically allocated stuffs */
	if (t->type == TOKEN_STRING || t->type == TOKEN_INT || t->type == TOKEN_IDENT) {
		/* free string */
		MEMORY_Free(t->value);
	}
	MEMORY_Free(t); /* free the token from memory */
}

#ifdef __cplusplus /* c++ check */
}
#endif