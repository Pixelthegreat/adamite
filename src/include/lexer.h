/* the lexer takes code and turns it into tokens. example:
		3 -> INT:3
		puts "Hello, World!"; -> IDENT:puts STRING:Hello, World EOL:;
*/
#include "token.h" /* lexer tokens */
#include <string.h> /* strcmp */

#ifndef LEXER_H
#define LEXER_H

#ifdef __cplusplus /* c++ check */
extern "C" {
#endif

/* is a string equal to another */
#define is(a,b) (!strcmp(a,b))
/* find out if a char begins a string */
#define is_start_of_string(c) (c == '\"' || c == '\'')
/* find out if a char is part of an ident */
#define is_ident(c) ((c >= 'A' && c <= 'Z') ||\
					 (c >= 'a' && c <= 'z') ||\
					 (c == '_'))
/* find out if a char is part of an int or float */
#define is_int_or_float(c) ((c >= '0' && c <= '9') ||\
							(c == '.'))
/* find out if a char is an integer */
#define is_int(c) (c >= '0' && c <= '9')
/* find whitespace (spaces and tabs) */
#define is_whitespace(c) ((c == '\t') ||\
						  (c == ' ') ||\
						  (c == '\n') ||\
						  (c == (char)13))
/* find out if a word is a keyword */
#define is_keyword(w) ((is(w, "return")) ||\
					   (is(w, "puts")) ||\
					   (is(w, "if")) ||\
					   (is(w, "for")) ||\
					   (is(w, "while")) ||\
					   (is(w, "to")) ||\
					   (is(w, "fn")) ||\
					   (is(w, "sizeof")) ||\
					   (is(w, "end")) ||\
					   (is(w, "if")) ||\
					   (is(w, "struct")) ||\
					   (is(w, "stdin")) ||\
					   (is(w, "include")) ||\
					   (is(w, "new")))
/* find out if a word is a var declaration */
#define is_var_decl(w) ((is(w, "int")) ||\
					    (is(w, "float")) ||\
					    (is(w, "char")) ||\
					    (is(w, "str")) ||\
					    (is(w, "inst")))

typedef struct _ADAMITE_Lib_Lexer {
	token **tokens; /* the tokens that are held at the end of processing code */
	const char *text; /* the actual code that is passed in */
	int index; /* the index of our current character */
	char c_char; /* current character that we are lexing */
	int len_of_text; /* length of text */
	int n_of_tokens; /* number of tokens */
	int tokens_cap; /* the capacity of the tokens array */
	int lineno; /* line */
	int colno; /* column */
	int err; /* boolean to tell if an error has been printed or not */
} lexer;

lexer *LEXER_NewLexer(const char *text); /* allocate new lexer */
token *LEXER_MakeString(lexer *l); /* lexer make a string */
token *LEXER_MakeIdent(lexer *l); /* lexer make an ident */
token *LEXER_MakeInt(lexer *l); /* lexer make an integer */
void LEXER_MakeTokens(lexer *l); /* make the tokens from code */
void LEXER_FreeLexer(lexer *l); /* free lexer */
void LEXER_Advance(lexer *l); /* advance lexer to next char */
void LEXER_AddToken(lexer *l, token *t); /* add a token to a lexer's token list */

#ifdef __cplusplus /* c++ check */
}
#endif

#endif /* LEXER_H */