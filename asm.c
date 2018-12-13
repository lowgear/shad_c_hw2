#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>

#include "mingw_stdio_fix.h"
#include "argtools.h"
#include "asm/error_check_tools.h"
#include "asm/exit_codes.h"
#include "utils/vector.h"
#include "asm/labels.h"
#include "utils/strtools.h"
#include "asm/regtools.h"
#include "asm/readtoken.h"

typedef const size_t CS;
CS ARGNUM = 2;

CS LABEL_MAX_LEN = 20;

CS MAX_ARG_LEN = 7;

CS CMD_BUF_SIZE = LABEL_MAX_LEN + 2;

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
#define fscanf PrOhIbItEd
        CHECK_F(cmd[CMD_BUF_SIZE - 1] == '\0', "read command from", inPath, rv, UNKNOWN_COMMAND, cleanup)

        strToUpper(cmd);

        if (IS_LABEL(cmd)) {
            const size_t cmdLen = strlen(cmd);
            char *savedLabel = malloc(sizeof(char) * cmdLen + 1);
            CHECK(savedLabel != NULL, "malloc fail", cleanup)
            strcpy(savedLabel, cmd);
            savedLabel[cmdLen] = '\0';

            CHECK(CNT(instructions) <= MAX_ADDR / 2, "label address out of range", cleanup)

            CHECK(
                    addLabelAddress(
                            &labelAddresses,
                            savedLabel,
                            (int16_t) (2 * CNT(instructions))),
                    "add label fail",
                    freeLabel
            );

            continue;

            freeLabel:
            goto cleanup;
        }

#define IF(case) if (strcmp(cmd, #case) == 0)
#define ELIF(case) else IF(case)

        IF(MOV) {
            char to[MAX_ARG_LEN], from[MAX_ARG_LEN];
            to[MAX_ARG_LEN - 1] = from[MAX_ARG_LEN - 1] = '\0';

            CHECK_F(ReadToken(ifp, to, MAX_ARG_LEN),
                    "read first move arg from",
                    inPath, rv, 1, cleanup)
            CHECK_F(ReadToken(ifp, from, MAX_ARG_LEN),
                    "read second move arg from",
                    inPath, rv, 1, cleanup)

#define ISMEMOP(a) (((a)[0] == '(' && (a)[strlen((a)) - 1] == ')') ? 1 : 0)

            if (from[0] == '#') {
                CHECK(IsRs(to), "mov expected first RS", cleanup)


            }

            short opType = (short) ((ISMEMOP(to) << 1) | ISMEMOP(from));
            switch (opType) {
                case 0:

            }
        } ELIF(PUSH) {

        } ELIF(POP) {

        } ELIF(CALL) {

        } ELIF(RET) {

        } ELIF(ADD) {

        } ELIF(SUB) {

        } ELIF(MUL) {

        } ELIF(DIV) {

        } ELIF(AND) {

        } ELIF(OR) {

        } ELIF(XOR) {

        } ELIF(NOT) {

        } ELIF(SHL) {

        } ELIF(SHR) {

        } ELIF(RESER) {

        } ELIF(NOP) {

        } ELIF(JMP) {

        } ELIF(JE) {

        } ELIF(JNE) {

        } else {
            goto no_valid_input;
        }

#undef IF
#undef ELIF

        continue;
        no_valid_input:
        CHECK_F(feof(ifp), "read command from", inPath, rv, 2, cleanup)
    }

    cleanup:
    ROLLBACK_F(fclose(ifp) == 0, "close", inPath, rv, 5)
    exit:
    return rv;
}
