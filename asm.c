#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <inttypes.h>

#include "mingw_stdio_fix.h"
#include "strtools.h"
#include "argtools.h"
#include "asm/tokenizer.h"


const int ARGNUM = 2;


int main(int argc, char* argv[]) {
    CHECK(argc == ARGNUM + 1, "Number of arguments should be 3, but got %d", argc - 1)

    const char* const inPath = argv[1];
    const char* const outPath = argv[2];



    return 0;
}
