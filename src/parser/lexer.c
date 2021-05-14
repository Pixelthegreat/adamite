/* lexer for adamite */
#include "lexer.h" /* header file */
#include "token.h" /* for the token */
#include "memory.h" /* memory management */

#include <stdio.h> /* for debugging and errors */
#include <stdlib.h> /* precise control for memory management */
#include <string.h> /* string functionality */

#ifdef __cplusplus /* c++ check */
extern "C" {
#endif

lexer *LEXER_NewLexer(const char *text) {
	/* allocate new lexer */
	lexer *l = MEMORY_Malloc(lexer);

	if (!l || l == NULL) /* failed allocation */
		return NULL;

	l->tokens = (token**)malloc(sizeof(token)*10);
	l->index = -1;
	l->lineno = 1;
	l->colno = -1;
	l->c_char = 0;
	l->len_of_text = strlen(text);
	l->text = text;
	l->tokens_cap = 10;
	l->n_of_tokens = 0;
	l->err = 0;
	LEXER_Advance(l);

	return l;
}

void LEXER_Advance(lexer *l) {
	/* advance lexer */
	l->index++;
	l->colno++;

	/* get the current character */
	if (l->len_of_text > l->index)
		l->c_char = l->text[l->index];
	else
		l->c_char = 0;

	/* advance the line number if needed */
	if (l->c_char == '\n') {
		l->lineno++; /* advance line */
		l->colno = 0; /* reset column */
	}
}

void LEXER_AddToken(lexer *l, token *t) {
	/* resize the token list if necessary */
	if (l->n_of_tokens >= l->tokens_cap) {
		l->tokens = (token**)MEMORY_Realloc(l->tokens, sizeof(token) * l->tokens_cap * 2);
		l->tokens_cap *= 2;
	}
	/* add the token to the list */
	l->tokens[l->n_of_tokens++] = t;
}

token *LEXER_MakeString(lexer *l) {
	/* make a string token */
	char *s = (char*)malloc(100); /* allocate a heap buffer */
	if (!s || s == NULL) /* failed allocation */
		return NULL;

	/* grab the line number and column number */
	int lineno = l->lineno;
	int colno = l->colno;

	char quote = l->c_char; /* char that started string */

	LEXER_Advance(l); /* advance */

	int i = 0; /* index of chars */

	/* loop through while char is still not '"' or "'" and i is not greater than 100 */
	while (l->c_char != quote && l->c_char != 0 && i < 100) {
		s[i++] = l->c_char; /* store extra char */
		LEXER_Advance(l);
	}

	if (i >= 100)
		s[99] = '\0';
	else
		s[i] = '\0';

	/* broke out, so if char is not 0, advance */
	if (l->c_char != 0)
		LEXER_Advance(l);

	/* return a new token */
	return TOKEN_NewToken(TOKEN_STRING, s, lineno, colno);
}

token *LEXER_MakeIdent(lexer *l) {
	/* make an identifier token */
	char *s = (char *)malloc(100); /* allocate a heap buffer */
	if (!s || s == NULL) /* failed allocation */
		return NULL;

	/* grab line and column numbers */
	int lineno = l->lineno;
	int colno = l->colno;

	int i = 0; /* counter to stop buffer overflow */
	/* loop through */
	while ((is_ident(l->c_char) || is_int(l->c_char)) && l->c_char != 0 && i < 100) {
		s[i++] = l->c_char;
		LEXER_Advance(l);
	}
	/* add null term char */
	if (i >= 100) /* we've gotten to the end of the buffer limit */
		s[99] = '\0';
	else /* we haven't yet */
		s[i] = '\0';

	/* default token type */
	int tok_type = TOKEN_IDENT;

	/* if keyword */
	if (is_keyword(s))
		tok_type = TOKEN_KWD;
	/* variable declaration word */
	else if (is_var_decl(s)) {
		tok_type = TOKEN_VAR_WORD;
	}

	/* return a new token */
	return TOKEN_NewToken(tok_type, s, lineno, colno);
}

token *LEXER_MakeInt(lexer *l) {
	/* make an integer or float token */
	char *s = (char *)malloc(100); /* allocate a heap buffer */
	if (!s || s == NULL) /* failed allocation */
		return NULL;

	int dot_count = 0; /* number of dots */
	/* grab line number and column number */
	int lineno = l->lineno;
	int colno = l->colno;

	int i = 0; /* counter to stop buffer overflow */

	/* loop through */
	while (dot_count < 2 && is_int_or_float(l->c_char) && l->c_char != 0 && i < 100) {
		if (l->c_char == '.') /* check for '.' to create float */
			dot_count++;
		if (dot_count >= 2) /* we've reached a different token point */
			continue; /* continue to let while loop finish */
		s[i++] = l->c_char; /* add char and advance \/ */
		LEXER_Advance(l);
	}

	/* add null term char */
	if (i >= 100)
		s[99] = '\0';
	else
		s[i] = '\0';

	/* get the type of the token */
	int tok_type = TOKEN_INT;

	if (dot_count > 0) /* float */
		tok_type = TOKEN_FLOAT;

	/* return token */
	return TOKEN_NewToken(tok_type, s, lineno, colno);
}

void LEXER_MakeTokens(lexer *l) {
	/* make the lexer's tokens */
	/* loop */
	while (l->c_char != 0) {
		if (is_start_of_string(l->c_char)) /* for strings */
			LEXER_AddToken(l, LEXER_MakeString(l));
		else if (is_ident(l->c_char)) /* for identifiers */
			LEXER_AddToken(l, LEXER_MakeIdent(l));
		else if (is_int_or_float(l->c_char)) /* for ints and floats */
			LEXER_AddToken(l, LEXER_MakeInt(l));
		else if (l->c_char == '+') { /* '+' operator */ 
			LEXER_AddToken(l, TOKEN_NewToken(TOKEN_PLUS, "+", l->lineno, l->colno));
			LEXER_Advance(l);
		}
		else if (l->c_char == '-') { /* '-' operator */
			int tok_type = TOKEN_MINUS; /* default type */
			int lineno = l->lineno; /* save these */
			int colno = l->colno;
			LEXER_Advance(l);
			if (l->c_char == '>') { /* arrow */
				tok_type = TOKEN_ARROW;
				LEXER_Advance(l);
			}
			char *s = (char*)malloc(3);
			strcpy(s, "-");
			if (tok_type == TOKEN_ARROW)
				strcat(s, ">"); /* value of token for our sake of sanity */
			/* add the token */
			LEXER_AddToken(l, TOKEN_NewToken(tok_type, s, lineno, colno));
		}
		else if (l->c_char == '*') { /* '*' operator */
			LEXER_AddToken(l, TOKEN_NewToken(TOKEN_MUL, "*", l->lineno, l->colno));
			LEXER_Advance(l);
		}
		else if (l->c_char == '/') { /* '/' operator or comment */
			//LEXER_AddToken(l, TOKEN_NewToken(TOKEN_DIV, "/", l->lineno, l->colno));
			//LEXER_Advance(l);
			LEXER_Advance(l);
			/* check for comment */
			if (l->c_char == '/') {
				while (l->c_char != 0 && l->c_char != '\n')
					LEXER_Advance(l); /* advance to next char */
				if (l->c_char == '\n') /* advance once more to next line */
					LEXER_Advance(l);
				/* linux */
				if (l->c_char == 13)
					LEXER_Advance(l);
			}
			/* multiline comment */
			if (l->c_char == '*') {
				while (l->c_char != 0) {
					LEXER_Advance(l);
					if (l->c_char == '*') { /* might be end of comment */
						LEXER_Advance(l); /* advance */
						if (l->c_char == '/') {
							LEXER_Advance(l); /* advance */
							break; /* break */
						}
					}
				}
				if (l->c_char == '\n') /* advance once more to next line */
					LEXER_Advance(l);
				/* linux */
				if (l->c_char == 13)
					LEXER_Advance(l);
			}
			else /* '/' operator */
				LEXER_AddToken(l, TOKEN_NewToken(TOKEN_DIV, "/", l->lineno, l->colno));
		}
		else if (l->c_char == '%') { /* '%' operator */
			LEXER_AddToken(l, TOKEN_NewToken(TOKEN_MOD, "%", l->lineno, l->colno));
			LEXER_Advance(l);
		}
		else if (l->c_char == '^') { /* '^' operator */
			LEXER_AddToken(l, TOKEN_NewToken(TOKEN_POW, "^", l->lineno, l->colno));
			LEXER_Advance(l);
		}
		else if (l->c_char == '=') { /* '=', '==' operators */
			LEXER_Advance(l);
			/* '==' */
			int tok_type = TOKEN_EQ;
			if (l->c_char == '=') {
				LEXER_Advance(l); /* advance */
				tok_type = TOKEN_EE;
			}
			if (tok_type == TOKEN_EQ) /* = */
				LEXER_AddToken(l, TOKEN_NewToken(tok_type, "=", l->lineno, l->colno));
			else /* == */
				LEXER_AddToken(l, TOKEN_NewToken(tok_type, "==", l->lineno, l->colno));
		}
		else if (l->c_char == '(') { /* '(' */
			LEXER_AddToken(l, TOKEN_NewToken(TOKEN_LPAREN, "(", l->lineno, l->colno));
			LEXER_Advance(l);
		}
		else if (l->c_char == ')') { /* ')' */
			LEXER_AddToken(l, TOKEN_NewToken(TOKEN_RPAREN, ")", l->lineno, l->colno));
			LEXER_Advance(l);
		}
		else if (l->c_char == '{') { /* '{' */
			LEXER_AddToken(l, TOKEN_NewToken(TOKEN_LSQUARE, "{", l->lineno, l->colno));
			LEXER_Advance(l);
		}
		else if (l->c_char == '}') { /* '}' */
			LEXER_AddToken(l, TOKEN_NewToken(TOKEN_RSQUARE, "}", l->lineno, l->colno));
			LEXER_Advance(l);
		}
		else if (l->c_char == '[') { /* '[' */
			LEXER_AddToken(l, TOKEN_NewToken(TOKEN_LBRACKET, "[", l->lineno, l->colno));
			LEXER_Advance(l);
		}
		else if (l->c_char == ']') { /* ']' */
			LEXER_AddToken(l, TOKEN_NewToken(TOKEN_RBRACKET, "]", l->lineno, l->colno));
			LEXER_Advance(l);
		}
		else if (l->c_char == ':') { /* ':' */
			LEXER_AddToken(l, TOKEN_NewToken(TOKEN_COLON, ":", l->lineno, l->colno));
			LEXER_Advance(l);
		}
		else if (l->c_char == ',') { /* ',' */
			LEXER_AddToken(l, TOKEN_NewToken(TOKEN_COMMA, ",", l->lineno, l->colno));
			LEXER_Advance(l);
		}
		else if (l->c_char == '$') { /* '$' */
			LEXER_AddToken(l, TOKEN_NewToken(TOKEN_DOLLAR, "$", l->lineno, l->colno));
			LEXER_Advance(l);
		}
		else if (l->c_char == '<') { /* '<' */
			LEXER_AddToken(l, TOKEN_NewToken(TOKEN_LT, "<", l->lineno, l->colno));
			LEXER_Advance(l);
		}
		else if (l->c_char == '>') { /* '>' */
			LEXER_AddToken(l, TOKEN_NewToken(TOKEN_GT, ">", l->lineno, l->colno));
			LEXER_Advance(l);
		}
		else if (l->c_char == ';') { /* end of line */
			LEXER_Advance(l);
			LEXER_AddToken(l, TOKEN_NewToken(TOKEN_EOL, ";", l->lineno, l->colno));
		}
		else if (l->c_char == '!') { /* '!=' */
			LEXER_Advance(l);
			/* expecting '=' */
			if (l->c_char != '=') {
				printf("Error (%d, %d): Expected '='\n", l->lineno, l->colno);
				l->err = 1;
				break; /* break */
			}
			/* advance */
			LEXER_Advance(l);
			/* add token */
			LEXER_AddToken(l, TOKEN_NewToken(TOKEN_NE, "!=", l->lineno, l->colno));
		}
		else if (is_whitespace(l->c_char)) /* whitespace found */
			LEXER_Advance(l); /* advance */
		else { /* unknown */
			/* print an error */
			printf("Error (%d, %d): Unknown Character '%c'\n", l->lineno, l->colno, l->c_char);
			l->err = 1; /* there was an error */
			break; /* break */
		}
	}
	/* add eof (end of file) */
	LEXER_AddToken(l, TOKEN_NewToken(TOKEN_EOF, "EOF", l->lineno, l->colno));
	/* finished */
}

void LEXER_FreeLexer(lexer *l) {
	/* free lexer and tokens from memory */
	for (int i = 0; i < l->n_of_tokens; i++) { /* loop through and free all tokens */
		TOKEN_FreeToken(l->tokens[i]); /* free the token */
	}
	MEMORY_Free(l);
}

#ifdef __cplusplus /* c++ check */
}
#endif