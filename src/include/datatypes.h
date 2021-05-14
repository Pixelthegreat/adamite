/* interpreter data types; only includes (at the moment)
int types. */

#include <stdint.h> /* int types */
#include "os.h" /* os fundamental stuff */

#ifndef DATATYPES_H
#define DATATYPES_H

#ifdef __cplusplus /* c++ check */
extern "C" {
#endif

#if defined(WINDOWS32) || defined(GNU32) /* 32 bit */
#define Int int /* standard 32 bit integer */
#elif defined(WINDOWS64) || defined(GNU64) /* 64 bit */
#define Int int64_t
#endif

#ifdef __cplusplus /* c++ check */
}
#endif

#endif /* DATATYPES_H */