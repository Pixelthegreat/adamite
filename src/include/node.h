#include "token.h" /* node includes token array */

#ifndef NODE_H
#define NODE_H

#ifdef __cplusplus /* c++ check */
extern "C" {
#endif

#define NODE_INT		0 	/* integers 						*/
#define NODE_FLOAT		1 	/* floating point numbers 			*/
#define NODE_STRING		2 	/* strings							*/
#define NODE_UNOP		3 	/* unary operations					*/
#define NODE_BINOP		4 	/* binary operations				*/
#define NODE_IOFUNC		5 	/* builtin i/o functions			*/
#define NODE_VARDEC		6 	/* variable declarations			*/
#define NODE_KEYWORD	7 	/* any regular keyword				*/
#define NODE_PRINT		8 	/* print statement (puts)			*/
#define NODE_STATEMENTS	9 	/* multiple statements				*/
#define NODE_VARAC		10	/* access a variable				*/
#define NODE_SIZEOF		11	/* get the size of a value			*/
#define NODE_FUNCDEF	12	/* function definition				*/
#define NODE_CALL		13	/* call a function					*/
#define NODE_ADDRESS	14	/* get address of variable			*/
#define NODE_VALUE		15	/* get value at address				*/
#define NODE_GETITEM	16	/* get an item from index			*/
#define NODE_ARRAY		17	/* create an array					*/
#define NODE_IFNODE		18	/* if statement						*/
#define NODE_SETITEM	19	/* set an item at index				*/
#define NODE_FORLOOP	20	/* for loop							*/
#define NODE_STRUCT		21	/* struct definition				*/
#define NODE_STDIN		22	/* get user keyboard input			*/
#define NODE_INCLUDE	23	/* run an external file				*/
#define NODE_NEW		24	/* dynamically allocate object		*/
#define NODE_WHILE		25	/* while loop						*/

typedef struct _ADAMITE_Lib_Node { /* parser nodes can be recursive */
	int type; /* type of node */
	token **tokens; /* tokens put in node */
	struct _ADAMITE_Lib_Node **children; /* children of node */
	unsigned int n_of_children; /* number of children */
	unsigned int n_of_toks; /* number of tokens */
	unsigned int n_children_cap; /* number of items that can be held in the array of children */
	unsigned int n_toks_cap; /* number of items that can be held in the array of tokens */
	int lineno; /* line number */
	int colno; /* column number */
	int b; /* boolean value for related things */
	int c; /* other values */
	int d; /* other values */
} node;

node *NODE_NewNode(int type); /* allocate a new node */
void NODE_FreeNode(node *n); /* free a node */
void NODE_FreeChildren(node *n); /* free the node's children */
node *NODE_AddChild(node *parent, node *child); /* add a child to a node */
node *NODE_AddToken(node *parent, token *t); /* add a token */
node *NODE_CopyNode(node *n); /* (made for functions) copy a node from parser to a node that is completely free from parser */
void NODE_FreeCopyNode(node *n); /* free node that has been copied through function */
void NODE_PrintTree(node *n); /* print the tree of a node */
int NODE_CheckNodeType(node *n); /* returns 1 if the node type is a known type, 0 if it is not */

#ifdef __cplusplus /* c++ check */
}
#endif

#endif /* NODE_H */