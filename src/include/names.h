/* simple library for holding a list of the names of variables */
#include "object.h" /* objects */

#ifndef NAMES_H
#define NAMES_H

#ifdef __cplusplus /* c++ check */
extern "C" {
#endif

#ifndef __cplusplus
char **NAMES_VariableNames; /* variable names list */
int NAMES_VariableNamesSz; /* length of variable names list */
int NAMES_VariableNamesCap; /* capacity of variable names list */
object **NAMES_Variables; /* variable value list */
int NAMES_VariablesSz; /* length of variable value list */
int NAMES_VariablesCap; /* capacity of variable value list */
#endif

void NAMES_Assign(char* name, object *o); /* add or assign a name */
int NAMES_FindName(char *name); /* find a name, return the position of it */
void NAMES_FreeAll(); /* free all names */
void NAMES_Init(); /* init names list */
void NAMES_PrintNames(); /* print all variable names (debug only) */
object *NAMES_Get(char *name); /* get a value from a name */

#ifdef __cplusplus /* c++ check */
}
#endif

#endif /* NAMES_H */