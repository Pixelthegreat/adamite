/* read storage.h for more info */
#include "storage.h" /* our header file */
#include "object.h" /* for object storage */
#include "objectio.h" /* printing objects (debug only) */
#include <stdlib.h> /* malloc/realloc/free */
#include <stdio.h> /* printf */

#ifdef __cplusplus /* c++ check */
extern "C" {
#endif

#ifdef __cplusplus
object **STORAGE_ObjectPointers; /* object pointers */
object **STORAGE_FreedPointers; /* freed pointers */
int STORAGE_ObjectPointersSz; /* size of object pointers list */
int STORAGE_FreedPointersSz; /* size of freed pointers list */
int STORAGE_ObjectPointersCap; /* capacity of object pointers list */
int STORAGE_FreedPointersCap; /* capacity of freed pointers list */
#endif

object *STORAGE_Register(object *o) {
	/* check if we need to resize */
	if (STORAGE_ObjectPointersSz >= STORAGE_ObjectPointersCap) {
		/* reallocate */
		STORAGE_ObjectPointers = (object**)realloc(STORAGE_ObjectPointers, sizeof(object*) * STORAGE_ObjectPointersCap * 2);
		/* update the size */
		STORAGE_ObjectPointersCap *= 2;
	}
	/* add the item */
	STORAGE_ObjectPointers[STORAGE_ObjectPointersSz++] = o;
	return o;
}

void STORAGE_Free(object *o) {
	/* free the actual object */
	OBJECT_FreeObject(o);
	/* check for it in our freed pointers list */
	int i;
	for (i = 0; i < STORAGE_FreedPointersSz; i++) {
		/* are they same */
		if (STORAGE_FreedPointers[i] == o) {
			/* break */
			break;
		}
	}
	if (i == STORAGE_FreedPointersSz) {
		/* check if we need to resize */
		if (STORAGE_FreedPointersSz >= STORAGE_FreedPointersCap) {
			/* reallocate */
			STORAGE_FreedPointers = (object**)realloc(STORAGE_FreedPointers, sizeof(object*) * STORAGE_FreedPointersCap * 2);
			/* update the size */
			STORAGE_FreedPointersCap *= 2;
		}
		/* add the pointer to the list */
		STORAGE_FreedPointers[STORAGE_FreedPointersSz] = o;
		/* advance the size */
		STORAGE_FreedPointersSz++;
	}
}

void STORAGE_FreeAll() {
	/* loop through object pointers */
	for (int i = 0; i < STORAGE_ObjectPointersSz; i++) {
		/* freed pointers aren't being used and probably won't.
		as a result, we are just going to immediately free the object
		from memory. */
		/* debug print object */
		#if defined(STORAGE_DEBUG) && STORAGE_DEBUG == 1
		printf("[DEBUG LOG] Pointer: %p, %d: ", STORAGE_ObjectPointers[i], STORAGE_ObjectPointers[i]->type);
		OBJECTIO_PrintObject(STORAGE_ObjectPointers[i]);
		#endif
		/* free object */
		OBJECT_FreeObject(STORAGE_ObjectPointers[i]);
		/* debug, print stuff */
		#if defined(STORAGE_DEBUG) && STORAGE_DEBUG == 1
		printf("[DEBUG LOG] Freed %p.\n", STORAGE_ObjectPointers[i]);
		#endif
	}
	/* free the freed pointers list */
	free(STORAGE_FreedPointers);
	/* free the object list */
	free(STORAGE_ObjectPointers);
}

void STORAGE_Init() {
	/* create the lists */
	STORAGE_ObjectPointers = (object**)malloc(sizeof(object*) * 100);
	STORAGE_FreedPointers = (object**)malloc(sizeof(object*) * 100);
	/* create length variables */
	STORAGE_FreedPointersSz = 0;
	STORAGE_ObjectPointersSz = 0;
	/* create capacity variables */
	STORAGE_FreedPointersCap = 100;
	STORAGE_ObjectPointersCap = 100;
}

int STORAGE_Find(object *o) {
	/* loop through list */
	int i; /* iter var */
	int found = 0; /* found var */
	for (i = 0; i < STORAGE_ObjectPointersSz; i++) {
		/* if equal */
		if (STORAGE_ObjectPointers[i] == o) {
			found = 1; /* found it */
		}
	}
	/* truth value */
	return found;
}

int STORAGE_FindFreed(object *o) {
	/* loop through list */
	int i; /* iter var */
	for (i = 0; i < STORAGE_FreedPointersSz; i++) {
		/* if equal */
		if (STORAGE_FreedPointers[i] == o)
			break; /* quit loop */
	}
	/* truth value */
	return (int)(!i == STORAGE_FreedPointersSz);
}

#ifdef __cplusplus /* c++ check */
}
#endif