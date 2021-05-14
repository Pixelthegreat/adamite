/* read object.h in /include/ */
#include "memory.h" /* memory management stuff */
#include "object.h" /* our header */
#include "storage.h" /* STORAGE_Find for not freeing wrong items */

#include <stdio.h> /* debugging */
#include <string.h> /* strcpy */

#ifdef __cplusplus /* c++ check */
extern "C" {
#endif

object *OBJECT_NewObject(int type) {
	/* create a new object */
	object *obj = MEMORY_Malloc(object); /* allocate an object */
	if (!obj || obj == NULL) /* memory allocation wasn't successful */
		return NULL;

	/* otherwise */
	obj->type = type;
	obj->value = NULL;
	return obj;
}

object *OBJECT_NewFunction(char *func_name, int ret_type, char **arg_names, uint8_t *arg_types, int n_of_args, node *body_node) {
	/* create a new function object */
	function *f = MEMORY_Malloc(function); /* the actual function object */
	if (!f || f == NULL) /* unsuccessful allocation */
		return NULL;
	/* assign values */
	f->func_name = func_name;
	f->ret_type = ret_type;
	f->arg_names = arg_names;
	f->arg_types = arg_types;
	f->body_node = body_node;
	f->n_of_args = n_of_args;
	/* create a regular object */
	object *obj = OBJECT_NewObject(OBJECT_FUNCTION);
	/* failed allocation */
	if (obj == NULL)
		return NULL;
	/* assign function */
	obj->value = (void*)f;
	return obj; /* return our object */
}

object *OBJECT_NewArray(int type, int size) {
	/* create new object */
	object *obj = OBJECT_NewObject(OBJECT_ARRAY); /* allocate an object */
	if (!obj || obj == NULL) /* memory allocation wasn't successful */
		return NULL;

	/* otherwise */
	obj->value = (void*)MEMORY_Malloc(arrayObject); /* allocate the array */
	if (!((arrayObject*)obj->value) || ((arrayObject*)obj->value) == NULL) {
		free(obj); /* free the object */
		return NULL;
	}

	/* otherwise */
	((arrayObject*)obj->value)->values = (object**)malloc(sizeof(object*)*size); /* allocate the *actual* array */
	if (!((arrayObject*)obj->value)->values || ((arrayObject*)obj->value)->values == NULL) {
		free(obj->value); /* free the arrayObject */
		free(obj); /* free the object */
		return NULL;
	}

	/* set the array type */
	((arrayObject*)obj->value)->array_type = type;

	/* loop through the array and fill in the gaps */
	for (int i = 0; i < size; i++) {
		/* check for int */
		if (type == OBJECT_INT)
			/* add int */
			((arrayObject*)obj->value)->values[i] = STORAGE_Register(OBJECT_NewInt(0)); /* default value */
		/* check for string */
		else if (type == OBJECT_STRING)
			/* add string */
			((arrayObject*)obj->value)->values[i] = STORAGE_Register(OBJECT_NewString("")); /* default value */
		/* otherwise */
		else
			/* add int */
			((arrayObject*)obj->value)->values[i] = STORAGE_Register(OBJECT_NewInt(0)); /* default value */
	}
	/* assign array size */
	((arrayObject*)obj->value)->size = size;
	/* lastly return the value */
	return obj;
}

object *OBJECT_NewString(const char *s) {
	/* create a string object */
	object *obj = OBJECT_NewObject(OBJECT_STRING);

	if (!obj || obj == NULL)
		return NULL; /* unsuccessful allocation */

	/* copy string */
	char *sb = (char*)malloc(strlen(s)+1);
	strcpy(sb, s);
	s = sb; /* 's' will be freed eventually */
	obj->value = (void*)s; /* assign the value of the string */
	return obj;
}

object *OBJECT_NewInt(int i) {
	/* create an int object */
	object *obj = OBJECT_NewObject(OBJECT_INT);

	if (!obj || obj == NULL)
		return NULL; /* failed allocation */

	/* int is most likely to be stack allocated, so we create a new variable with value of int instead */
	int *val = MEMORY_Malloc(int);
	if (!val || val == NULL) /* failed allocation */
		return NULL;

	*val = i;

	obj->value = (void*)val; /* assign the value of the int */
	return obj;
}

object *OBJECT_NewChar(char c) {
	/* create an int object */
	object *obj = OBJECT_NewInt((int)c);

	if (!obj || obj == NULL)
		return NULL; /* failed allocation */

	obj->type = OBJECT_CHAR; /* indicate char */
	return obj; /* return the object */
}

void OBJECT_FreeObject(object *o) {
	if (o->type != OBJECT_ARRAY && o->type != OBJECT_FUNCTION) {
		MEMORY_Free(o->value); /* frees value */
	}
	else if (o->type == OBJECT_ARRAY) { /* array */
		OBJECT_FreeArray((arrayObject*)o->value); /* free the values of the array */
		MEMORY_Free(o->value); /* free the actual array */
	}
	else if (o->type == OBJECT_STRUCT) { /* struct */
		OBJECT_FreeStruct((structObject*)o->value);
		MEMORY_Free(o->value); /* free the actual struct */
	}
	else if (o->type == OBJECT_FUNCTION) { /* function */
		function *f = (function*)o->value; /* the normal function pointer */
		for (int i = 0; i < f->n_of_args; i++) {
			/* free argument */
			free(f->arg_names[i]);
		}
		/* free lists */
		free(f->arg_names);
		free(f->arg_types);
		free(f->func_name);
		/* free body node (because it has been copied from parser won't be freed automatically) */
		NODE_FreeCopyNode(f->body_node);
	}
	MEMORY_Free(o); /* free the object from memory */
}

void OBJECT_FreeArray(arrayObject *o) {
	/* loop through values and free them */
	for (int i = 0; i < o->size; i++)
		if (!STORAGE_Find(o->values[i])) OBJECT_FreeObject(o->values[i]); /* free an object */
	MEMORY_Free(o->values); /* free the actual array */
}

void OBJECT_FreeStruct(structObject *o) {
	/* loop through names and free them */
	for (int i = 0; i < o->n_of_vals; i++)
		MEMORY_Free(o->val_names[i]); /* free name */
	/* free arrays */
	MEMORY_Free(o->val_types);
	MEMORY_Free(o->val_names);
	MEMORY_Free(o->struct_name);
}

int OBJECT_StringLength(arrayObject *o) {
	/* loop until we reach the null term char */
	int i = 0;
	while ((char)*(int*)(o->values[i]) != '\0') {
		/* advance i */
		i++;
	}
	return i; /* length of string */
}

object *OBJECT_AddedTo(object *self, object *other) {
	/* check for int */
	if (self->type == OBJECT_INT) {
		/* illegal operation */
		if (other->type != OBJECT_INT)
			/* return */
			return NULL;
		/* create new object */
		object *o = OBJECT_NewInt(*(int*)(self->value) + *(int*)(other->value));
		/* return object */
		return o;
	}
	/* check for string */
	else if (self->type == OBJECT_STRING) {
		/* illegal operation */
		if (other->type != OBJECT_STRING)
			/* return */
			return NULL;
		/* create new string buffer */
		char *s = (char*)malloc(1024);
		/* copy first string */
		strcpy(s, (char*)self->value);
		/* concat second string */
		strcat(s, (char*)other->value);
		/* create new object */
		object *o = OBJECT_NewString(s);
		/* failed allocation */
		if (o == NULL) {
			/* free string value */
			free(s);
			/* return null */
			return NULL;
		}
		/* return object */
		return o;
	}
	/* return null */
	return NULL;
}

object *OBJECT_SubbedBy(object *self, object *other) {
	/* check for int */
	if (self->type == OBJECT_INT) {
		/* illegal operation */
		if (other->type != OBJECT_INT)
			/* return null */
			return NULL;
		/* create new object */
		object *o = OBJECT_NewInt(*(int*)(self->value) - *(int*)(other->value));
		/* return object */
		return o;
	}
	/* return null */
	return NULL;
}

object *OBJECT_MultedBy(object *self, object *other) {
	/* check for int */
	if (self->type == OBJECT_INT) {
		/* illegal operation */
		if (other->type != OBJECT_INT)
			/* return null */
			return NULL;
		/* create new object */
		object *o = OBJECT_NewInt(*(int*)(self->value) * *(int*)(other->value));
		return o; /* return object */
	}

	/* return null */
	return NULL;
}

object *OBJECT_DivedBy(object *self, object *other) {
	/* check for int */
	if (self->type == OBJECT_INT) {
		/* illegal operation */
		if (other->type != OBJECT_INT)
			/* return null */
			return NULL;
		/* create new object */
		object *o = OBJECT_NewInt(*(int*)(self->value) / *(int*)(other->value));
		return o; /* return object */
	}

	/* return null */
	return NULL;
}

object *OBJECT_ModdedBy(object *self, object *other) {
	/* check for int */
	if (self->type == OBJECT_INT) {
		/* illegal operation */
		if (other->type != OBJECT_INT)
			return NULL; /* exit */
		/* return object */
		return OBJECT_NewInt(*(int*)(self->value) % *(int*)(other->value));
	}
	return NULL;
}

object *OBJECT_IsEqualTo(object *self, object *other) {
	/* check for int */
	if (self->type == OBJECT_INT) {
		/* illegal operation */
		if (other->type != OBJECT_INT)
			/* return null */
			return NULL;
		/* create new object */
		return OBJECT_NewInt((int)(*(int*)(self->value) == *(int*)(other->value)));
	}
	/* check for char */
	if (self->type == OBJECT_CHAR) {
		/* illegal operation */
		if (other->type != OBJECT_CHAR)
			/* return null */
			return NULL;
		/* create new object */
		return OBJECT_NewInt((int)(*(char*)(self->value) == *(char*)(other->value)));
	}
	/* check for string */
	if (self->type == OBJECT_STRING) {
		/* illegal operation */
		if (other->type != OBJECT_STRING)
			/* return null */
			return NULL;
		/* create new object */
		return OBJECT_NewInt((int)(!strcmp((char*)self->value, (char*)other->value)));
	}
	/* return null */
	return NULL;
}

object *OBJECT_IsNotEqualTo(object *self, object *other) {
	/* check for int */
	if (self->type == OBJECT_INT) {
		/* illegal operation */
		if (other->type != OBJECT_INT)
			/* return null */
			return NULL;
		/* create new object */
		return OBJECT_NewInt((int)(*(int*)(self->value) != *(int*)(other->value)));
	}
	/* check for string */
	if (self->type == OBJECT_STRING) {
		/* illegal operation */
		if (other->type != OBJECT_STRING)
			/* return null */
			return NULL;
		/* create new object */
		return OBJECT_NewInt((int)(!!strcmp((char*)self->value, (char*)other->value)));
	}
	/* return null */
	return NULL;
}

object *OBJECT_IsLessThan(object *self, object *other) {
	/* unimplemented */
	return NULL;
}

object *OBJECT_IsGreaterThan(object *self, object *other) {
	/* unimplemented */
	return NULL;
}

object *OBJECT_IsTrue(object *self) {
	/* integer */
	if (self->type == OBJECT_INT) {
		/* != 0 */
		return OBJECT_NewInt((int)(*(int*)self->value != 0));
	}
	/* string */
	if (self->type == OBJECT_STRING) {
		/* != "" */
		return OBJECT_NewInt((int)!strcmp((char*)self->value, ""));
	}
	/* char */
	if (self->type == OBJECT_CHAR) {
		/* != 0 */
		return OBJECT_NewInt((int)(*(char*)self->value != (char)0));
	}
	/* default value */
	return OBJECT_NewInt(1);
}

object *OBJECT_NewStruct(char *name, uint8_t *val_types, char **val_names, int n_of_vals) {
	/* allocate new struct */
	structObject *st = MEMORY_Malloc(structObject);
	/* bad memory */
	if (st == NULL)
		return NULL;
	/* assign values */
	st->val_types = val_types;
	st->val_names = val_names;
	st->n_of_vals = n_of_vals;
	st->struct_name = name;
	/* allocate new object */
	object *o = OBJECT_NewObject(OBJECT_STRUCT);
	/* bad memory allocation */
	if (o == NULL) {
		/* free */
		MEMORY_Free(st);
		return NULL; /* quit */
	}
	/* assign value */
	o->value = (void*)st;
	/* return */
	return o;
}

object *OBJECT_NewInstance(structObject *st) {
	/* allocate new instance */
	instance *i = MEMORY_Malloc(instance);
	/* bad memory */
	if (i == NULL)
		return NULL;
	/* assign values */
	i->st = st;
	/* allocate new object */
	object *o = OBJECT_NewObject(OBJECT_INSTANCE);
	/* bad memory */
	if (o == NULL) {
		MEMORY_Free(i); /* free */
		return NULL;
	}
	o->value = (void*)i;
	/* allocate value list */
	i->values = (object**)malloc(sizeof(object*) * st->n_of_vals);
	/* bad memory */
	if (i->values == NULL) {
		/* free */
		MEMORY_Free(i);
		MEMORY_Free(o);
		return NULL;
	}
	/* return object */
	return o;
}

#ifdef __cplusplus /* c++ check */
}
#endif