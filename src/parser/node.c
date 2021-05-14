/* nodes */
#include "node.h" /* our header */
#include "token.h" /* tokens */
#include "memory.h" /* memory management */

#include <stdlib.h> /* realloc */
#include <stdio.h> /* printf */
#include <string.h> /* strlen, strcpy, ... */

#ifdef __cplusplus /* c++ check */
extern "C" {
#endif

node *NODE_NewNode(int type) {
	/* allocate a new node */
	node *n = MEMORY_Malloc(node);
	/* failed allocation */
	if (!n || n == NULL)
		return NULL;

	/* assign values */
	n->type = type; /* type of node */
	n->tokens = (token**)malloc(sizeof(token*) * 8); /* tokens */
	n->children = (node**)malloc(sizeof(node*) * 8); /* child nodes */
	n->n_of_children = 0; /* number of children */
	n->n_of_toks = 0; /* number of tokens */
	n->n_toks_cap = 8; /* capacity of token list */
	n->n_children_cap = 8; /* capacity of children list */
	n->b = 0; /* boolean value for other things such as array declarations */
	n->c = 0;
	n->d = 0;
	return n; /* return new node */
}

void NODE_FreeNode(node *n) { /* deprecated, i recommended you use NODE_FreeChildren which automatically frees children as well */
	/* free node */
	MEMORY_Free(n);
}

void NODE_FreeChildren(node *n) {
	if (n->n_of_children > 0)
		/* loop through children */
		for (int i = 0; i < n->n_of_children; i++) {
			/* free children and tokens */
			NODE_FreeChildren(n->children[i]);
		}
	/* free token list */
	MEMORY_Free(n->tokens);
	/* free children list */
	MEMORY_Free(n->children);
	/* free the node */
	MEMORY_Free(n);
}

node *NODE_AddChild(node *parent, node *child) {
	/* check the capacity */
	if (parent->n_of_children >= parent->n_children_cap) {
		/* realloc list */
		parent->children = (node**)realloc(parent->children, sizeof(node*) * parent->n_children_cap * 2);
		/* update cap */
		parent->n_children_cap *= 2;
	}
	/* add the token to next spot in list */
	parent->children[parent->n_of_children++] = child;
	/* return the parent node */
	return parent;
}

node *NODE_AddToken(node *parent, token *t) {
	/* check the capacity */
	if (parent->n_of_toks >= parent->n_toks_cap) {
		/* realloc list */
		parent->tokens = (token**)realloc(parent->tokens, sizeof(token*) * parent->n_toks_cap * 2);
		/* update cap */
		parent->n_toks_cap *= 2;
	}
	/* add the token to next spot in list */
	parent->tokens[parent->n_of_toks++] = t;
	/* return parent node */
	return parent;
}

/*
context: when calling functions from outside of the main file that the call is in, the body node of the
function itself would have already been freed have we had not already copied the node and all it's contents.
*/
node *NODE_CopyNode(node *n) {
	/* allocate new node */
	node *newNode = NODE_NewNode(n->type);
	/* create copies of children */
	for (int i = 0; i < n->n_of_children; i++) {
		/* add child */
		NODE_AddChild(newNode, NODE_CopyNode(n->children[i]));
	}
	/* create copies of tokens */
	for (int i = 0; i < n->n_of_toks; i++) {
		/* create new value */
		char *value = (char*)malloc(strlen(n->tokens[i]->value) + 2);
		strcpy(value, n->tokens[i]->value);
		/* create new token */
		token *t = TOKEN_NewToken(n->tokens[i]->type, value, n->tokens[i]->lineno, n->tokens[i]->colno);
		/* add token to node */
		NODE_AddToken(newNode, t);
	}
	/* assign arbitrary values and line and column numbers */
	newNode->lineno = n->lineno;
	newNode->colno = n->colno;
	newNode->b = n->b;
	newNode->c = n->c;
	newNode->d = n->d;
	/* return node */
	return newNode;
}

void NODE_FreeCopyNode(node *n) {
	/* free tokens */
	for (int i = 0; i < n->n_of_toks; i++) {
		/* free a token */
		TOKEN_FreeToken(n->tokens[i]);
	}
	/* free node and it's children */
	NODE_FreeChildren(n);
}

void NODE_PrintTree(node *n) {
	/* check the type */
	switch (n->type) {
		/* binop node */
		case NODE_BINOP:
			/* print the binop node stuff */
			printf("(");
			NODE_PrintTree(n->children[0]);
			printf(" %d ", n->tokens[0]->type);
			NODE_PrintTree(n->children[1]);
			printf(")");
			break;
		case NODE_UNOP:
			/* print the unop node stuff */
			printf("(%d ", n->tokens[0]->type);
			NODE_PrintTree(n->children[0]);
			printf(")");
			break;
		case NODE_INT:
			/* print an int */
			printf("%s", n->tokens[0]->value);
			break;
		case NODE_STRING:
			/* print a string */
			printf("'%s'", n->tokens[0]->value);
			break;
		case NODE_PRINT:
			/* print the representation of printing a value */
			printf("puts ");
			NODE_PrintTree(n->children[0]); /* print the value */
			printf(";"); /* print EOL char */
			break;
		case NODE_STATEMENTS:
			/* print all nodes */
			for (int i = 0; i < n->n_of_children; i++) {
				NODE_PrintTree(n->children[i]); /* print the child node */
				printf("\n"); /* print newline */
			}
			break;
		default:
			/* unknown type */
			printf("(Unknown token of type %d)", n->type);
	}
}

#ifdef __cplusplus /* c++ check */
}
#endif