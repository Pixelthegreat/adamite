/* see names.h for documentation */
#include "names.h" /* our header */
#include "object.h" /* objects */
#include "objectio.h" /* objectio */
#include "storage.h" /* object storage */

#include <stdlib.h> /* malloc/realloc/free */
#include <string.h> /* strcmp */
#include <stdio.h> /* debugging */

#ifdef __cplusplus /* c++ check */
extern "C" {
#endif

#ifdef __cplusplus
/*
to future self: Although C allows multiple definitions
of variables through header files, but C++ doesn't. As
a result, I have moved these to their respective
location inside the c file. However, since I liked
the neatness of the original system, I have put this
inside an ifdef block.
*/
char **NAMES_VariableNames; /* variable names list */
int NAMES_VariableNamesSz; /* length of variable names list */
int NAMES_VariableNamesCap; /* capacity of variable names list */
object **NAMES_Variables; /* variable value list */
int NAMES_VariablesSz; /* length of variable value list */
int NAMES_VariablesCap; /* capacity of variable value list */
#endif

void NAMES_Assign(char *name, object *o) {
	/* reallocate if size is the same as cap */
	if (NAMES_VariableNamesSz >= NAMES_VariableNamesCap) {
		NAMES_VariableNames = (char**)realloc(NAMES_VariableNames, NAMES_VariableNamesCap * 2); /* realloc */
		NAMES_Variables = (object**)realloc(NAMES_Variables, NAMES_VariablesCap * 2); /* realloc */
		/* update capacity */
		NAMES_VariableNamesCap *= 2;
		NAMES_VariablesCap *= 2;
	}
	/* find name in list */
	int index = NAMES_FindName(name);
	/* copy to new buffer */
	char *nm = (char*)malloc(strlen(name)+1);
	strcpy(nm, name);
	char *name_old = name;
	name = nm;

	/* name must be freed if possible, since it will later get overridden */
	if (index != NAMES_VariableNamesSz)
		free(NAMES_VariableNames[index]);

	/* set object at that position */
	NAMES_VariableNames[index] = name;
	NAMES_Variables[index] = o;

	/* advance sizes if needed */
	if (index == NAMES_VariableNamesSz) {
		NAMES_VariableNamesSz++;
		NAMES_VariablesSz++;
	}
}

int NAMES_FindName(char *name) {
	int pos; /* position of name */
	/* loop through list */
	for (pos = 0; pos < NAMES_VariableNamesSz; pos++) {
		/* the same */
		if (!strcmp(NAMES_VariableNames[pos], name)) {
			break; /* break */
		}
	}
	return pos; /* return the index */
}

void NAMES_Init() {
	/* assign the lists */
	NAMES_Variables = (object**)malloc(sizeof(object*) * 100);
	NAMES_VariableNames = (char**)malloc(sizeof(char*) * 100);
	/* assign capacities */
	NAMES_VariableNamesCap = 100;
	NAMES_VariablesCap = 100;
	/* assign sizes */
	NAMES_VariablesSz = 0;
	NAMES_VariableNamesSz = 0;
	/* assign constants */
	NAMES_Assign((char*)"true", STORAGE_Register(OBJECT_NewInt(1)));
	NAMES_Assign((char*)"false", STORAGE_Register(OBJECT_NewInt(0)));
	NAMES_Assign((char*)"null", STORAGE_Register(OBJECT_NewInt(0)));
}

void NAMES_PrintNames() {
	/* loop through names */
	for (int i = 0; i < NAMES_VariablesSz; i++) {
		/* print name and variable */
		printf("%s = ", NAMES_VariableNames[i]);
		OBJECTIO_PrintObject(NAMES_Variables[i]);
	}
}

void NAMES_FreeAll() {
	/* free all of the names */
	for (int i = 0; i < NAMES_VariableNamesSz; i++) {
		/* free */
		free(NAMES_VariableNames[i]);
	}
	/* free lists */
	free(NAMES_Variables);
	free(NAMES_VariableNames);
}

object *NAMES_Get(char *name) {
	/* loop */
	int i; /* iter var */
	for (i = 0; i < NAMES_VariableNamesSz; i++) {
		/* the same */
		if (!strcmp(NAMES_VariableNames[i], name)) {
			break; /* break */
		}
	}
	if (i == NAMES_VariableNamesSz) { /* failed to find variable */
		return NULL;
	}
	else {
		object *o = NAMES_Variables[i]; /* found variable */
		return o;
	}
}

#ifdef __cplusplus /* c++ check */
}
#endif