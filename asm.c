#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>

#include "mingw_stdio_fix.h"
#include "strtools.h"
#include "argtools.h"
#include "asm/tokenizer.h"
#include "asm/error_check_tools.h"
#include "asm/exit_codes.h"
#include "utils/vector.h"

typedef const size_t CS;
CS ARGNUM = 2;

CS LABEL_MAX_LEN = 20;

CS CMD_BUF_SIZE = LABEL_MAX_LEN + 1;


#define IS_LABEL(str) ((str)[strlen(str) - 1] == ':')

typedef char* CharPtr;
DEFINE_VECTOR(CharPtr)


int main(int argc, char *argv[]) {
    CHECK_FALL(argc == ARGNUM + 1, "Number of arguments should be 3, but got %d", argc - 1)

    const char *const inPath = argv[1];
    const char *const outPath = argv[2];

    struct Vector_CharPtr labels;

    FILE *ifp;
    int rv = 0;
    CHECK(ifp = fopen(inPath, "r"), "open", inPath, rv, CANT_OPEN_INPUT_FILE, exit)
    while (!feof(ifp)) {
        char cmd[CMD_BUF_SIZE];
        cmd[CMD_BUF_SIZE - 1] = '\0';
        if (fscanf(ifp, "%s", cmd) != 1)
            goto no_valid_input;
        CHECK(cmd[CMD_BUF_SIZE - 1] == '\0', "read command from", inPath, rv, UNKNOWN_COMMAND, cleanup)

        if (IS_LABEL(cmd))


        continue;
        no_valid_input:
        CHECK(feof(ifp), "read command from", inPath, rv, 2, cleanup)
    }

    cleanup:
    ROLLBACK(fclose(ifp) == 0, "close", inPath, rv, 5)
    exit:
    return rv;
}
