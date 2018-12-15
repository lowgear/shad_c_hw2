#pragma once

#include <stdio.h>

long getFileSize(FILE* fp) {
    long prevPos = ftell(fp);
    fseek(fp, 0L, SEEK_END);
    long size = ftell(fp);
    fseek(fp, prevPos, SEEK_SET);

    return size;
}
