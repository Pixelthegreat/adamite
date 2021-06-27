/* interpreter */
#include "interpreter.h" /* our header */
#include "objectio.h" /* object io */
#include "object.h" /* objects */
#include "node.h" /* nodes */
#include "memory.h" /* memory management */
#include "storage.h" /* variable storage */
#include "error.h" /* error handling */
#include "token.h" /* tokens */
#include "names.h" /* name storage */
#include "run.h" /* run a file */

#include <stdlib.h> /* atoi */
#include <string.h> /* strcmp */
#include <stdio.h> /* printf */

#ifdef __cplusplus /* c++ check */
extern "C" {
#endif

interpreter *INTERPRETER_NewInterpreter() {
	/* allocate new interpreter */
	interpreter *i = MEMORY_Malloc(interpreter);
	/* failed allocation */
	if (i == NULL)
		return NULL;
	i->e = NULL; /* error */
	return i; /* return */
}

void INTERPRETER_FreeInterpreter(interpreter *i) {
	/* free error */
	if (i->e != NULL) MEMORY_Free(i->e);
	/* free interp */
	MEMORY_Free(i);
}

object *INTERPRETER_Visit(interpreter *i, node *n) {
	/* check the node type */
	if (n->type == NODE_INT || n->type == NODE_FLOAT)
		/* number node */
		return INTERPRETER_VisitNumber(i, n);
	else if (n->type == NODE_STRING)
		/* string node */
		return INTERPRETER_VisitString(i, n);
	else if (n->type == NODE_BINOP)
		/* binary operation */
		return INTERPRETER_VisitBinOp(i, n);
	else if (n->type == NODE_UNOP)
		/* unary operation */
		return INTERPRETER_VisitUnOp(i, n);
	else if (n->type == NODE_STATEMENTS)
		/* multiple statements */
		return INTERPRETER_VisitStatements(i, n);
	else if (n->type == NODE_PRINT)
		/* print values */
		return INTERPRETER_VisitPrint(i, n);
	else if (n->type == NODE_VARDEC)
		/* variable declaration */
		return INTERPRETER_VisitVarAssign(i, n);
	else if (n->type == NODE_VARAC)
		/* variable access */
		return INTERPRETER_VisitVarAccess(i, n);
	else if (n->type == NODE_SIZEOF)
		/* access size of value */
		return INTERPRETER_VisitSizeof(i, n);
	else if (n->type == NODE_FUNCDEF)
		/* function definition */
		return INTERPRETER_VisitFuncDef(i, n);
	else if (n->type == NODE_ADDRESS)
		/* get address of value */
		return INTERPRETER_VisitAddress(i, n);
	else if (n->type == NODE_CALL)
		/* call a function */
		return INTERPRETER_VisitCall(i, n);
	else if (n->type == NODE_GETITEM)
		/* get an item from an index */
		return INTERPRETER_VisitGetItem(i, n);
	else if (n->type == NODE_ARRAY)
		/* create an array object */
		return INTERPRETER_VisitArray(i, n);
	else if (n->type == NODE_IFNODE)
		/* if statement */
		return INTERPRETER_VisitIfNode(i, n);
	else if (n->type == NODE_SETITEM)
		/* set an item in an array */
		return INTERPRETER_VisitSetItem(i, n);
	else if (n->type == NODE_FORLOOP)
		/* loop for a number of times */
		return INTERPRETER_VisitForLoop(i, n);
	else if (n->type == NODE_VALUE)
		/* "dereference" a pointer */
		return INTERPRETER_VisitValue(i, n);
	else if (n->type == NODE_STRUCT)
		/* create struct */
		return INTERPRETER_VisitStruct(i, n);
	else if (n->type == NODE_STDIN)
		/* get user keyboard interrupt */
		return INTERPRETER_VisitStdin(i, n);
	else if (n->type == NODE_INCLUDE)
		/* include a file */
		return INTERPRETER_VisitInclude(i, n);
	else if (n->type == NODE_NEW)
		/* dynamically allocate */
		return INTERPRETER_VisitNew(i, n);
	else if (n->type == NODE_WHILE)
		/* while loop */
		return INTERPRETER_VisitWhile(i, n);

	else /* no method found */
		return NULL;
}

object *INTERPRETER_VisitInclude(interpreter *i, node *n) {
	/* get filename */
	const char *fname = n->tokens[0]->value;
	/* run file */
	int code = run(fname);
	/* code that is not 0 means error */
	if (code != 0) {
		/* create new error */
		i->e = ERROR_RuntimeError("Failed to finish executing script", n->lineno, n->colno);
		return NULL; /* exit */
	}
	/* return code */
	return OBJECT_NewInt(code);
}

object *INTERPRETER_VisitNew(interpreter *i, node *n) {
	/* create an array */
	if (n->b) {
		/* visit the expression */
		object *o = INTERPRETER_Visit(i, n->children[0]);
		/* error or failed allocation */
		if (o == NULL || i->e != NULL) {
			return NULL; /* exit */
		}
		/* type is not integer */
		if (o->type != OBJECT_INT) {
			/* create error */
			i->e = ERROR_RuntimeError("Array size must be integer", n->children[0]->lineno, n->children[1]->colno);
			/* free stuff */
			if (!STORAGE_Find(o)) OBJECT_FreeObject(o);
			return NULL; /* exit */
		}
		/* get value and free object */
		int arr_sz = *(int*)o->value;
		if (!STORAGE_Find(o)) OBJECT_FreeObject(o);
		/* get type */
		uint8_t obj_type = 255;
		char *v_type = (char*)n->tokens[0]->value;
		/* int */
		if (!strcmp(v_type, "int")) obj_type = OBJECT_INT;
		/* char */
		if (!strcmp(v_type, "char")) obj_type = OBJECT_CHAR;
		/* instance */
		if (!strcmp(v_type, "inst")) obj_type = OBJECT_INSTANCE;
		/* string */
		if (!strcmp(v_type, "str")) obj_type = OBJECT_STRING;
		/* create array */
		object *a = OBJECT_NewArray(obj_type, arr_sz);
		/* register object */
		a = STORAGE_Register(a);
		/* return pointer */
		return OBJECT_NewInt((int)a);
	}
	/* get type */
	char *v_type = (char*)n->tokens[0]->value;
	object *o = NULL;
	/* create an int */
	if (!strcmp(v_type, "int")) o = OBJECT_NewInt(0);
	/* create a char */
	if (!strcmp(v_type, "char")) o = OBJECT_NewChar(0);
	/* create a string */
	if (!strcmp(v_type, "str")) o = OBJECT_NewString("");
	/* register object */
	o = STORAGE_Register(o);
	/* return object */
	return OBJECT_NewInt((int)o);
}

object *INTERPRETER_VisitStdin(interpreter *i, node *n) {
	/* return user input from objectio */
	return OBJECTIO_Input();
}

object *INTERPRETER_VisitStruct(interpreter *i, node *n) {
	/* create arrays for names and values */
	char **names = (char**)malloc(sizeof(char*) * n->c);
	uint8_t *types = (uint8_t*)malloc(sizeof(uint8_t) * n->c);
	int n_of_vals = n->c;
	/* add names and values */
	char *name = (char*)malloc(strlen(n->tokens[0]->value)+1);
	strcpy(name, n->tokens[0]->value);
	/* loop through tokens */
	for (int j = 0; j < n_of_vals * 2; j += 2) {
		/* store name and type */
		uint8_t val_type = 255;
		char *val_name = (char*)n->tokens[j+1]->value;
		/* int */
		if (!strcmp(n->tokens[j+2]->value, "int")) val_type = OBJECT_INT;
		/* char */
		if (!strcmp(n->tokens[j+2]->value, "char")) val_type = OBJECT_CHAR;
		/* string */
		if (!strcmp(n->tokens[j+2]->value, "str")) val_type = OBJECT_STRING;
		/* struct instance */
		if (!strcmp(n->tokens[j+2]->value, "inst")) val_type = OBJECT_INSTANCE;
		/* copy name */
		char *_val_name = (char*)malloc(strlen(val_name)+2);
		strcpy(_val_name, val_name);
		/* add name and type to list */
		names[j/2] = _val_name;
		types[j/2] = val_type;
	}
	/* create and register struct */
	object *st = STORAGE_Register(OBJECT_NewStruct(name, types, names, n_of_vals));
	NAMES_Assign(name, st);
	/* return struct */
	return st;
}

object *INTERPRETER_VisitArray(interpreter *i, node *n) {
	/* get tokens */
	token *var_type = n->tokens[0];
	token *array_size = n->tokens[1];
	int arr_sz = atoi(array_size->value);
	/* get type */
	uint8_t tp = 255;
	/* int */
	if (!strcmp((char*)var_type->value, "int")) tp = OBJECT_INT;
	/* char */
	if (!strcmp((char*)var_type->value, "char")) tp = OBJECT_CHAR;
	/* str */
	if (!strcmp((char*)var_type->value, "str")) tp = OBJECT_STRING;
	/* struct instance */
	if (!strcmp((char*)var_type->value, "inst")) tp = OBJECT_INSTANCE;
	/* new array */
	return OBJECT_NewArray(tp, arr_sz);
}

object *INTERPRETER_VisitNumber(interpreter *i, node *n) {
	/* int */
	if (n->type == NODE_INT) {
		/* get value of int */
		int x = atoi(n->tokens[0]->value);
		/* return new int */
		return OBJECT_NewInt(x);
	}
	/* otherwise, return null */
	return NULL;
}

object *INTERPRETER_VisitString(interpreter *i, node *n) {
	/* return new string object */
	return OBJECT_NewString(n->tokens[0]->value);
}

object *INTERPRETER_VisitSizeof(interpreter *i, node *n) {
	/* return the size of a value or type */
	/* final size value */
	int size = 0;
	if (n->b) {
		/* get size of type */
		if (!strcmp(n->tokens[0]->value, "int")) size = sizeof(int);
		else if (!strcmp(n->tokens[0]->value, "char")) size = sizeof(char);
		else if (!strcmp(n->tokens[0]->value, "str")) size = sizeof(char*);
	}
	else {
		/* get size of value */
		object *o = INTERPRETER_Visit(i, n->children[0]);
		/* error or failed allocation */
		if (i->e != NULL || o == NULL) {
			/* free if neccessary */
			if (o != NULL && !STORAGE_Find(o)) OBJECT_FreeObject(o);
			/* return */
			return NULL;
		}
		/* arrays */
		if (o->type == OBJECT_ARRAY) {
			/* array pointer */
			arrayObject *a = (arrayObject*)o->value;
			/* length of array */
			size = a->size;
			/* int array */
			if (a->array_type == OBJECT_INT) size *= sizeof(int);
			/* char array */
			else if (a->array_type == OBJECT_CHAR) size *= sizeof(char);
		}
		/* int */
		else if (o->type == OBJECT_INT) size = sizeof(int);
		/* char */
		else if (o->type == OBJECT_CHAR) size = sizeof(char);
		/* string */
		else if (o->type == OBJECT_STRING) size = strlen((char*)o->value) * sizeof(char);
		/* free stuff */
		if (!STORAGE_Find(o)) OBJECT_FreeObject(o);
	}
	/* return new int */
	return OBJECT_NewInt(size);
}

object *INTERPRETER_VisitStatements(interpreter *i, node *n) {
	object *o = NULL; /* default value */

	/* loop through statement nodes */
	for (int j = 0; j < n->n_of_children; j++) {
		/* free previous object if not null and not registered */
		if (o != NULL) {
			if (!STORAGE_Find(o)) {
				OBJECT_FreeObject(o); /* free object */
			}
		}
		/* visit child */
		o = INTERPRETER_Visit(i, n->children[j]);
	
		/* check for memory issue or error */
		if (o == NULL || i->e != NULL)
			break; /* break out of loop */
	}
	
	/* return */
	return o;
}

object *INTERPRETER_VisitPrint(interpreter *i, node *n) {
	/* visit the object */
	object *o = INTERPRETER_Visit(i, n->children[0]);
	/* memory failure or error */
	if (o == NULL || i->e != NULL) {
		/* free object */
		if (o != NULL && !STORAGE_Find(o))
			/* free */
			OBJECT_FreeObject(o);
		/* return */
		return NULL;
	}
	/* print the object with objectio */
	OBJECTIO_PrintObject(o);
	/* return object */
	return o;
}

object *INTERPRETER_VisitFuncDef(interpreter *i, node *n) {
	/* get function name, argument names, and argument types */
	char *func_name = (char*)n->tokens[0]->value; /* function name */
	/* copy function name to new buffer */
	char *fname = (char*)malloc(strlen(func_name) + 1);
	strcpy(fname, func_name);
	func_name = fname;
	/* */
	int n_of_args = (n->n_of_toks - 2) / 2; /* number of arguments */
	char **arg_names = (char**)malloc(sizeof(char*) * n_of_args); /* argument names */
	uint8_t *arg_types = (uint8_t*)malloc(sizeof(uint8_t) * n_of_args); /* argument types */
	uint8_t ret_type = 255; /* default return type */
	char *rt = (char*)n->tokens[n->n_of_toks-1]->value;
	if (!strcmp(rt, "int")) ret_type = OBJECT_INT; /* int */
	if (!strcmp(rt, "char")) ret_type = OBJECT_CHAR; /* char */
	if (!strcmp(rt, "str")) ret_type = OBJECT_STRING; /* string */
	if (!strcmp(rt, "inst")) ret_type = OBJECT_INSTANCE; /* struct instance */
	/* loop through tokens and copy argument names and types */
	for (int i = 0; i < n_of_args * 2; i += 2) {
		uint8_t arg_type = 255; /* argument type */
		if (!strcmp(n->tokens[i + 2]->value, "int")) arg_type = OBJECT_INT;
		if (!strcmp(n->tokens[i + 2]->value, "char")) arg_type = OBJECT_CHAR;
		if (!strcmp(n->tokens[i + 2]->value, "str")) arg_type = OBJECT_STRING;
		if (!strcmp(n->tokens[i + 2]->value, "inst")) arg_type = OBJECT_INSTANCE;
		arg_types[i / 2] = arg_type;
		/* copy argument name */
		char *arg_name = (char*)malloc(strlen((char*)n->tokens[1 + i]->value));
		strcpy(arg_name, n->tokens[1 + i]->value);
		/* assign argument name */
		arg_names[i / 2] = /*(char*)n->tokens[1 + i]->value*/arg_name;
	}
	/* body of function */
	node *body_node = NODE_CopyNode(n->children[0]);
	/* create a new function object */
	object *f = STORAGE_Register(OBJECT_NewFunction(func_name, ret_type, arg_names, arg_types, n_of_args, body_node));
	/* assign the name to the function */
	NAMES_Assign(func_name, f);
	/* return an int with the location of the function */
	return OBJECT_NewInt((int)f);
}

object *INTERPRETER_VisitCall(interpreter *i, node *n) {
	/* get the function */
	object *fobj = NAMES_Get((char*)n->tokens[0]->value);
	/* function was not found */
	if (fobj == NULL) {
		/* create runtime error */
		i->e = ERROR_RuntimeError("Variable not defined", n->lineno, n->colno);
		/* return */
		return NULL;
	}
	/* unknown call type */
	if (fobj->type != OBJECT_FUNCTION && fobj->type != OBJECT_STRUCT) {
		/* create error */
		i->e = ERROR_RuntimeError("Cannot call value", n->lineno, n->colno);
		/* return */
		return NULL;
	}
	/* struct */
	else if (fobj->type == OBJECT_STRUCT) {
		/* create instance */
		object *i = OBJECT_NewInstance((structObject*)fobj->value);
		/* failed */
		if (i == NULL)
			return NULL;
		/* return instance */
		return i;
	}
	/* otherwise, function */
	function *f = (function*)fobj->value;
	/* invalid number of arguments */
	if (n->n_of_children != f->n_of_args) {
		/* create runtime error */
		i->e = ERROR_RuntimeError("Invalid number of arguments passed", n->lineno, n->colno);
		/* return */
		return NULL;
	}
	object **o2 = (object**)malloc(sizeof(object*) * 100);
	/* visit each object and assign it's name */
	for (int k = 0; k < f->n_of_args; k++) {
		/* visit an object */
		object *o = INTERPRETER_Visit(i, n->children[k]);
		/* error */
		if (o == NULL || i->e != NULL)
			return NULL;
		/* check type */
		if (o->type != f->arg_types[k]) {
			/* create runtime error */
			i->e = ERROR_RuntimeError("Mismatched argument type", n->lineno, n->colno);
			/* free object */
			if (!STORAGE_Find(o)) OBJECT_FreeObject(o);
			/* return */
			return NULL;
		}
		/* register if not registered */
		if (!STORAGE_Find(o)) o = STORAGE_Register(o);
		o2[k] = o;
		/* assign name */
		NAMES_Assign(f->arg_names[k], o);
	}
	/* execute the code inside the function */
	object *o3 = INTERPRETER_Visit(i, f->body_node);
	/* error */
	if (o3 == NULL || i->e != NULL) {
		/* free stuff */
		for (int j = 0; j < f->n_of_args; j++) {
			/* free argument */
			if (!STORAGE_Find(o2[j])) OBJECT_FreeObject(o2[j]);
		}
		/* free list */
		free(o2);
		/* return */
		return NULL;
	}
	/* free arguments */
	for (int j = 0; j < f->n_of_args; j++) {
		/* free argument */
		if (!STORAGE_Find(o2[j])) OBJECT_FreeObject(o2[j]);
	}
	/* free list */
	free(o2);
	/* return result */
	return o3;
}

object *INTERPRETER_VisitVarAssign(interpreter *i, node *n) {
	/* get the object for the value */
	object *o = INTERPRETER_Visit(i, n->children[0]);
	/* error or failed allocation */
	if (o == NULL || i->e != NULL)
		return NULL;

	/* get variable name value */
	char *var_name = (char*)n->tokens[0]->value;
	/* get type value */
	char *var_type = (char*)n->tokens[1]->value;
	/* get array related values */
	int is_array = n->b;
	int array_size = n->c;

	/* variable type */
	int _var_type = 1000; /* default */
	int _array_type = 1000; /* array type */

	/* array */
	if (is_array) {
		_var_type = OBJECT_ARRAY; /* array */
		/* int array */
		if (!strcmp(var_type, "int")) {
			_array_type = OBJECT_INT; /* int */
		}
		/* char array */
		if (!strcmp(var_type, "char")) {
			_array_type = OBJECT_CHAR; /* char */
		}
		/* struct instance array */
		if (!strcmp(var_type, "inst")) {
			_array_type = OBJECT_INSTANCE; /* struct instance */
		}
	}
	/* int */
	else if (!strcmp(var_type, "int")) {
		_var_type = OBJECT_INT; /* int */
	}
	/* str */
	else if (!strcmp(var_type, "str")) {
		_var_type = OBJECT_STRING; /* str */
	}
	/* instance */
	else if (!strcmp(var_type, "inst")) {
		_var_type = OBJECT_INSTANCE; /* instance */
	}
	/* char */
	else if (!strcmp(var_type, "char")) {
		_var_type = OBJECT_CHAR; /* char */
		/* expects string literal */
		if (o->type != OBJECT_STRING && o->type != OBJECT_INT) {
			/* create runtime error */
			i->e = ERROR_RuntimeError("Mismatched Types", n->lineno, n->colno);
			/* free an object if it isn't in storage */
			if (!STORAGE_Find(o)) OBJECT_FreeObject(o);
			/* return */
			return NULL;
		}
		if (o->type == OBJECT_STRING) {
			/* get first char from string */
			object *o2 = OBJECT_NewChar(((char*)o->value)[0]);
			/* free original object */
			if (!STORAGE_Find(o)) OBJECT_FreeObject(o);
			/* assign new object */
			o = o2;
		} else {
			/* new char from int value */
			object *o2 = OBJECT_NewChar((char)*(int*)o->value);
			/* free original object */
			if (!STORAGE_Find(o)) OBJECT_FreeObject(o);
			/* assign new object */
			o = o2;
		}
	}

	/* object type not same as var type */
	if (_var_type != o->type && !is_array) {
		/* create runtime error */
		i->e = ERROR_RuntimeError("Mismatched Types", n->lineno, n->colno);
		/* free an object if it isn't in storage */
		if (!STORAGE_Find(o)) OBJECT_FreeObject(o);
		/* return */
		return NULL;
	}
	/* handling of arrays */
	else if (is_array) {
		/* compare array type */
		if (_var_type == o->type) {
			/* mismatched types */
			if (((arrayObject*)o->value)->array_type != _array_type) {
				/* create runtime error */
				i->e = ERROR_RuntimeError("Mismatched Types", n->lineno, n->colno);
				/* free an object if it isn't in storage */
				if (!STORAGE_Find(o)) OBJECT_FreeObject(o);
				/* return */
				return NULL;
			}
		}
		/* char array string thing */
		else if (o->type == OBJECT_STRING && _array_type == OBJECT_CHAR) {
			/* create a new array object */
			object *o2 = OBJECT_NewArray(OBJECT_CHAR, array_size);
			/* loop through string */
			for (int i = 0; i < strlen((char*)o->value); i++) {
				/* break if we've reached limit of array size */
				if (i == array_size) break;
				/* add the char */
				object *c = OBJECT_NewChar(((char*)o->value)[i]);
				if (!STORAGE_Find(c)) c = STORAGE_Register(c);
				((arrayObject*)o2->value)->values[i] = c;
			}
			/* free if not registered */
			if (!STORAGE_Find(o)) OBJECT_FreeObject(o);
			o = o2; /* reassign to new array */
		}
		/* error */
		else {
			/* create runtime error */
			i->e = ERROR_RuntimeError("Mismatched Types", n->lineno, n->colno);
			/* free an object if it isn't in storage */
			if (!STORAGE_Find(o)) OBJECT_FreeObject(o);
			/* return */
			return NULL;
		}
	}

	/* if an object is not registered, register it */
	if (!STORAGE_Find(o)) o = STORAGE_Register(o);

	/* assign the name to the value */
	NAMES_Assign(var_name, o);

	/* return object */
	return o;
}

object *INTERPRETER_VisitVarAccess(interpreter *i, node *n) {
	/* get the value from the names list */
	object *o = NAMES_Get((char*)n->tokens[0]->value);

	/* undefined object */
	if (o == NULL) {
		/* create runtime error */
		i->e = ERROR_RuntimeError("Variable not defined", n->lineno, n->colno);
		return NULL; /* exit */
	}

	/* return object */
	return o;
}

object *INTERPRETER_VisitUnOp(interpreter *i, node *n) {
	/* visit the right value */
	object *right = INTERPRETER_Visit(i, n->children[0]);
	/* error from right */
	if (i->e != NULL || right == NULL) {
		/* free right if it isn't in storage */
		if (!STORAGE_Find(right)) OBJECT_FreeObject(right);
		return NULL; /* exit */
	}

	object *result = NULL; /* final result */
	if (n->tokens[0]->type == TOKEN_MINUS) {
		/* negate it */
		object *x = OBJECT_NewInt(-1);
		result = OBJECT_MultedBy(right, x);
		OBJECT_FreeObject(x);
	}

	/* free stuff */
	if (!STORAGE_Find(right)) OBJECT_FreeObject(right);

	/* return result */
	return result;
}

object *INTERPRETER_VisitAddress(interpreter *i, node *n) {
	/* visit the child node */
	object *chd = INTERPRETER_Visit(i, n->children[0]);
	/* store address of object */
	int adr = (int)chd; /* address of object* rather than object for various reasons */
	/* free if needed */
	if (!STORAGE_Find(chd)) OBJECT_FreeObject(chd);
	/* return new int */
	return OBJECT_NewInt(adr);
}

object *INTERPRETER_VisitGetItem(interpreter *i, node *n) {
	/* visit the child node */
	object *chd = INTERPRETER_Visit(i, n->children[0]);
	/* get the value */
	object *value = NAMES_Get((char*)n->tokens[0]->value);
	/* error from child */
	if (i->e != NULL || chd == NULL) {
		/* free child if it isn't null and isn't in storage */
		if (chd != NULL && !STORAGE_Find(chd))
			/* free pointer */
			OBJECT_FreeObject(chd);
		return NULL; /* exit */
	}
	/* unknown value from value */
	if (value == NULL) {
		/* create error */
		i->e = ERROR_RuntimeError("Variable not defined", n->lineno, n->colno);
		/* free child */
		if (!STORAGE_Find(chd)) OBJECT_FreeObject(chd);
		return NULL; /* exit */
	}
	/* array */
	if (value->type == OBJECT_ARRAY) {
		/* int value invalid */
		if (chd->type != OBJECT_INT) {
			/* create error */
			i->e = ERROR_RuntimeError("Index must be Integer", n->lineno, n->colno);
			/* free value and child */
			if (!STORAGE_Find(chd)) OBJECT_FreeObject(chd);
			if (!STORAGE_Find(value)) OBJECT_FreeObject(value);
			return NULL; /* exit */
		}
		/* store index */
		int idx = *(int*)chd->value;
		/* get array */
		arrayObject *a = (arrayObject*)value->value;
		/* invalid index */
		if (idx >= a->size) {
			/* create error */
			i->e = ERROR_RuntimeError("Index greater than limit of array", n->lineno, n->colno);
			/* free value and child */
			if (!STORAGE_Find(chd)) OBJECT_FreeObject(chd);
			if (!STORAGE_Find(value)) {
				OBJECT_FreeObject(value);
			}
			return NULL; /* exit */
		}
		/* free child object */
		if (!STORAGE_Find(chd)) OBJECT_FreeObject(chd);
		/* get object */
		return a->values[idx];
	}
	/* string */
	if (value->type == OBJECT_STRING) {
		/* int value invalid */
		if (chd->type != OBJECT_INT) {
			/* create error */
			i->e = ERROR_RuntimeError("Index must be Integer", n->lineno, n->colno);
			/* free value and child */
			if (!STORAGE_Find(chd)) OBJECT_FreeObject(chd);
			if (!STORAGE_Find(value)) OBJECT_FreeObject(value);
			return NULL; /* exit */
		}
		/* store index */
		int idx = *(int*)chd->value;
		/* free child object */
		if (!STORAGE_Find(chd)) OBJECT_FreeObject(chd);
		/* return new char */
		return OBJECT_NewChar(((char*)value->value)[idx]);
	}
	/* instance */
	if (value->type == OBJECT_INSTANCE) {
		/* expecting string */
		if (chd->type != OBJECT_STRING) {
			/* create error */
			i->e = ERROR_RuntimeError("Index must be String", n->lineno, n->colno);
			/* free value and child */
			if (!STORAGE_Find(chd)) OBJECT_FreeObject(chd);
			if (!STORAGE_Find(value)) OBJECT_FreeObject(value);
			return NULL; /* exit */
		}
		/* get string value */
		char *key = (char*)chd->value;
		/* get instance */
		instance *inst = (instance*)value->value;
		/* search through names and find correct one */
		int j;
		for (j = 0; j < inst->st->n_of_vals; j++) {
			/* if they are the same */
			if (!strcmp(key, inst->st->val_names[j])) {
				break; /* break */
			}
		}
		/* unknown member */
		if (j == inst->st->n_of_vals) {
			/* create error */
			i->e = ERROR_RuntimeError("Unknown member name", n->lineno, n->colno);
			/* free value and child */
			if (!STORAGE_Find(chd)) OBJECT_FreeObject(chd);
			if (!STORAGE_Find(value)) OBJECT_FreeObject(value);
			return NULL; /* exit */
		}
		/* free child object */
		if (!STORAGE_Find(chd)) OBJECT_FreeObject(chd);
		/* return value at position */
		return inst->values[j];
	}
	return NULL;
}

object *INTERPRETER_VisitSetItem(interpreter *i, node *n) {
	/* get child node */
	object *chd = INTERPRETER_Visit(i, n->children[0]);
	/* fail */
	if (chd == NULL || i->e != NULL) {
		/* free */
		if (chd != NULL && !STORAGE_Find(chd)) OBJECT_FreeObject(chd);
		/* return */
		return NULL;
	}
	/* get value node */
	object *new_value = INTERPRETER_Visit(i, n->children[1]);
	/* fail */
	if (new_value == NULL || i->e != NULL) {
		/* free */
		if (new_value != NULL && !STORAGE_Find(new_value)) OBJECT_FreeObject(new_value);
		/* return */
		return NULL;
	}
	/* get index */
	object *value = NAMES_Get((char*)n->tokens[0]->value);
	/* value not found */
	if (value == NULL) {
		/* create error */
		i->e = ERROR_RuntimeError("Variable not defined", n->lineno, n->colno);
		/* free */
		if (!STORAGE_Find(chd)) OBJECT_FreeObject(chd);
		if (!STORAGE_Find(new_value)) OBJECT_FreeObject(new_value);
		/* return */
		return NULL;
	}
	/* array */
	if (value->type == OBJECT_ARRAY) {
		/* int value invalid */
		if (chd->type != OBJECT_INT) {
			/* create error */
			i->e = ERROR_RuntimeError("Index must be Integer", n->lineno, n->colno);
			/* free value and child */
			if (!STORAGE_Find(chd)) OBJECT_FreeObject(chd);
			if (!STORAGE_Find(new_value)) OBJECT_FreeObject(new_value);
			if (!STORAGE_Find(value)) OBJECT_FreeObject(value);
			return NULL; /* exit */
		}
		/* store index */
		int idx = *(int*)chd->value;
		/* get array */
		arrayObject *a = (arrayObject*)value->value;
		/* invalid index */
		if (idx >= a->size) {
			/* create error */
			i->e = ERROR_RuntimeError("Index greater than limit of array", n->lineno, n->colno);
			/* free value and child */
			if (!STORAGE_Find(chd)) OBJECT_FreeObject(chd);
			if (!STORAGE_Find(new_value)) OBJECT_FreeObject(new_value);
			if (!STORAGE_Find(value)) OBJECT_FreeObject(value);
			return NULL; /* exit */
		}
		if (!STORAGE_Find(new_value)) a->values[idx] = STORAGE_Register(new_value);
		else a->values[idx] = new_value;
		/* free child object */
		if (!STORAGE_Find(chd)) OBJECT_FreeObject(chd);
		/* get object */
		return a->values[idx];
	}
	/* string */
	if (value->type == OBJECT_STRING) {
		/* int value invalid */
		if (chd->type != OBJECT_INT) {
			/* create error */
			i->e = ERROR_RuntimeError("Index must be Integer", n->lineno, n->colno);
			/* free value and child */
			if (!STORAGE_Find(chd)) OBJECT_FreeObject(chd);
			if (!STORAGE_Find(new_value)) OBJECT_FreeObject(new_value);
			if (!STORAGE_Find(value)) OBJECT_FreeObject(value);
			return NULL; /* exit */
		}
		/* store index */
		int idx = *(int*)chd->value;
		/* if string get first char */
		if (new_value->type == OBJECT_STRING) {
			/* set char */
			((char*)value->value)[idx] = ((char*)new_value->value)[0];
		}
		/* char object */
		else if (new_value->type == OBJECT_CHAR) {
			/* set char */
			((char*)value->value)[idx] = (*(char*)new_value->value);
		}
		/* free stuff */
		if (!STORAGE_Find(chd)) OBJECT_FreeObject(chd);
		if (!STORAGE_Find(new_value)) OBJECT_FreeObject(new_value);
		/* create char object */
		object *chr = OBJECT_NewChar(((char*)value->value)[idx]);
		/* free child object */
		if (!STORAGE_Find(chd)) OBJECT_FreeObject(chd);
		/* return new char */
		return chr;
	}
	/* instance */
	if (value->type == OBJECT_INSTANCE) {
		/* requires string */
		if (chd->type != OBJECT_STRING) {
			/* create error */
			i->e = ERROR_RuntimeError("Index must be String", n->lineno, n->colno);
			/* free value and child */
			if (!STORAGE_Find(chd)) OBJECT_FreeObject(chd);
			if (!STORAGE_Find(new_value)) OBJECT_FreeObject(new_value);
			if (!STORAGE_Find(value)) OBJECT_FreeObject(value);
			return NULL; /* exit */
		}
		/* get string value */
		char *key = (char*)chd->value;
		/* get instance */
		instance *inst = (instance*)value->value;
		/* search for name */
		int j;
		for (j = 0; j < inst->st->n_of_vals; j++) {
			/* if they are the same */
			if (!strcmp(key, inst->st->val_names[j]))
				break; /* break */
		}
		/* unknown member */
		if (j == inst->st->n_of_vals) {
			/* create error */
			i->e = ERROR_RuntimeError("Unknown member name", n->lineno, n->colno);
			/* free value and child */
			if (!STORAGE_Find(chd)) OBJECT_FreeObject(chd);
			if (!STORAGE_Find(new_value)) OBJECT_FreeObject(new_value);
			if (!STORAGE_Find(value)) OBJECT_FreeObject(value);
			return NULL; /* exit */
		}
		/* set value at position */
		if (!STORAGE_Find(new_value)) new_value = STORAGE_Register(new_value);
		inst->values[j] = new_value;
		/* free child object */
		if (!STORAGE_Find(chd)) OBJECT_FreeObject(chd);
		/* return */
		return new_value;
	}
	return NULL;
}

object *INTERPRETER_VisitIfNode(interpreter *i, node *n) {
	/* get the comparison */
	object *comp = INTERPRETER_Visit(i, n->children[0]);
	/* error */
	if (i->e != NULL || comp == NULL)
		return NULL;
	/* if the comparison is true */
	object *is_true = OBJECT_IsTrue(comp);
	if (*(int*)is_true->value == 1) {
		/* get statements */
		object *statements = INTERPRETER_Visit(i, n->children[1]);
		/* error */
		if (i->e != NULL || statements == NULL) {
			/* free stuff */
			if (!STORAGE_Find(comp)) OBJECT_FreeObject(comp);
			OBJECT_FreeObject(is_true);
			return NULL; /* exit */
		}
		/* free statements */
		if (!STORAGE_Find(statements)) OBJECT_FreeObject(statements);
	}
	/* free comparison */
	if (!STORAGE_Find(comp)) OBJECT_FreeObject(comp);
	OBJECT_FreeObject(is_true);
	/* new int */
	return OBJECT_NewInt(1);
}

object *INTERPRETER_VisitWhile(interpreter *i, node *n) {
	/* visit comparison */
	object *comp = INTERPRETER_Visit(i, n->children[0]);
	/* error or failed allocation */
	if (comp == NULL || i->e != NULL)
		return NULL; /* exit */
	/* get truth value */
	object *is_true = OBJECT_IsTrue(comp);
	/* loop */
	while (*(int*)is_true->value == 1) {
		/* get statements */
		object *statements = INTERPRETER_Visit(i, n->children[1]);
		/* error */
		if (i->e != NULL || statements == NULL) {
			/* free stuff */
			if (!STORAGE_Find(comp)) OBJECT_FreeObject(comp);
			OBJECT_FreeObject(is_true);
			return NULL; /* exit */
		}
		/* free comparison, is_true, and statements */
		if (!STORAGE_Find(comp)) OBJECT_FreeObject(comp);
		OBJECT_FreeObject(is_true);
		if (!STORAGE_Find(statements)) OBJECT_FreeObject(statements);
		/* get next comparison */
		comp = INTERPRETER_Visit(i, n->children[0]);
		/* error or failed allocation */
		if (comp == NULL || i->e != NULL) {
			/* free stuff */
			if (!STORAGE_Find(statements)) OBJECT_FreeObject(statements);
			OBJECT_FreeObject(is_true);
			return NULL; /* exit */
		}
		/* get truth value */
		is_true = OBJECT_IsTrue(comp);
	}
	/* free stuff */
	if (!STORAGE_Find(comp)) OBJECT_FreeObject(comp);
	if (!STORAGE_Find(is_true)) OBJECT_FreeObject(is_true);
	/* return new int */
	return OBJECT_NewInt(1);
}

object *INTERPRETER_VisitForLoop(interpreter *i, node *n) {
	/* get start end tokens */
	object *so = INTERPRETER_Visit(i, n->children[1]);
	object *eo = INTERPRETER_Visit(i, n->children[2]);
	/* found error */
	if (so == NULL || eo == NULL || i->e != NULL) {
		/* free stuff */
		if (so != NULL && !STORAGE_Find(so)) OBJECT_FreeObject(so);
		if (eo != NULL && !STORAGE_Find(eo)) OBJECT_FreeObject(eo);
		return NULL; /* exit */
	}
	/* must be integers */
	if (so->type != OBJECT_INT || eo->type != OBJECT_INT) {
		/* create error */
		i->e = ERROR_RuntimeError("Start and end values must be integers", n->children[1]->lineno, n->children[1]->colno);
		/* free */
		if (!STORAGE_Find(so)) OBJECT_FreeObject(so);
		if (!STORAGE_Find(eo)) OBJECT_FreeObject(eo);
		return NULL; /* exit */
	}
	/* store ints */
	int start = *(int*)so->value;
	int end = *(int*)eo->value;
	/* free objects */
	if (!STORAGE_Find(so)) OBJECT_FreeObject(so);
	if (!STORAGE_Find(eo)) OBJECT_FreeObject(eo);
	/* create an object */
	object *o = STORAGE_Register(OBJECT_NewInt(start));
	/* assign object to name */
	NAMES_Assign((char*)n->tokens[0]->value, o);
	/* while the start is less than end */
	while (start < end) {
		*(int*)o->value = start++; /* increment */
		/* visit the statements */
		object *st = INTERPRETER_Visit(i, n->children[0]);
		/* error or failed allocation */
		if (st == NULL || i->e != NULL) {
			/* exit */
			return NULL;
		}
		/* free object */
		if (!STORAGE_Find(st)) OBJECT_FreeObject(st);
	}
	/* return */
	return o;
}

object *INTERPRETER_VisitValue(interpreter *i, node *n) {
	/* get the value */
	object *value = INTERPRETER_Visit(i, n->children[0]);
	/* error or memory failure */
	if (value == NULL || i->e != NULL) {
		/* free */
		if (value != NULL && !STORAGE_Find(value)) OBJECT_FreeObject(value);
		return NULL; /* exit */
	}
	/* expecting integer */
	if (value->type != OBJECT_INT) {
		/* create error */
		i->e = ERROR_RuntimeError("Pointers can only exist as integers", n->lineno, n->colno);
		/* free */
		if (!STORAGE_Find(value)) OBJECT_FreeObject(value);
		return NULL; /* exit */
	}
	/* get value from object and free it if needed */
	object *adr = (object*)(*(int*)value->value);
	if (!STORAGE_Find(value)) OBJECT_FreeObject(value);
	/* return */
	return adr;
}

object *INTERPRETER_VisitBinOp(interpreter *i, node *n) {
	/* get the left and right values */
	object *left = INTERPRETER_Visit(i, n->children[0]);
	/* error from left */
	if (i->e != NULL || left == NULL) {
		/* free left if it isn't in storage */
		if (left != NULL && !STORAGE_Find(left)) {
			/* free pointer */
			OBJECT_FreeObject(left);
		}
		/* return */
		return NULL;
	}

	object *right = INTERPRETER_Visit(i, n->children[1]);
	/* error from right */
	if (i->e != NULL || right == NULL) {
		/* free right if it isn't in storage */
		if (right != NULL && !STORAGE_Find(right)) {
			/* free pointer */
			OBJECT_FreeObject(right);
		}
		/* free left */
		if (!STORAGE_Find(left)) OBJECT_FreeObject(left);
		/* return */
		return NULL;
	}

	/* get operation token */
	token *t = n->tokens[0];

	/* default result */
	object *r = NULL;

	/* '+' */
	if (t->type == TOKEN_PLUS)
		/* get result */
		r = OBJECT_AddedTo(left, right);
	/* '-' */
	else if (t->type == TOKEN_MINUS)
		/* get result */
		r = OBJECT_SubbedBy(left, right);
	/* '*' */
	else if (t->type == TOKEN_MUL)
		/* get result */
		r = OBJECT_MultedBy(left, right);
	/* '/' */
	else if (t->type == TOKEN_DIV)
		/* get result */
		r = OBJECT_DivedBy(left, right);
	/* '%' */
	else if (t->type == TOKEN_MOD)
		/* get result */
		r = OBJECT_ModdedBy(left, right);
	/* '==' */
	else if (t->type == TOKEN_EE)
		/* get result */
		r = OBJECT_IsEqualTo(left, right);
	/* '!=' */
	else if (t->type == TOKEN_NE)
		/* get result */
		r = OBJECT_IsNotEqualTo(left, right);
	/* '<' */
	else if (t->type == TOKEN_LT)
		/* get result */
		r = OBJECT_IsLessThan(left, right);
	/* '>' */
	else if (t->type == TOKEN_GT)
		/* get result */
		r = OBJECT_IsGreaterThan(left, right);

	/* result not found */
	if (r == NULL) {
		/* new error */
		i->e = ERROR_RuntimeError("Illegal Operation", n->lineno, n->colno);
	}

	/* free left and right if they aren't registered */
	if (!STORAGE_Find(left))
		/* free left */
		OBJECT_FreeObject(left);
	if (!STORAGE_Find(right))
		/* free right */
		OBJECT_FreeObject(right);

	/* return result */
	return r;
}

#ifdef __cplusplus /* c++ check */
}
#endif