/* IO functions for objects. */
#include "object.h" /* object stuff */

#ifndef OBJECTIO_H
#define OBJECTIO_H

#ifdef __cplusplus /* c++ check */
extern "C" {
#endif

object *OBJECTIO_PrintObject(object *o); /* print the value of an object */
object *OBJECTIO_Input(); /* get input of an object and return the object */

#ifdef __cplusplus /* c++ check */
}
#endif

#endif /* OBJECTIO_H */