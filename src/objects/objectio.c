/* IO functions for objects. */
#include "object.h" /* object stuff */
#include "objectio.h" /* our header file */
#include "os.h" /* operating system information */

#include <stdio.h> /* printf, gets */
#include <stdlib.h> /* free */

#ifdef __cplusplus /* c++ stdio check */
#include "stdio.h" /* printf, gets */
#endif

#ifdef __cplusplus /* c++ check */
extern "C" {
#endif

object *OBJECTIO_PrintObject(object *o) {
	/* integer */
	if (o->type == OBJECT_INT)
		/* print integer */
		printf("%d\n", *(int*)o->value);
	/* float */
	if (o->type == OBJECT_FLOAT)
		/* print float */
		printf("%f\n", *(float*)o->value);
	/* char */
	if (o->type == OBJECT_CHAR)
		/* print char */
		printf("%c\n", *(char*)o->value);
	/* string */
	if (o->type == OBJECT_STRING)
		/* print string */
		printf("%s\n", (char*)o->value);
	/* function */
	if (o->type == OBJECT_FUNCTION) {
		/* print function */
		printf("<function \'%s\'>\n", ((function*)o->value)->func_name);
	}
	/* array */
	if (o->type == OBJECT_ARRAY)
		/* string (aka char array) */
		if (((arrayObject*)o->value)->array_type == OBJECT_CHAR) { /* char array / string */
			/* loop through chars */
			for (int i = 0; i < OBJECT_StringLength((arrayObject*)o->value); i++)
				/* print the char */
				printf("%c", (char)*(int*)((arrayObject*)o->value)->values[i]->value);
			/* print newline */
			printf("\n");
		}
	/* struct */
	if (o->type == OBJECT_STRUCT)
		/* struct */
		printf("<struct \'%s\'>\n", ((structObject*)o->value)->struct_name);
	/* instance */
	if (o->type == OBJECT_INSTANCE)
		/* instance */
		printf("<instance of struct \'%s\'>\n", ((instance*)o->value)->st->struct_name);

	/* return the object */
	return o;
}

object *OBJECTIO_Input() {
	/* new string buffer */
	char *s = (char*)malloc(100);
	/* read the string into the buffer */
	#if defined(LINUX)
	fgets(s, 100, stdin); /* gcc on linux warns about 'gets' function */
	#else
	s = gets(s); /* windows doesn't */
	#endif
	/* failure */
	if (s == NULL)
		return NULL;
	/* create an object */
	object *o = OBJECT_NewString(s);

	if (!o || o == NULL) { /* bad allocation */
		free(s);
		return NULL;
	}

	/* return our object */
	return o;
}

#ifdef __cplusplus /* c++ check */
}
#endif