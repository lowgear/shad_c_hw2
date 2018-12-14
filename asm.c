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
#include "asm/token_tools.h"
#include "asm/readtoken.h"

typedef const size_t CS;
CS ARGNUM = 2;

CS LABEL_MAX_LEN = 20;

CS ARG_BUF_SIZE = 7;

CS CMD_BUF_SIZE = LABEL_MAX_LEN + 3;

CS MAX_ADDR = (1 << 15) - 1;

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

        uint16_t instruction;

#define IF(case) if (strcmp(cmd, #case) == 0)
#define ELIF(case) else IF(case)

        char arg[ARG_BUF_SIZE];
        uint8_t imm8;
        char a[ARG_BUF_SIZE], b[ARG_BUF_SIZE];

        IF(MOV) {
            char to[ARG_BUF_SIZE], from[ARG_BUF_SIZE];
            to[ARG_BUF_SIZE - 1] = from[ARG_BUF_SIZE - 1] = '\0';

            CHECK(ReadToken(ifp, to, ARG_BUF_SIZE),
                  "read first move arg from", cleanup)
            CHECK(ReadToken(ifp, from, ARG_BUF_SIZE),
                  "read second move arg from", cleanup)

#define ISMEMOP(a) (((a)[0] == '(' && (a)[strlen((a)) - 1] == ')') ? 1 : 0)

            if (ParseImm8(from, &imm8)) {
                CHECK(IsRs(to), "mov expected first RS", cleanup)
                int toId = RsToId(to);

                instruction = (uint16_t) ((3 << 12)
                                          | (toId << 8)
                                          | imm8);

                continue;
            } else {

#define SETINST(code) \
instruction = (uint16_t) (((code##u) << 12) \
| (RxToId(to) << 3) \
| RxToId(from));

                short opType = (short) ((ISMEMOP(to) << 1) | ISMEMOP(from));
                switch (opType) {
                    case 0:
                        CHECK(IsRx(to) && IsRx(from), "mov expected RX RX", cleanup)
                        SETINST(0)
                        break;
                    case 1:
                        TrimBrackets(from);
                        SETINST(1)
                        break;
                    case 2:
                        TrimBrackets(from);
                        SETINST(2)
                        break;
                    default:
                        CHECK(false, "mov (RX) (RX) not supported", cleanup)
                }
            }

#undef SETINST
        } ELIF(PUSH) {
            CHECK(ReadToken(ifp, arg, ARG_BUF_SIZE), "failed read push arg", cleanup)
            CHECK(IsRx(arg), "push expected RX", cleanup)
            instruction = (uint16_t) ((4u << 12) | RxToId(arg));
        } ELIF(POP) {
            CHECK(ReadToken(ifp, arg, ARG_BUF_SIZE), "failed read pop arg", cleanup)
            CHECK(IsRx(arg), "pop expected RX", cleanup)
            instruction = (uint16_t) ((5u << 12) | RxToId(arg));
        } ELIF(CALL) {
            CHECK(ReadToken(ifp, arg, ARG_BUF_SIZE), "failed read call arg", cleanup)
            CHECK(ParseImm8(arg, &imm8), "call expected #imm8", cleanup)
            instruction = (uint16_t) ((6u << 12) | imm8);
        } ELIF(RET) {
            instruction = 14u << 11;
        } ELIF(ADD) {
#define SETINST(code) \
instruction = (uint16_t) (((code##u) << 11) \
| (RxToId(a) << 3) \
| RxToId(b));

#define BIN_OP(code, name) \
            CHECK(ReadToken(ifp, a, ARG_BUF_SIZE), \
                  "read first " #name " arg from", cleanup) \
            CHECK(IsRx(a),  #name " expected RX RX", cleanup) \
            CHECK(ReadToken(ifp, b, ARG_BUF_SIZE), \
                  "read second " #name " arg from", cleanup) \
            CHECK(IsRx(b), #name " expected RX RX", cleanup) \
            SETINST(code)

            BIN_OP(15, ADD)
        } ELIF(SUB) {
            BIN_OP(16, SUB)
        } ELIF(MUL) {
            BIN_OP(17, MUL)
        } ELIF(DIV) {
            BIN_OP(18, DIV)
        } ELIF(AND) {
            BIN_OP(19, AND)
        } ELIF(OR) {
            BIN_OP(20, OR)
        } ELIF(XOR) {
            BIN_OP(21, XOR)
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

        PUSH_BACK_P(&instructions, instruction, cleanup)

        continue;
        no_valid_input:
        CHECK_F(feof(ifp), "read command from", inPath, rv, 2, cleanup)
    }

    cleanup:
    ROLLBACK_F(fclose(ifp) == 0, "close", inPath, rv, 5)
    exit:
    return rv;
}
