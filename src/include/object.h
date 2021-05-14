/* object library for objects that can be manipulated at runtime. */
/* "interpreter's equivelant to nodes and tokens" */

#include "node.h" /* for functions */
#include <stdint.h> /* uint8_t type */

#ifndef OBJECT_H
#define OBJECT_H

#ifdef __cplusplus /* c++ check */
extern "C" {
#endif

/* types of objects */
#define OBJECT_INT			0
#define OBJECT_CHAR			1
#define OBJECT_FLOAT		2
#define OBJECT_STRING		3
#define OBJECT_ARRAY		4
#define OBJECT_FUNCTION		5
#define OBJECT_STRUCT		6
#define OBJECT_INSTANCE		7

/* base object type */
typedef struct _ADAMITE_Lib_Object { /* base object type for variables */
	uint8_t type; /* type of object */
	void *value; /* pointer to the value */
} object; /* final name */
/* object array type */
typedef struct _ADAMITE_Lib_ArrayObject {
	uint8_t array_type; /* the type of the array */
	object **values; /* the values of the array */
	int size; /* the size of the array */
} arrayObject; /* will be held in 'value' of object* */
/* function type */
typedef struct _ADAMITE_Lib_FunctionObject {
	uint8_t ret_type; /* the return type of the function */
	char **arg_names; /* the function argument names */
	uint8_t *arg_types; /* the function argument types */
	node *body_node; /* function body */
	char *func_name; /* function name */
	int n_of_args; /* number of function arguments */
} function;
/* struct object */
typedef struct _ADAMITE_Lib_StructObject {
	char *struct_name; /* name of struct */
	uint8_t *val_types; /* types of the values */
	char **val_names; /* names of the values */
	int n_of_vals; /* number of values */
} structObject;
/* instance */
typedef struct _ADAMITE_Lib_Instance {
	structObject *st; /* actual struct */
	object **values; /* actual struct values */
} instance;

object *OBJECT_NewObject(int type); /* instantiate a new object */
object *OBJECT_NewArray(int type, int size); /* new array */
object *OBJECT_NewFunction(char *func_name, int ret_type, char **arg_names, uint8_t *arg_types, int n_of_args, node *body_node); /* new function */
object *OBJECT_AddedTo(object *self, object *other); /* add the value of an object to another object */
object *OBJECT_SubbedBy(object *self, object *other); /* subtract */
object *OBJECT_MultedBy(object *self, object *other); /* multiply */
object *OBJECT_DivedBy(object *self, object *other); /* divide (not by zero) */
object *OBJECT_ModdedBy(object *self, object *other); /* modulo (get remainder of division) */
object *OBJECT_IsEqualTo(object *self, object *other); /* == */
object *OBJECT_IsNotEqualTo(object *self, object *other); /* != */
object *OBJECT_IsGreaterThan(object *self, object *other); /* > */
object *OBJECT_IsLessThan(object *self, object *other); /* < */
object *OBJECT_IsGreaterThanOrEqualTo(object *self, object *other); /* >= */
object *OBJECT_IsLessThanOrEqualTo(object *self, object *other); /* <= */
object *OBJECT_IsTrue(object *self); /* object's truth value */
object *OBJECT_PowedBy(object *self, object *other); /* exponent */
object *OBJECT_NewString(const char *s); /* new string */
object *OBJECT_NewInt(int i); /* new integer */
object *OBJECT_NewChar(char c); /* new char */
object *OBJECT_NewFloat(float f); /* new float */
object *OBJECT_NewPtr(int addr); /* new pointer to a variable */
object *OBJECT_NewStruct(char *name, uint8_t *val_types, char **val_names, int n_of_vals); /* create a new struct */
object *OBJECT_NewInstance(structObject *st); /* create an instance */
void OBJECT_FreeObject(object *o); /* free an object's memory */
void OBJECT_FreeArray(arrayObject *o); /* free an array object's memory */
void OBJECT_FreeStruct(structObject *o); /* free a struct */
void OBJECT_FreeInstance(instance *o); /* free a struct instance */
int OBJECT_StringLength(arrayObject *o); /* find the length of a char array */

#ifdef __cplusplus /* c++ check */
}
#endif

#endif /* OBJECT_H */