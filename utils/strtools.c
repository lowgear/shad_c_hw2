#include <ctype.h>
#include "strtools.h"

void strToLower(char* str) {
    for (; *str; ++str)
        *str = (char) tolower(*str);
}

void strToUpper(char* str) {
    for (; *str; ++str)
        *str = (char) toupper(*str);
}

bool strIsSpace(const char* str) {
    for (; *str; ++str) {
        if (!isspace(*str))
            return false;
    }
    return true;
}
