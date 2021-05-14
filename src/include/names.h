/* simple library for holding a list of the names of variables */
#include "object.h" /* objects */

#ifndef NAMES_H
#define NAMES_H

#ifdef __cplusplus /* c++ check */
extern "C" {
#endif

char **NAMES_VariableNames;
int NAMES_VariableNamesSz;
int NAMES_VariableNamesCap;

object **NAMES_Variables;
int NAMES_VariablesSz;
int NAMES_VariablesCap;

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