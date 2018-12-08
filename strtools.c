#include <stdbool.h>
#include <ctype.h>

bool strIsSpace(char* str) {
    for (; *str; ++str) {
        if (!isspace(*str))
            return false;
    }
    return true;
}
