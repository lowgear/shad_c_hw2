#pragma once

#include <stdbool.h>
#include <string.h>
#include <ctype.h>

bool IsRx(char *rx) {
    return (strlen(rx) == 3
            && rx[0] == 'R'
            && isdigit(rx[1])
            && rx[1] <= 7
            && rx[2] == 'X')
           || strcmp(rx, "SP") == 0;
}

int RxToId(char *rx) {
    if (strcmp(rx, "SP") == 0)
        return 7;
    return rx[1] - '0';
}

bool IsRs(char *rx) {
    return strlen(rx) == 3
           && rx[0] == 'R'
           && isdigit(rx[1])
           && rx[1] <= 7
           && (rx[2] == 'L' || rx[2] == 'H');
}

int RsToId(char *rx) {
    return ((rx[1] - '0') << 1)
           + (rx[2] == 'H' ? 1 : 0);
}
