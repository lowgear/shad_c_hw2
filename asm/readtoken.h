#pragma once

#include <stdbool.h>
#include <stdio.h>

bool ReadToken(FILE* ifp, char* str, size_t len) {
    str[len - 1] = '\0';
    int read = fscanf(ifp, "%s", str);
    return read == 1 && str[len - 1] == '\0';
}
