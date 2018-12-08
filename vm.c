#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <inttypes.h>
#include "mingw_stdio_fix.h"
#include "strtools.h"
#include "argtools.h"


const int ARGNUM = 3;


int main(int argc, char* argv[]) {
    CHECK(argc == ARGNUM + 1, "Number of arguments should be 3, but got %d", argc - 1)

    char* endOfNumber;
    unsigned long long maxInstructions = strtoull(argv[1], &endOfNumber, 0);
    CHECK(errno != ERANGE, "First argument is not an unsigned integer or out of range: <%s>", argv[1])
    CHECK(strIsSpace(endOfNumber), "There are extra chars in first argument which should be integer: <%s>", argv[1])

    unsigned long programAddress = strtoul(argv[3], &endOfNumber, 0);
    CHECK(errno != ERANGE, "Third argument is not an unsigned integer or out of range: <%s>", argv[3])
    CHECK(strIsSpace(endOfNumber), "There are extra chars in third argument which should be integer: <%s>", argv[3])
    CHECK(programAddress <= UINT16_MAX, "Third argument should not be greater than UINT16_MAX<%hu>: <%s>", UINT16_MAX, argv[3])

    printf("%llu ", maxInstructions);
    printf("%lu", programAddress);
}