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
                    AddLabelAddress(
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
                  "read first MOV arg from", cleanup)
            CHECK(ReadToken(ifp, from, ARG_BUF_SIZE),
                  "read second MOV arg from", cleanup)

#define ISMEMOP(a) (((a)[0] == '(' && (a)[strlen((a)) - 1] == ')') ? 1 : 0)

            if (ParseImm8(from, &imm8)) {
                CHECK(IsRs(to), "MOV expected first RS", cleanup)
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
                        CHECK(IsRx(to) && IsRx(from), "MOV expected RX RX", cleanup)
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
                        CHECK(false, "MOV (RX) (RX) not supported", cleanup)
                }
            }

#undef SETINST
        } ELIF(PUSH) {
            CHECK(ReadToken(ifp, arg, ARG_BUF_SIZE), "failed read PUSH arg", cleanup)
            CHECK(IsRx(arg), "PUSH expected RX", cleanup)
            instruction = (uint16_t) ((4u << 12) | RxToId(arg));
        } ELIF(POP) {
            CHECK(ReadToken(ifp, arg, ARG_BUF_SIZE), "failed read POP arg", cleanup)
            CHECK(IsRx(arg), "POP expected RX", cleanup)
            instruction = (uint16_t) ((5u << 12) | RxToId(arg));
        } ELIF(CALL) {
            CHECK(ReadToken(ifp, arg, ARG_BUF_SIZE), "failed read CALL arg", cleanup)
            CHECK(ParseImm8(arg, &imm8), "CALL expected #imm8", cleanup)
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
                  "failed read first " #name " arg", cleanup) \
            CHECK(IsRx(a),  #name " expected RX RX", cleanup) \
            CHECK(ReadToken(ifp, b, ARG_BUF_SIZE), \
                  "failed read second " #name " arg", cleanup) \
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

#undef BIN_OP
#undef SETINST
        } ELIF(NOT) {
            CHECK(ReadToken(ifp, a, ARG_BUF_SIZE),
                  "failed read first NOT arg", cleanup)
            CHECK(IsRx(a), "NOT expected RX", cleanup)
            instruction = (uint16_t) ((22u << 11) | RxToId(a));
        } ELIF(SHL) {
            CHECK(ReadToken(ifp, a, ARG_BUF_SIZE),
                  "failed read first SHL arg", cleanup)
            CHECK(IsRx(a), "SHL expected RX", cleanup)
            CHECK(ReadToken(ifp, b, ARG_BUF_SIZE),
                  "failed read second SHL arg", cleanup)
            uint8_t imm4;
            CHECK(ParseImm4(b, &imm4), "failed parse #imm4", cleanup)
            instruction = (uint16_t) ((23u << 11)
                                      | (RxToId(a) << 4)
                                      | imm4);
        } ELIF(SHR) {
            CHECK(ReadToken(ifp, a, ARG_BUF_SIZE),
                  "failed read first SHR arg", cleanup)
            CHECK(IsRx(a), "SHL expected RX", cleanup)
            CHECK(ReadToken(ifp, b, ARG_BUF_SIZE),
                  "failed read second SHR arg", cleanup)
            uint8_t imm4;
            CHECK(ParseImm4(b, &imm4), "failed parse #imm4", cleanup)
            instruction = (uint16_t) ((24u << 11)
                                      | (RxToId(a) << 4)
                                      | imm4);
        } ELIF(RESET) {
            instruction = (uint16_t) (25u << 11);
        } ELIF(NOP) {
            instruction = (uint16_t) (26u << 11);
        } ELIF(JMP) {

        } ELIF(JE) {

        } ELIF(JNE) {

        } ELIF(IN) {
            CHECK(ReadToken(ifp, a, ARG_BUF_SIZE),
                  "failed read IN arg", cleanup)
            CHECK(IsRs(a), "IN expected RS", cleanup)
            instruction = (uint16_t) ((30u << 11) | RsToId(a));
        } ELIF(OUT) {
            CHECK(ReadToken(ifp, a, ARG_BUF_SIZE),
                  "failed read OUT arg", cleanup)
            CHECK(IsRs(a), "OUT expected RS", cleanup)
            instruction = (uint16_t) ((31u << 11) | RsToId(a));
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
