#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <inttypes.h>

#include "mingw_stdio_fix.h"
#include "strtools.h"
#include "argtools.h"
#include "asm/tokenizer.h"
#include "asm/error_check_tools.h"


const int ARGNUM = 2;


int main(int argc, char *argv[]) {
    CHECK_FALL(argc == ARGNUM + 1, "Number of arguments should be 3, but got %d", argc - 1)

    const char *const inPath = argv[1];
//    const char *const outPath = argv[2];

    FILE *ifp;
    int rv = 0;
    CHECK(ifp = fopen(inPath, "r"), "open", inPath, rv, 1, exit);
    while (!feof(ifp)) {
        char cmd[10];
        /*int read = */fscanf(ifp, "%s", cmd);


//        check_eof:

    }

//    cleanup:
    ROLLBACK(fclose(ifp) == 0, "close", inPath, rv, 5);
    exit:
    return rv;
}
