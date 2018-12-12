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
#include "asm/labels.h"


typedef const size_t CS;
CS ARGNUM = 2;

CS LABEL_MAX_LEN = 20;

CS CMD_BUF_SIZE = LABEL_MAX_LEN + 1;

CS MAX_ADDR = (1 << 15) - 1;

#define IS_LABEL(str) ((str)[strlen(str) - 1] == ':')

typedef uint16_t instr;
DEFINE_VECTOR(instr)


int main(int argc, char *argv[]) {
    CHECK_FALL(argc == ARGNUM + 1, "Number of arguments should be 3, but got %d", argc - 1)

    const char *const inPath = argv[1];
    const char *const outPath = argv[2];

    V_LabelDescPtr_Ptr labelAddresses;
    INIT_VEC(labelAddresses, 1, exit)
    V_instr_Ptr instructions;
    INIT_VEC(instructions, 1, exit)

    FILE *ifp;
    int rv = 0;
    CHECK_F(ifp = fopen(inPath, "r"), "open", inPath, rv, CANT_OPEN_INPUT_FILE, exit)
    while (!feof(ifp)) {
        char cmd[CMD_BUF_SIZE];
        cmd[CMD_BUF_SIZE - 1] = '\0';
        if (fscanf(ifp, "%s", cmd) != 1)
            goto no_valid_input;
        CHECK_F(cmd[CMD_BUF_SIZE - 1] == '\0', "read command from", inPath, rv, UNKNOWN_COMMAND, cleanup)

        if (IS_LABEL(cmd)) {
            const size_t cmdLen = strlen(cmd);
            char* savedLabel = malloc(sizeof(char) * cmdLen + 1);
            CHECK(savedLabel != NULL, "malloc fail", cleanup)
            strcpy(savedLabel, cmd);

            CHECK(CNT(instructions) <= MAX_ADDR / 2, "label address out of range", cleanup)

            addLabelAddress(&labelAddresses, savedLabel, (uint16_t) (2 * CNT(instructions)));

            continue;
        }



        continue;
        no_valid_input:
        CHECK_F(feof(ifp), "read command from", inPath, rv, 2, cleanup)
    }

    cleanup:
    ROLLBACK_F(fclose(ifp) == 0, "close", inPath, rv, 5)
    exit:
    return rv;
}
