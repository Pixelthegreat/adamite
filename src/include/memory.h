/* memory management macros. */
#include <stdlib.h>

#ifndef MEMORY_H
#define MEMORY_H

#ifdef __cplusplus /* c++ check */
extern "C" {
#endif

#define MEMORY_Malloc(type) (type*)(malloc(sizeof(type))) /* allocate an object */
#define MEMORY_Free(obj) free((void*)obj) /* free an object from memory */
#define MEMORY_Realloc(obj, sz) realloc(obj, sz) /* reallocate an object to new memory */

#ifdef __cplusplus /* c++ check */
}
#endif

#endif /* MEMORY_H */