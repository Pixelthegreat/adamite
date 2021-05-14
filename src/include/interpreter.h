/* interpreter */
#include "node.h" /* nodes */
#include "error.h" /* error handling */
#include "object.h" /* object system */

#ifndef INTERPRETER_H
#define INTERPRETER_H

#ifdef __cplusplus /* c++ check */
extern "C" {
#endif

typedef struct _ADAMITE_Lib_Interpreter {
	error *e; /* current error */
} interpreter;

interpreter *INTERPRETER_NewInterpreter(); /* create new interpreter */
void INTERPRETER_FreeInterpreter(interpreter *i); /* free interpreter */
object *INTERPRETER_VisitNumber(interpreter *i, node *n); /* visit a number */
object *INTERPRETER_VisitString(interpreter *i, node *n); /* visit a string */
object *INTERPRETER_VisitBinOp(interpreter *i, node *n); /* visit a binary operation */
object *INTERPRETER_VisitUnOp(interpreter *i, node *n); /* visit a unary operation */
object *INTERPRETER_VisitPrint(interpreter *i, node *n); /* visit a print statement */
object *INTERPRETER_VisitStatements(interpreter *i, node *n); /* visit multiple statements */
object *INTERPRETER_VisitVarAssign(interpreter *i, node *n); /* visit a variable assignment */
object *INTERPRETER_VisitVarAccess(interpreter *i, node *n); /* visit to access a variable's value */
object *INTERPRETER_VisitSizeof(interpreter *i, node *n); /* sizeof value */
object *INTERPRETER_VisitFuncDef(interpreter *i, node *n); /* function definition */
object *INTERPRETER_VisitCall(interpreter *i, node *n); /* call a function */
object *INTERPRETER_VisitAddress(interpreter *i, node *n); /* return address of object */
object *INTERPRETER_VisitValue(interpreter *i, node *n); /* return object of address */
object *INTERPRETER_VisitGetItem(interpreter *i, node *n); /* return value at index of array or string */
object *INTERPRETER_VisitArray(interpreter *i, node *n); /* create array object */
object *INTERPRETER_VisitIfNode(interpreter *i, node *n); /* if statement */
object *INTERPRETER_VisitSetItem(interpreter *i, node *n); /* set item in array */
object *INTERPRETER_VisitForLoop(interpreter *i, node *n); /* loop for a number of times */
object *INTERPRETER_VisitStruct(interpreter *i, node *n); /* create a struct */
object *INTERPRETER_VisitStdin(interpreter *i, node *n); /* get user keyboard input */
object *INTERPRETER_VisitInclude(interpreter *i, node *n); /* run the contents of a file */
object *INTERPRETER_VisitNew(interpreter *i, node *n); /* dynamically allocate an object */
object *INTERPRETER_VisitWhile(interpreter *i, node *n); /* while loop */
object *INTERPRETER_Visit(interpreter *i, node *n); /* visit a node */

#ifdef __cplusplus /* c++ check */
}
#endif

#endif /* INTERPRETER_H */