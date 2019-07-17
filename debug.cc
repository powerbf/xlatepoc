
#include <cstdlib>
#include <cstdarg>
using namespace std;

#include "AppHdr.h"


#undef die
NORETURN void die(const char *file, int line, const char *format, ...)
{
    char tmp[2048] = {};

    va_list args;

    va_start(args, format);
    vsnprintf(tmp, sizeof(tmp), format, args);
    va_end(args);

    printf("ERROR in '%s' at line %d: %s",
             file, line, tmp);

    exit(1);
}

