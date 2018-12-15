#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <inttypes.h>
#include <errno.h>

#include "token_tools.h"

bool IsRx(const char *rx) {
    return (strlen(rx) == 3
            && rx[0] == 'R'
            && isdigit(rx[1])
            && rx[1] <= 7
            && rx[2] == 'X')
           || strcmp(rx, "SP") == 0;
}

int RxToId(const char *rx) {
    if (strcmp(rx, "SP") == 0)
        return 7;
    return rx[1] - '0';
}

bool IsRs(const char *rx) {
    return strlen(rx) == 3
           && rx[0] == 'R'
           && isdigit(rx[1])
           && rx[1] <= 7
           && (rx[2] == 'L' || rx[2] == 'H');
}

int RsToId(const char *rx) {
    return ((rx[1] - '0') << 1)
           + (rx[2] == 'H' ? 1 : 0);
}

#define IS_LABEL(str) ((str)[strlen(str) - 1] == ':')

bool ParseImm8(const char* str, uint8_t* imm8) {
    unsigned long res = strtoul(str, NULL, 0);
    if (errno == ERANGE || UINT8_MAX < res)
        return false;
    *imm8 = (uint8_t) res;
    return true;
}

bool ParseSignedImm8(const char* str, uint8_t* imm8) {
    long res = strtol(str, NULL, 0);
    if (errno == ERANGE || res < INT8_MIN || INT8_MAX < res)
        return false;
    *imm8 = (uint8_t) res;
    return true;
}

bool ParseImm4(const char* str, uint8_t* imm4) {
    unsigned long res = strtoul(str, NULL, 0);
    if (errno == ERANGE || res > 15)
        return false;
    *imm4 = (uint8_t) res;
    return true;
}

void TrimBrackets(char* str) {
    str[strlen(str) - 1] = '\0';
    for (; (str[0] = str[1]); ++str);
}
