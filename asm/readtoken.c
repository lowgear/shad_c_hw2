#include "readtoken.h"

bool ReadToken(FILE* ifp, char* str, unsigned int bufSize) {
    str[bufSize - 2] = '\0';
    char format[23];
    sprintf(format, "%%%us", bufSize - 2);
    int read = fscanf(ifp, format, str);

    if (read == 1 && str[bufSize - 2] == '\0') {
        strToUpper(str);
        return true;
    }

    return false;
}