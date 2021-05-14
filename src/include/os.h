/* operating system stuff, as well as information on what
compiler you are using. */

#ifndef OS_H
#define OS_H

#ifdef __cplusplus /* c++ check */
extern "C" {
#endif

#if defined(_WIN32) || defined(_WIN64) /* windows */
#define WINDOWS /* Operating System is Windows */
#ifdef _WIN64
#define WINDOWS64 /* OS is 64-bit */
#else
#define WINDOWS32 /* OS is 32-bit */
#endif /* _WIN64:_WIN32 */
#endif /* _WIN32, _WIN64 */

#if __GNUC__ /* g++ or gcc used here */
#define GNU
#if __x86_64__ || __ppc64__ /* 64-bit os */
#define GNU64
#else
#define GNU32
#endif /* __x86_64__||__ppc64__:? */
#endif /* __GNUC__ */

#ifdef __unix__ /* linux/unix systems */
#define LINUX
#endif /* __unix__ */

#ifdef __cplusplus /* c++ check */
}
#endif

#endif /* OS_H */