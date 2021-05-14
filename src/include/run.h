/* for running files */
#ifndef RUN_H
#define RUN_H

#ifdef __cplusplus /* c++ check */
extern "C" {
#endif

int run(const char *fname); /* run the code in a file; returns 0 if no error, 1 if error */

#ifdef __cplusplus /* c++ check */
}
#endif

#endif /* RUN_H */