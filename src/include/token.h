#include <string.h> /* string manipulation */

#ifndef TOKEN_H
#define TOKEN_H

#ifdef __cplusplus /* check for c++, it doesn't implictly state 'extern' before each function unlike c */
extern "C" {
#endif

#define TOKEN_INT			0 /* Integer 		*/
#define TOKEN_STRING		1 /* String 		*/
#define TOKEN_IDENT			2 /* Identifier 	*/
#define TOKEN_EQ			3 /* '=' 			*/
#define TOKEN_KWD			4 /* Keyword 		*/
#define TOKEN_PLUS			5 /* '+' 			*/
#define TOKEN_MINUS			6 /* '-' 			*/
#define TOKEN_MUL			7 /* '*' 			*/
#define TOKEN_DIV			8 /* '/' 			*/
#define TOKEN_MOD			9 /* '%' 			*/
#define TOKEN_POW			10/* '^' 			*/
#define TOKEN_FLOAT			11/* Float			*/
#define TOKEN_EOL			12/* End Line		*/
#define TOKEN_LPAREN		13/* '('			*/
#define TOKEN_RPAREN		14/* ')'			*/
#define TOKEN_LSQUARE		15/* '{'			*/
#define TOKEN_RSQUARE		16/* '}'			*/
#define TOKEN_LBRACKET		17/* '['			*/
#define TOKEN_RBRACKET		18/* ']'			*/
#define TOKEN_COLON			19/* ':'			*/
#define TOKEN_COMMA			20/* ','			*/
#define TOKEN_VAR_WORD		21/* 'int','char'...*/
#define TOKEN_ARROW			22/* '->'			*/
#define TOKEN_DOLLAR		23/* '$'			*/
#define TOKEN_EE			24/* '=='			*/
#define TOKEN_NE			25/* '!='			*/
#define TOKEN_LT			26/* '<'			*/
#define TOKEN_GT			27/* '>'			*/
#define TOKEN_EOF			100 /* end of the file */

/* token struct for storing token information */
typedef struct _ADAMITE_Lib_Token {
	int type; /* token type, can be any of the types specified above */
	const char *value; /* usually reliant on the type */
	int lineno; /* line number for error tracking */
	int colno; /* column number for error tracking */
} token;

#define TOKEN_Matches(tok, ty, val) (tok->type == ty && (!strcmp(tok->value, val))) /* see if token's type and value match */

token *TOKEN_NewToken(int type, const char *value, int lineno, int colno); /* allocate a new token in the heap */
void TOKEN_FreeToken(token *t); /* free a token from memory */

#ifdef __cplusplus /* same check to close extern statement */
}
#endif

#endif /* TOKEN_H */