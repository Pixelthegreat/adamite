/* see parser.h for documentation */
#include "parser.h" /* header */
#include "node.h" /* nodes */
#include "token.h" /* tokens */
#include "memory.h" /* memory management */
#include "error.h" /* errors */

#include <stdio.h> /* printf (for debugging) */
#include <stdlib.h> /* atoi */

#ifdef __cplusplus /* c++ check */
extern "C" {
#endif

parser *PARSER_NewParser(token **tokens, unsigned int n_of_toks) {
	/* allocate new parser */
	parser *p = MEMORY_Malloc(parser);
	/* bad allocation */
	if (!p || p == NULL)
		return NULL;
	/* assign values */
	p->tokens = tokens; /* tokens */
	p->current_token = NULL; /* current token that we are processing */
	p->newNode = NULL; /* node that we process */
	p->e = NULL;
	p->pos = -1; /* position in token list */
	p->n_of_toks = n_of_toks; /* number of tokens in list */
	/* advance to first token */
	PARSER_Advance(p);
	return p;
}

void PARSER_Advance(parser *p) {
	/* add one to pos */
	p->pos++;

	/* if pos has reached end */
	if (p->pos >= p->n_of_toks)
		/* set current token to NULL */
		p->current_token = NULL;
	else /* otherwise */
		p->current_token = p->tokens[p->pos]; /* get next token */
}

node *PARSER_Expr(parser *p) {
	/* return arithmatic expression */
	node *n = PARSER_CompExpr(p);
	return n;
}

node *PARSER_CompExpr(parser *p) {
	int types[4] = {TOKEN_EE,TOKEN_NE,TOKEN_GT,TOKEN_LT};
	return PARSER_BinOp(p, PARSER_ArithExpr, types, 4);
}

node *PARSER_Statements(parser *p) {
	/* allocate new node */
	node *n = NODE_NewNode(NODE_STATEMENTS);

	/* failed allocation */
	if (!n || n == NULL)
		return NULL;

	/* empty file */
	if (p->current_token->type == TOKEN_EOF)
		return n;

	/* get the first statement */
	node *next = PARSER_Expr(p);

	/* failed to retrieve node or error */
	if (p->e != NULL || next == NULL) {
		/* free statement node */
		if (next != NULL) NODE_FreeNode(next);
		/* return */
		return NULL;
	}
	/* add node as child */
	NODE_AddChild(n, next);

	/* set line and column numbers */
	n->lineno = next->lineno;
	n->colno = next->colno;

	/* loop until token is not of type EOL */
	while (p->current_token->type == TOKEN_EOL && !TOKEN_Matches(p->current_token, TOKEN_KWD, "end")) {
		/* advance past ';' */
		PARSER_Advance(p);
		/* break at EOF */
		if (p->current_token->type == TOKEN_EOF || TOKEN_Matches(p->current_token, TOKEN_KWD, "end"))
			break;
		/* advance past more EOLs */
		while (p->current_token->type == TOKEN_EOL)
			PARSER_Advance(p);
		/* get next statement */
		next = PARSER_Expr(p);

		/* error or memory failure */
		if (p->e != NULL || next == NULL) {
			/* free statement node */
			NODE_FreeNode(next);
			/* return */
			return NULL;
		}

		/* add the next as child */
		NODE_AddChild(n, next);
	}

	/* return node */
	return n;
}

node *PARSER_ArithExpr(parser *p) {
	/* '+', '-' */
	int types[2] = {TOKEN_PLUS,TOKEN_MINUS};
	return PARSER_BinOp(p, PARSER_Term, types, 2);
}
node *PARSER_Term(parser *p) {
	/* '*', '/', '%' */
	int types[3] = {TOKEN_MUL,TOKEN_DIV,TOKEN_MOD};
	return PARSER_BinOp(p, PARSER_Factor, types, 3);
}

node *PARSER_Factor(parser *p) {
	/* get current token */
	token *tok = p->current_token;

	/* check for '+' and '-' */
	int tps_1[2] = {TOKEN_PLUS,TOKEN_MINUS};
	int tps_2[2] = {TOKEN_INT,TOKEN_FLOAT};

	if (is_int_in(tok->type, tps_1, 2)) {
		/* advance */
		PARSER_Advance(p);
		/* get factor */
		node *factor = PARSER_Factor(p);
		/* memory or regular error */
		if (factor == NULL || p->e != NULL)
			return NULL;

		/* create new unop node */
		node *n = NODE_NewNode(NODE_UNOP);
		/* failed allocation */
		if (n == NULL)
			return NULL;
		/* add child and token */
		NODE_AddChild(n, factor);
		NODE_AddToken(n, tok);
		/* assign line and column numbers */
		n->lineno = tok->lineno;
		n->colno = tok->colno;
		/* return node */
		return n;
	}
	else if (is_int_in(tok->type, tps_2, 2)) {
		/* advance */
		PARSER_Advance(p);
		/* determine node type */
		int node_type = NODE_INT;
		/* float */
		if (tok->type == TOKEN_FLOAT)
			node_type = NODE_FLOAT;
		/* allocate new node */
		node *n = NODE_NewNode(node_type);
		/* failed allocation */
		if (n == NULL)
			return NULL;
		/* add token */
		NODE_AddToken(n, tok);
		/* assign line and column numbers */
		n->lineno = tok->lineno;
		n->colno = tok->colno;
		/* return node */
		return n;
	}
	else if (tok->type == TOKEN_LPAREN) {
		/* advance */
		PARSER_Advance(p);
		/* get expression */
		node *expr = PARSER_Expr(p);
		/* error or failed allocation */
		if (expr == NULL || p->e != NULL)
			return NULL; /* exit */
		/* expects ')' */
		if (p->current_token->type != TOKEN_RPAREN) {
			/* create error */
			p->e = ERROR_InvalidSyntax("Expected ')'", p->current_token->lineno, p->current_token->colno);
			/* free expression */
			NODE_FreeChildren(expr);
			return NULL; /* return */
		}
		/* advance */
		PARSER_Advance(p);
		/* return expression */
		return expr;
	}
	else if (tok->type == TOKEN_STRING) {
		/* advance */
		PARSER_Advance(p);
		/* allocate new node */
		node *n = NODE_NewNode(NODE_STRING);
		/* failed allocation */
		if (n == NULL)
			return NULL;
		/* add token */
		NODE_AddToken(n, tok);
		/* assign line and column numbers */
		n->lineno = tok->lineno;
		n->colno = tok->colno;
		/* return node */
		return n;
	}
	else if (TOKEN_Matches(tok, TOKEN_KWD, "while")) {
		/* advance */
		PARSER_Advance(p);
		/* get expression */
		node *comp_expr = PARSER_Expr(p);
		/* error or failed allocation */
		if (comp_expr == NULL || p->e != NULL)
			return NULL;
		/* get statements */
		node *statements = PARSER_Statements(p);
		/* error or failed allocation */
		if (statements == NULL || p->e != NULL) {
			/* free stuff */
			NODE_FreeChildren(comp_expr);
			return NULL; /* exit */
		}
		/* expects 'end' */
		if (!TOKEN_Matches(p->current_token, TOKEN_KWD, "end")) {
			/* create error */
			p->e = ERROR_InvalidSyntax("Expected 'end'", p->current_token->lineno, p->current_token->colno);
			/* free stuff */
			NODE_FreeChildren(comp_expr);
			NODE_FreeChildren(statements);
			return NULL; /* exit */
		}
		/* advance */
		PARSER_Advance(p);
		/* allocate new node */
		node *n = NODE_NewNode(NODE_WHILE);
		/* failed allocation */
		if (n == NULL) {
			/* free stuff */
			NODE_FreeChildren(comp_expr);
			NODE_FreeChildren(statements);
			return NULL; /* exit */
		}
		/* add children */
		NODE_AddChild(n, comp_expr);
		NODE_AddChild(n, statements);
		/* assign values */
		n->lineno = tok->lineno;
		n->colno = tok->colno;
		/* return node */
		return n;
	}
	else if (TOKEN_Matches(tok, TOKEN_KWD, "new")) {
		/* advanace */
		PARSER_Advance(p);
		/* expecting var word */
		if (p->current_token->type != TOKEN_VAR_WORD) {
			/* create error */
			p->e = ERROR_InvalidSyntax("Expected type name", p->current_token->lineno, p->current_token->colno);
			return NULL; /* exit */
		}
		/* store var word and advance */
		token *v_type = p->current_token;
		PARSER_Advance(p);
		/* stuff */
		node *expr = NULL; /* array size */
		int is_array = 0; /* is array */
		/* '[' for array type */
		if (p->current_token->type == TOKEN_LBRACKET) {
			/* advance */
			PARSER_Advance(p);
			/* get expression for size */
			expr = PARSER_Expr(p);
			/* error or failed allocation */
			if (expr == NULL || p->e != NULL)
				return NULL;
			/* expects ']' */
			if (p->current_token->type != TOKEN_RBRACKET) {
				/* create error */
				p->e = ERROR_InvalidSyntax("Expected ']'", p->current_token->lineno, p->current_token->colno);
				/* free node */
				NODE_FreeChildren(expr);
				return NULL; /* exit */
			}
			PARSER_Advance(p); /* advance */
			/* it is an array */
			is_array = 1;
		}
		/* allocate new node */
		node *n = NODE_NewNode(NODE_NEW);
		/* failed allocation */
		if (n == NULL) {
			/* free */
			if (expr != NULL) NODE_FreeChildren(expr);
			return NULL; /* exit */
		}
		/* add token and values */
		NODE_AddToken(n, v_type);
		if (expr != NULL) NODE_AddChild(n, expr);
		n->b = is_array;
		/* assign line and column numbers */
		n->lineno = tok->lineno;
		n->colno = tok->colno;
		/* return node */
		return n;
	}
	else if (TOKEN_Matches(tok, TOKEN_KWD, "stdin")) {
		/* advance */
		PARSER_Advance(p);
		/* allocate node */
		node *n = NODE_NewNode(NODE_STDIN);
		/* failed allocation */
		if (n == NULL)
			return NULL;
		/* assign line and column numbers */
		n->lineno = tok->lineno;
		n->colno = tok->colno;
		/* return node */
		return n;
	}
	else if (TOKEN_Matches(tok, TOKEN_KWD, "fn")) {
		/* advance */
		PARSER_Advance(p);
		/* expects identifier for name */
		if (p->current_token->type != TOKEN_IDENT) {
			/* create error */
			p->e = ERROR_InvalidSyntax("Expected identifier", p->current_token->lineno, p->current_token->colno);
			return NULL; /* exit */
		}
		/* store func name */
		token *func_name = p->current_token;
		PARSER_Advance(p); /* advance */
		/* expects '(' */
		if (p->current_token->type != TOKEN_LPAREN) {
			/* create error */
			p->e = ERROR_InvalidSyntax("Expected '('", p->current_token->lineno, p->current_token->colno);
			return NULL; /* exit */
		}
		PARSER_Advance(p); /* advance */
		/* allocate new node */
		node *n = NODE_NewNode(NODE_FUNCDEF);
		NODE_AddToken(n, func_name);
		/* failed allocation */
		if (n == NULL)
			return NULL;
		/* expecting identifiers and var words until ')' */
		while (p->current_token->type == TOKEN_IDENT) {
			token *arg_name = p->current_token; /* pointer to argument name */
			PARSER_Advance(p); /* expects ':' after arg name */
			if (p->current_token->type != TOKEN_COLON) {
				/* create error */
				p->e = ERROR_InvalidSyntax("Expected ':'", p->current_token->lineno, p->current_token->colno);
				/* free our node */
				NODE_FreeChildren(n);
				/* return */
				return NULL;
			}
			/* advance */
			PARSER_Advance(p);
			/* expecting var word */
			if (p->current_token->type != TOKEN_VAR_WORD) {
				/* create error */
				p->e = ERROR_InvalidSyntax("Expected variable type", p->current_token->lineno, p->current_token->colno);
				/* free our node */
				NODE_FreeChildren(n);
				/* return */
				return NULL;
			}
			/* assign arg type */
			token *arg_type = p->current_token;
			PARSER_Advance(p); /* advance */
			/* add tokens to node */
			NODE_AddToken(n, arg_name);
			NODE_AddToken(n, arg_type);
			/* expects ',' */
			if (p->current_token->type != TOKEN_COMMA) {
				/* create error */
				p->e = ERROR_InvalidSyntax("Expected ','", p->current_token->lineno, p->current_token->colno);
				/* free our children */
				NODE_FreeChildren(n);
				/* return */
				return NULL;
			}
			/* advance once more */
			PARSER_Advance(p);
		}
		/* expects ')' */
		if (p->current_token->type != TOKEN_RPAREN) {
			/* create error */
			p->e = ERROR_InvalidSyntax("Expected ')'", p->current_token->lineno, p->current_token->colno);
			/* free our node */
			NODE_FreeChildren(n);
			/* return */
			return NULL;
		}
		/* advance */
		PARSER_Advance(p);
		/* expecting '->' */
		if (p->current_token->type != TOKEN_ARROW) {
			/* create error */
			p->e = ERROR_InvalidSyntax("Expected '->'", p->current_token->lineno, p->current_token->colno);
			/* free our node */
			NODE_FreeChildren(n);
			/* return */
			return NULL;
		}
		/* advance */
		PARSER_Advance(p);
		/* expecting var word for return type */
		if (p->current_token->type != TOKEN_VAR_WORD) {
			/* create error */
			p->e = ERROR_InvalidSyntax("Expected variable type", p->current_token->lineno, p->current_token->colno);
			/* free our node */
			NODE_FreeChildren(n);
			/* return */
			return NULL;
		}
		/* pointer to return type token */
		token *ret_type = p->current_token;
		NODE_AddToken(n, ret_type); /* add token to list */
		/* advance */
		PARSER_Advance(p);
		/* ';' */
		if (p->current_token->type == TOKEN_EOL) PARSER_Advance(p);
		/* get statements */
		node *statements = PARSER_Statements(p);
		/* error or failed allocation */
		if (statements == NULL || p->e != NULL) {
			/* free our node */
			NODE_FreeChildren(n);
			/* return */
			return NULL;
		}
		/* expecting 'end' */
		if (!TOKEN_Matches(p->current_token, TOKEN_KWD, "end")) {
			/* create error */
			p->e = ERROR_InvalidSyntax("Expected 'end'", p->current_token->lineno, p->current_token->colno);
			/* free our node */
			NODE_FreeChildren(n);
			NODE_FreeChildren(statements);
			/* return */
			return NULL;
		}
		/* advance */
		PARSER_Advance(p);
		/* add the node */
		NODE_AddChild(n, statements);
		/* assign line and column numbers */
		n->lineno = tok->lineno;
		n->colno = tok->colno;
		/* return node */
		return n;
	}
	else if (TOKEN_Matches(tok, TOKEN_KWD, "struct")) {
		/* advance */
		PARSER_Advance(p);
		/* expecting identifier */
		if (p->current_token->type != TOKEN_IDENT) {
			/* create error */
			p->e = ERROR_InvalidSyntax("Expected identifier", p->current_token->lineno, p->current_token->colno);
			return NULL; /* exit */
		}
		/* struct name */
		token *name = p->current_token;
		/* advance */
		PARSER_Advance(p);
		/* skip past newlines */
		while (p->current_token->type == TOKEN_EOL)
			PARSER_Advance(p);
		/* integer for number of values */
		int n_of_vals = 1;
		/* allocate node */
		node *n = NODE_NewNode(NODE_STRUCT);
		/* bad allocation */
		if (n == NULL || p->e != NULL)
			return NULL;
		/* add token */
		NODE_AddToken(n, name);
		/* expecting identifier */
		if (p->current_token->type != TOKEN_IDENT) {
			/* create error */
			p->e = ERROR_InvalidSyntax("Expected identifier", p->current_token->lineno, p->current_token->colno);
			/* free node */
			NODE_FreeNode(n);
			return NULL; /* exit */
		}
		/* name */
		token *val_name = p->current_token;
		/* advance */
		PARSER_Advance(p);
		/* expects ':' */
		if (p->current_token->type != TOKEN_COLON) {
			/* create error */
			p->e = ERROR_InvalidSyntax("Expected ':'", p->current_token->lineno, p->current_token->colno);
			/* free node */
			NODE_FreeNode(n);
			return NULL; /* exit */
		}
		/* advance */
		PARSER_Advance(p);
		/* expects type */
		if (p->current_token->type != TOKEN_VAR_WORD) {
			/* create error */
			p->e = ERROR_InvalidSyntax("Expected type name", p->current_token->lineno, p->current_token->colno);
			/* free node */
			NODE_FreeNode(n);
			return NULL; /* exit */
		}
		/* type */
		token *val_type = p->current_token;
		/* advance */
		PARSER_Advance(p);
		/* add tokens */
		NODE_AddToken(n, val_name);
		NODE_AddToken(n, val_type);
		/* get other values */
		while (p->current_token->type == TOKEN_COMMA) {
			/* advance */
			PARSER_Advance(p);
			/* expects name */
			if (p->current_token->type != TOKEN_IDENT) {
				/* create error */
				p->e = ERROR_InvalidSyntax("Expected identifier", p->current_token->lineno, p->current_token->colno);
				/* free node */
				NODE_FreeNode(n);
				return NULL; /* exit */
			}
			/* name */
			val_name = p->current_token;
			/* advance */
			PARSER_Advance(p);
			/* expects ':' */
			if (p->current_token->type != TOKEN_COLON) {
				/* create error */
				p->e = ERROR_InvalidSyntax("Expected ':'", p->current_token->lineno, p->current_token->colno);
				/* free node */
				NODE_FreeNode(n);
				return NULL; /* exit */
			}
			/* advance */
			PARSER_Advance(p);
			/* expects type */
			if (p->current_token->type != TOKEN_VAR_WORD) {
				/* create error */
				p->e = ERROR_InvalidSyntax("Expected type name", p->current_token->lineno, p->current_token->colno);
				/* free node */
				NODE_FreeNode(n);
				return NULL; /* exit */
			}
			/* type */
			val_type = p->current_token;
			/* advance */
			PARSER_Advance(p);
			/* add tokens */
			NODE_AddToken(n, val_name);
			NODE_AddToken(n, val_type);
			n_of_vals++;
		}
		/* advance past newlines */
		while (p->current_token->type == TOKEN_EOL)
			PARSER_Advance(p); /* advance */
		/* expects 'end' */
		if (!TOKEN_Matches(p->current_token, TOKEN_KWD, "end")) {
			/* create error */
			p->e = ERROR_InvalidSyntax("Expected 'end'", p->current_token->lineno, p->current_token->colno);
			/* free node */
			NODE_FreeNode(n);
			return NULL; /* exit */
		}
		PARSER_Advance(p); /* advance */
		/* assign line and column numbers */
		n->lineno = tok->lineno;
		n->colno = tok->colno;
		n->c = n_of_vals;
		/* return node */
		return n;
	}
	else if (TOKEN_Matches(tok, TOKEN_KWD, "include")) {
		/* advance */
		PARSER_Advance(p);
		/* expects string as file name */
		if (p->current_token->type != TOKEN_STRING) {
			/* create error */
			p->e = ERROR_InvalidSyntax("Expected string", p->current_token->lineno, p->current_token->colno);
			return NULL; /* exit */
		}
		/* store string and advance */
		token *fname = p->current_token;
		PARSER_Advance(p); /* advance */
		/* allocate new node */
		node *n = NODE_NewNode(NODE_INCLUDE);
		/* bad memory */
		if (n == NULL)
			return NULL;
		/* add token */
		NODE_AddToken(n, fname);
		/* assign line and column numbers */
		n->lineno = tok->lineno;
		n->colno = tok->colno;
		/* return node */
		return n;
	}
	else if (TOKEN_Matches(tok, TOKEN_KWD, "sizeof")) {
		/* advance */
		PARSER_Advance(p);
		/* expects a type or an expression */
		int a; /* determine if we are checking size of type or size of value */
		if (p->current_token->type == TOKEN_VAR_WORD) a = 1;
		else a = 0;

		/* allocate new node */
		node *n = NODE_NewNode(NODE_SIZEOF);
		/* failed allocation */
		if (n == NULL)
			return NULL;
		/* size of type */
		if (a) {
			/* add token */
			NODE_AddToken(n, p->current_token);
			PARSER_Advance(p); /* advance */
		} else {
			/* get the size of a value */
			node *expr = PARSER_Expr(p); /* get an expression for value */
			/* failed allocation or error */
			if (p->e != NULL || expr == NULL) {
				/* free original node */
				NODE_FreeNode(n);
				return NULL; /* exit */
			}
			/* add child node */
			NODE_AddChild(n, expr);
		}
		/* assign line and column numbers */
		n->lineno = tok->lineno;
		n->colno = tok->colno;
		n->b = a;
		/* return node */
		return n;
	}
	else if (TOKEN_Matches(tok, TOKEN_KWD, "for")) {
		/* advance */
		PARSER_Advance(p);
		/* expects var name */
		if (p->current_token->type != TOKEN_IDENT) {
			/* create error */
			p->e = ERROR_InvalidSyntax("Expected identifier", p->current_token->lineno, p->current_token->colno);
			return NULL; /* exit */
		}
		/* assign token */
		token *var_name = p->current_token;
		PARSER_Advance(p); /* advance */
		/* expects '=' */
		if (p->current_token->type != TOKEN_EQ) {
			/* create error */
			p->e = ERROR_InvalidSyntax("Expected '='", p->current_token->lineno, p->current_token->colno);
			return NULL; /* exit */
		}
		PARSER_Advance(p); /* advance */
		/* assign expr */
		node *start = PARSER_Expr(p);
		/* error or failed memory allocation */
		if (start == NULL || p->e != NULL)
			return NULL; /* exit */
		/* expects 'to' */
		if (!TOKEN_Matches(p->current_token, TOKEN_KWD, "to")) {
			/* create error */
			p->e = ERROR_InvalidSyntax("Expected 'to'", p->current_token->lineno, p->current_token->colno);
			return NULL; /* exit */
		}
		PARSER_Advance(p); /* advance */
		/* assign expr */
		node *end = PARSER_Expr(p);
		/* error or failed allocation */
		if (end == NULL || p->e != NULL) {
			/* free */
			NODE_FreeChildren(start);
			return NULL; /* exit */
		}
		/* loop past ';' */
		while (p->current_token->type == TOKEN_EOL) PARSER_Advance(p);
		/* get statements */
		node *statements = PARSER_Statements(p);
		/* error or failed allocation */
		if (statements == NULL || p->e != NULL) {
			return NULL; /* exit */
		}
		/* expects 'end' */
		if (!TOKEN_Matches(p->current_token, TOKEN_KWD, "end")) {
			/* free statements */
			NODE_FreeChildren(statements);
			return NULL; /* exit */
		}
		PARSER_Advance(p); /* advance */
		/* allocate new node */
		node *n = NODE_NewNode(NODE_FORLOOP);
		/* failed allocation */
		if (n == NULL) {
			/* free statements */
			NODE_FreeChildren(statements);
			return NULL; /* exit */
		}
		/* add children and tokens */
		NODE_AddChild(n, statements);
		NODE_AddToken(n, var_name);
		NODE_AddChild(n, start);
		NODE_AddChild(n, end);
		/* assign line and column numbers */
		n->lineno = tok->lineno;
		n->colno = tok->colno;
		/* return node */
		return n;
	}
	else if (TOKEN_Matches(tok, TOKEN_KWD, "if")) {
		/* advance */
		PARSER_Advance(p);
		/* get expression */
		node *comp_expr = PARSER_Expr(p);
		/* memory failure or error */
		if (p->e != NULL || comp_expr == NULL)
			return NULL;
		/* advance past newline */
		if (p->current_token->type == TOKEN_EOL)
			PARSER_Advance(p);
		/* get statements */
		node *statements = PARSER_Statements(p);
		/* error */
		if (p->e != NULL || statements == NULL) {
			/* free children nodes */
			NODE_FreeChildren(comp_expr);
			if (statements != NULL) NODE_FreeChildren(statements);
			return NULL; /* exit */
		}
		/* expects 'end' */
		if (!TOKEN_Matches(p->current_token, TOKEN_KWD, "end")) {
			/* create error */
			p->e = ERROR_InvalidSyntax("Expected 'end'", p->current_token->lineno, p->current_token->colno);
			/* free stuff */
			NODE_FreeChildren(comp_expr);
			NODE_FreeChildren(statements);
			return NULL; /* exit */
		}
		PARSER_Advance(p); /* advance */
		/* allocate new node */
		node *n = NODE_NewNode(NODE_IFNODE);
		/* failed allocation */
		if (n == NULL) {
			/* free stuff */
			NODE_FreeChildren(comp_expr);
			NODE_FreeChildren(statements);
			return NULL; /* exit */
		}
		/* add childs */
		NODE_AddChild(n, comp_expr);
		NODE_AddChild(n, statements);
		/* return node */
		return n;
	}
	else if (TOKEN_Matches(tok, TOKEN_KWD, "puts")) {
		/* advance */
		PARSER_Advance(p);
		/* allocate new node */
		node *n = NODE_NewNode(NODE_PRINT);
		/* failed allocation */
		if (n == NULL)
			return NULL;
		/* get expr */
		node *expr = PARSER_Expr(p);
		/* failed allocation or error */
		if (p->e != NULL || expr == NULL) {
			/* free original node */
			NODE_FreeNode(n);
			/* return pointer to null */
			return NULL;
		}
		/* add child node */
		NODE_AddChild(n, expr);
		/* assign line and column numbers */
		n->lineno = tok->lineno;
		n->colno = tok->colno;
		/* return node */
		return n;
	}
	else if (tok->type == TOKEN_IDENT) {
		/* advance */
		PARSER_Advance(p);
		/* '(' function call */
		if (p->current_token->type == TOKEN_LPAREN) {
			PARSER_Advance(p); /* advance */
			/* allocate new node */
			node *n = NODE_NewNode(NODE_CALL);
			/* failed allocation */
			if (n == NULL)
				return NULL;
			/* get first argument */
			if (p->current_token->type != TOKEN_RPAREN) {
				/* get expression */
				node *expr = PARSER_Expr(p);
				/* error or memory failure */
				if (expr == NULL || p->e != NULL) {
					/* free node */
					NODE_FreeChildren(n);
					return NULL; /* exit */
				}
				/* add as child node */
				NODE_AddChild(n, expr);
			}
			/* while ',' */
			while (p->current_token->type == TOKEN_COMMA) {
				/* advance */
				PARSER_Advance(p);
				/* get expression */
				node *expr = PARSER_Expr(p);
				/* error or memory failure */
				if (expr == NULL || p->e != NULL) {
					/* free node */
					NODE_FreeChildren(n);
					return NULL; /* exit */
				}
				/* add as child node */
				NODE_AddChild(n, expr);
			}
			/* expects ')' */
			if (p->current_token->type != TOKEN_RPAREN) {
				/* create runtime error */
				p->e = ERROR_InvalidSyntax("Expected ')'", p->current_token->lineno, p->current_token->colno);
				/* free node */
				NODE_FreeChildren(n);
				return NULL; /* exit */
			}
			PARSER_Advance(p); /* advance */
			/* add name */
			NODE_AddToken(n, tok);
			/* assign line and column numbers */
			n->lineno = tok->lineno;
			n->colno = tok->colno;
			/* return node */
			return n;
		}
		/* '[' get item from index */
		else if (p->current_token->type == TOKEN_LBRACKET) {
			PARSER_Advance(p); /* advance */
			/* get expression */
			node *expr = PARSER_Expr(p);
			/* failed allocation or error */
			if (p->e != NULL || expr == NULL)
				return NULL;
			/* expecting ']' */
			if (p->current_token->type != TOKEN_RBRACKET) {
				/* create error */
				p->e = ERROR_InvalidSyntax("Expected ']'", p->current_token->lineno, p->current_token->colno);
				/* free expr */
				NODE_FreeChildren(expr);
				return NULL; /* exit */
			}
			/* advance */
			PARSER_Advance(p);
			/* set item at index */
			if (p->current_token->type == TOKEN_EQ) {
				PARSER_Advance(p); /* advance */
				node *value = PARSER_Expr(p); /* get our value */
				/* failed allocation or error */
				if (value == NULL || p->e != NULL) {
					/* free expr */
					NODE_FreeChildren(expr);
					return NULL; /* exit */
				}
				/* allocate new node */
				node *n = NODE_NewNode(NODE_SETITEM);
				/* failed allocation */
				if (n == NULL) {
					NODE_FreeChildren(expr);
					NODE_FreeChildren(value);
					return NULL; /* exit */
				}
				/* add childs */
				NODE_AddChild(n, expr);
				NODE_AddToken(n, tok);
				/* add more childs */
				NODE_AddChild(n, value);
				/* assign line and column numbers */
				n->lineno = tok->lineno;
				n->colno = tok->colno;
				/* return node */
				return n;
			}
			/* allocate new node */
			node *n = NODE_NewNode(NODE_GETITEM);
			/* failed allocation */
			if (n == NULL) {
				NODE_FreeChildren(expr); /* free expr */
				return NULL; /* exit */
			}
			/* add child */
			NODE_AddChild(n, expr);
			NODE_AddToken(n, tok);
			/* assign line and column numbers */
			n->lineno = tok->lineno;
			n->colno = tok->colno;
			/* return node */
			return n;
		}
		/* allocate new node */
		node *n = NODE_NewNode(NODE_VARAC);
		/* failed allocation */
		if (n == NULL)
			return NULL;
		/* add name */
		NODE_AddToken(n, tok);
		/* assign line and column numbers */
		n->lineno = tok->lineno;
		n->colno = tok->colno;
		/* return node */
		return n;
	}
	else if (tok->type == TOKEN_DOLLAR) {
		/* advance */
		PARSER_Advance(p);
		/* allocate new node */
		node *n = NODE_NewNode(NODE_VALUE);
		/* failed allocation */
		if (n == NULL)
			return NULL;
		/* expression to get value from */
		node *expr = PARSER_Expr(p);
		/* error or memory failure */
		if (p->e != NULL || expr == NULL) {
			/* free */
			NODE_FreeChildren(expr);
			NODE_FreeNode(n);
			return NULL; /* exit */
		}
		/* add node */
		NODE_AddChild(n, expr);
		/* return node */
		return n;
	}
	else if (tok->type == TOKEN_ARROW) {
		/* advance */
		PARSER_Advance(p);
		/* allocate new node */
		node *n = NODE_NewNode(NODE_ADDRESS);
		/* failed allocation */
		if (n == NULL)
			return NULL;
		/* expression to get value from */
		node *expr = PARSER_Expr(p);
		/* error or memory failure */
		if (p->e != NULL || expr == NULL) {
			/* free expr */
			if (expr != NULL) NODE_FreeChildren(expr);
			NODE_FreeNode(n);
			return NULL; /* exit */
		}
		/* add child node */
		NODE_AddChild(n, expr);
		/* return node */
		return n;
	}
	else if (tok->type == TOKEN_LSQUARE) {
		/* advance */
		PARSER_Advance(p);
		/* expects var word */
		token *var_type = p->current_token;
		if (var_type->type != TOKEN_VAR_WORD) {
			/* create error */
			p->e = ERROR_InvalidSyntax("Expected variable type", var_type->lineno, var_type->colno);
			return NULL; /* exit */
		}
		/* advance */
		PARSER_Advance(p);
		/* expects ',' */
		if (p->current_token->type != TOKEN_COMMA) {
			/* create error and exit */
			p->e = ERROR_InvalidSyntax("Expected ','", p->current_token->lineno, p->current_token->colno);
			return NULL;
		}
		/* advance */
		PARSER_Advance(p);
		/* expects integer */
		if (p->current_token->type != TOKEN_INT) {
			/* create error and exit */
			p->e = ERROR_InvalidSyntax("Expected integer for array size", p->current_token->lineno, p->current_token->colno);
			return NULL;
		}
		/* store token */
		token *array_size = p->current_token;
		/* advance */
		PARSER_Advance(p);
		/* expects '}' */
		if (p->current_token->type != TOKEN_RSQUARE) {
			/* create error and exit */
			p->e = ERROR_InvalidSyntax("Expected '}'", p->current_token->lineno, p->current_token->colno);
			return NULL;
		}
		/* advance */
		PARSER_Advance(p);
		/* allocate new node */
		node *n = NODE_NewNode(NODE_ARRAY);
		/* failed allocation */
		if (n == NULL)
			return NULL;
		/* add tokens */
		NODE_AddToken(n, var_type);
		NODE_AddToken(n, array_size);
		/* assign line and column numbers */
		n->lineno = tok->lineno;
		n->colno = tok->colno;
		/* return node */
		return n;
	}
	else if (tok->type == TOKEN_VAR_WORD) {
		/* advance */
		PARSER_Advance(p);
		/* variables for name, type, and value */
		token *var_name = NULL;
		token *var_type = tok;
		node *var_value = NULL;
		int is_array = 0;
		int array_size = 0;
		/* '[' means we are making array */
		if (p->current_token->type == TOKEN_LBRACKET) {
			/* advance */
			PARSER_Advance(p);
			/* expecting int */
			if (p->current_token->type != TOKEN_INT) {
				/* create error */
				p->e = ERROR_InvalidSyntax("Expected integer for array size", p->current_token->lineno, p->current_token->colno);
				return NULL; /* exit */
			}
			/* assign is_array and array_size */
			is_array = 1;
			array_size = atoi(p->current_token->value);
			/* expecting ']' */
			PARSER_Advance(p);
			if (p->current_token->type != TOKEN_RBRACKET) {
				/* create error */
				p->e = ERROR_InvalidSyntax("Expected ']'", p->current_token->lineno, p->current_token->colno);
				return NULL; /* exit */
			}
			/* advance once more */
			PARSER_Advance(p);
		}
		/* expecting identifier for var name */
		if (p->current_token->type != TOKEN_IDENT) {
			/* create error */
			p->e = ERROR_InvalidSyntax("Expected identifier", p->current_token->lineno, p->current_token->colno);
			return NULL; /* exit */
		}
		/* set var name and advance */
		var_name = p->current_token;
		PARSER_Advance(p);
		/* expecting '=' */
		if (p->current_token->type != TOKEN_EQ) {
			/* create error */
			p->e = ERROR_InvalidSyntax("Expected '='", p->current_token->lineno, p->current_token->colno);
			return NULL; /* exit */
		}
		/* advance */
		PARSER_Advance(p);
		/* get value of var */
		var_value = PARSER_Expr(p);
		/* failed allocation or error */
		if (var_value == NULL || p->e != NULL) {
			/* free value */
			NODE_FreeNode(var_value);
			/* return */
			return NULL;
		}
		/* allocate new node */
		node *n = NODE_NewNode(NODE_VARDEC);
		/* failed allocation */
		if (n == NULL) {
			NODE_FreeChildren(var_value); /* free value */
			return NULL; /* exit */
		}
		/* add tokens and nodes */
		NODE_AddToken(n, var_name);
		NODE_AddToken(n, var_type);
		NODE_AddChild(n, var_value);
		/* assign line and column numbers as well as array stuff */
		n->lineno = var_type->lineno;
		n->colno = var_type->colno;
		n->b = is_array;
		n->c = array_size;
		/* return node */
		return n;
	}
	else {
		/* unexpected token */
		p->e = ERROR_InvalidSyntax("Expected int, float, or string", tok->lineno, tok->colno); /* new error */
		return NULL;
	}
}

node *PARSER_BinOp(parser *p, node* (*func)(parser*), int types[], int n_of_types) {
	/* get the first left node */
	node *left = func(p); /* get the value from the function */
	/* new node */
	node *n = NULL; /* automatically null */
	node *right = NULL; /* right node */

	/* failed allocation or error */
	if (p->e != NULL || left == NULL)
		return NULL;

	/* operation token */
	token *op_token = NULL;

	/* while the token is in the list */
	while (is_int_in(p->current_token->type, types, n_of_types)) {
		/* get op token */
		op_token = (p->current_token);
		/* advance */
		PARSER_Advance(p);

		/* get right */
		right = func(p);

		/* allocate new */
		n = NODE_NewNode(NODE_BINOP);
		/* add children */
		NODE_AddChild(n, left); /* left node */
		NODE_AddChild(n, right); /* right node */
		/* add token */
		NODE_AddToken(n, op_token);
		/* assign line and column numbers */
		n->lineno = left->lineno;
		n->colno = left->colno;
		/* set left node to new */
		left = n; /* left still has address stored in new */
	}

	return left; /* return new node */
}

void PARSER_FreeParser(parser *p) {
	/* free the node if it exists */
	if (p->newNode != NULL) {
		NODE_FreeChildren(p->newNode); /* automatically free node and children */
	}
	/* free error */
	if (p->e != NULL) ERROR_FreeError(p->e);
	/* free the parser */
	MEMORY_Free(p);
}

void PARSER_Parse(parser *p) {
	/* set new node */
	p->newNode = PARSER_Statements(p);
}

int is_int_in(int x, int l[], int l_len) {
	/* var to determine */
	int t = 0;

	/* loop through list */
	for (int i = 0; i < l_len; i++) {
		/* are they the same? */
		if (l[i] == x)
			t = 1; /* found */
	}
	/* return */
	return t;
}

#ifdef __cplusplus /* c++ check */
}
#endif