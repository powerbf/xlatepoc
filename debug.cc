/*
 * debug.cc
 *
 */

#include <stdio.h>
#include <stdlib.h>

//#include "AppHdr.h"

#if defined(__GNUC__)
# define NORETURN __attribute__ ((noreturn))
#elif defined(_MSC_VER)
# define NORETURN __declspec(noreturn)
#else
# define NORETURN
#endif



NORETURN void die(char const *file, int line, char const* msg, ...)
{
    printf("Died at %s:%d\n", file, line);
    exit(1);
}

