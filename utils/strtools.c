#include <ctype.h>
#include "strtools.h"

void strToLower(char* str) {
    for (; *str; ++str)
        *str = tolower(*str);
}

void strToUpper(char* str) {
    for (; *str; ++str)
        *str = toupper(*str);
}

bool strIsSpace(char* str) {
    for (; *str; ++str) {
        if (!isspace(*str))
            return false;
    }
    return true;
}
