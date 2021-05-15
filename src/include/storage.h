/* system to handle storage of objects.
This storage system will, when requested,
free all objects in the list that haven't
been freed. when the debug constant is
defined, it will print out all of the memory
addresses that haven't been freed already. */

#include "object.h" /* header for objects */

#ifndef STORAGE_H
#define STORAGE_H

#ifdef __cplusplus /* c ++ check */
extern "C" {
#endif

#define STORAGE_DEBUG 0 /* determine if we want to debug */

#ifndef __cplusplus
object **STORAGE_ObjectPointers; /* object pointers */
object **STORAGE_FreedPointers; /* freed pointers */
int STORAGE_ObjectPointersSz; /* size of object pointers list */
int STORAGE_FreedPointersSz; /* size of freed pointers list */
int STORAGE_ObjectPointersCap; /* capacity of object pointers list */
int STORAGE_FreedPointersCap; /* capacity of freed pointers list */
#endif

object *STORAGE_Register(object *o); /* register an object into our list */
void STORAGE_Free(object *o); /* free an object and add the object pointer to list of freed pointers */
void STORAGE_FreeAll(); /* should be run at end of program to free all unfreed memory */
void STORAGE_Init(); /* initialise pointer lists */
int STORAGE_Find(object *o); /* find an object in storage, return 1 if it exists, 0 if otherwise */
int STORAGE_FindFreed(object *o); /* same as STORAGE_Find, searches through freed pointer list */

#ifdef __cplusplus /* c++ check */
}
#endif

#endif /* STORAGE_H */