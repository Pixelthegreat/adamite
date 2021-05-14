/* parser takes tokens and turns them into
a tree of nodes. */
#include "node.h" /* nodes */
#include "token.h" /* tokens */
#include "error.h" /* errors */

#ifndef PARSER_H
#define PARSER_H

#ifdef __cplusplus /* c++ check */
extern "C" {
#endif

typedef struct _ADAMITE_Lib_Parser { /* actual struct for parser */
	token **tokens; /* list of tokens */
	token *current_token; /* current token */
	node *newNode; /* finished node */
	error *e; /* current error */
	int pos; /* position in token list */
	unsigned int n_of_toks; /* number of tokens */
} parser;

parser *PARSER_NewParser(token **tokens, unsigned int n_of_toks); /* create a new parser */
void PARSER_Parse(parser *p); /* parse the tokens given */
void PARSER_Advance(parser *p); /* advance parser */
node *PARSER_Statements(parser *p); /* parse statements */
node *PARSER_Expr(parser *p); /* parse expression */
node *PARSER_Atom(parser *p); /* parse atom */
node *PARSER_Factor(parser *p); /* parse factor */
node *PARSER_Term(parser *p); /* parse term */
node *PARSER_ArithExpr(parser *p); /* arithmatic expression */
node *PARSER_CompExpr(parser *p); /* comparison */
node *PARSER_BinOp(parser *p, node* (*func)(parser*), int types[], int n_of_types); /* binary operation */
void PARSER_FreeParser(parser *p); /* free a parser */

int is_int_in(int x, int l[], int l_len); /* find an int in an array of unknown size */

#ifdef __cplusplus /* c++ check */
}
#endif

#endif /* PARSER_H */