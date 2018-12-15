#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>

#include "mingw_stdio_fix.h"
#include "argtools.h"
#include "utils/error_check_tools.h"
#include "asm/exit_codes.h"
#include "utils/vector.h"
#include "asm/labels.h"
#include "utils/strtools.h"
#include "asm/token_tools.h"
#include "asm/readtoken.h"

#define ARG_NUM 2

#define LABEL_MAX_LEN 20

#define ARG_BUF_SIZE 7

#define CMD_BUF_SIZE (LABEL_MAX_LEN + 3)

typedef uint16_t instr;
DEFINE_VECTOR(instr)


int main(int argc, char *argv[]) {
    int rv = 0;
    int fileType;

    CHECK(argc == ARG_NUM + 1, "Number of arguments should be 2", rv = INVALID_ARGS_NUM, exit)

    const char *const inPath = argv[1];
    const char *const outPath = argv[2];

    V_LabelDescPtr_Ptr labels;
    INIT_VEC(labels, 1, exit)
    V_instr_Ptr instructs;
    INIT_VEC(instructs, 1, freeLabels)

    FILE *fp;
    fileType = IN_FILE_TYPE;
    CHECK_F(fp = fopen(inPath, "r"), "open", inPath, rv, CANT_OPEN_FILE | fileType, freeInstrs)

#define CHECK_READ(condition, message) CHECK_F((condition), message, inPath, rv, READ_FAIL, closeFile)
#define CHECK_ARG(condition, message) CHECK((condition), message, rv = BAD_ARG, closeFile)

    while (!feof(fp)) {
        char cmd[CMD_BUF_SIZE];
        cmd[CMD_BUF_SIZE - 1] = '\0';
        if (fscanf(fp, "%s", cmd) != 1) {
            goto checkEOF;
        }

        CHECK(cmd[CMD_BUF_SIZE - 1] == '\0', "unknown command", rv = UNKNOWN_COMMAND, closeFile)

        strToUpper(cmd);

        if (IS_LABEL(cmd)) {
            cmd[strlen(cmd) - 1] = '\0';

            size_t labelId = FindLabDesc(labels, cmd);
            if (labelId == labels->cnt) {
                CHECK(AddLabDesc(&labels, cmd), "failed to add new label descriptor", rv = ALLOC_FAIL, closeFile)
            }

            ID(labels, labelId)->labelId = CNT(instructs);

            continue;
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

            CHECK_READ(ReadToken(fp, to, ARG_BUF_SIZE),                  "read first MOV arg from")
            CHECK_READ(ReadToken(fp, from, ARG_BUF_SIZE),                  "read second MOV arg from")

#define ISMEMOP(a) (((a)[0] == '(' && (a)[strlen((a)) - 1] == ')') ? 1 : 0)

            if (ParseImm8(from, &imm8)) {
                CHECK_ARG(IsRs(to), "MOV expected first RS")

                instruction = (uint16_t) ((3 << 12)
                                          | (RsToId(to) << 8)
                                          | imm8);
            } else {

#define SETINST(code) \
instruction = (uint16_t) (((code##u) << 12) \
| (RxToId(to) << 3) \
| RxToId(from));

                short opType = (short) ((ISMEMOP(to) << 1) | ISMEMOP(from));
                switch (opType) {
                    case 0:
                        CHECK_ARG(IsRx(to) && IsRx(from), "MOV expected RX RX")
                        SETINST(0)
                        break;
                    case 1:
                        TrimBrackets(from);
                        CHECK_ARG(IsRx(to) && IsRx(from), "MOV expected RX (RX)")
                        SETINST(1)
                        break;
                    case 2:
                        TrimBrackets(from);
                        CHECK_ARG(IsRx(to) && IsRx(from), "MOV expected (RX) RX")
                        SETINST(2)
                        break;
                    case 3:
                        CHECK(false, "MOV (RX) (RX) not supported", rv = UNKNOWN_COMMAND, closeFile)
                    default:
                        CHECK(false, "unexpected workflow", rv = UNWNOWN_ERROR, closeFile)
                }
            }

#undef SETINST
        } ELIF(PUSH) {
            CHECK_READ(ReadToken(fp, arg, ARG_BUF_SIZE), "failed read PUSH arg from")
            CHECK_ARG(IsRx(arg), "PUSH expected RX")
            instruction = (uint16_t) ((4u << 12) | RxToId(arg));
        } ELIF(POP) {
            CHECK_READ(ReadToken(fp, arg, ARG_BUF_SIZE), "failed read POP arg")
            CHECK_ARG(IsRx(arg), "POP expected RX")
            instruction = (uint16_t) ((5u << 12) | RxToId(arg));
        } ELIF(CALL) {
            CHECK_READ(ReadToken(fp, arg, ARG_BUF_SIZE), "failed read CALL arg")
            CHECK_ARG(ParseSignedImm8(arg, &imm8), "CALL expected #imm8")
            instruction = (uint16_t) ((6u << 12) | imm8);
        } ELIF(RET) {
            instruction = 14u << 11;
        } ELIF(ADD) {
#define SETINST(code) \
instruction = (uint16_t) (((code##u) << 11) \
| (RxToId(a) << 3) \
| RxToId(b));

#define BIN_OP(code, name) \
            CHECK_READ(ReadToken(fp, a, ARG_BUF_SIZE), \
                  "failed read first " #name " arg from") \
            CHECK_ARG(IsRx(a),  #name " expected RX RX") \
            CHECK_READ(ReadToken(fp, b, ARG_BUF_SIZE), \
                  "failed read second " #name " arg") \
            CHECK_ARG(IsRx(b), #name " expected RX RX") \
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
            CHECK_READ(ReadToken(fp, a, ARG_BUF_SIZE),
                  "failed read first NOT arg from")
            CHECK_ARG(IsRx(a), "NOT expected RX")
            instruction = (uint16_t) ((22u << 11) | RxToId(a));
        } ELIF(SHL) {
            CHECK_READ(ReadToken(fp, a, ARG_BUF_SIZE),
                  "failed read first SHL arg")
            CHECK_ARG(IsRx(a), "SHL expected RX")
            CHECK_READ(ReadToken(fp, b, ARG_BUF_SIZE),
                  "failed read second SHL arg")
            uint8_t imm4;
            CHECK_ARG(ParseImm4(b, &imm4), "failed parse #imm4")
            instruction = (uint16_t) ((23u << 11)
                                      | (RxToId(a) << 4)
                                      | imm4);
        } ELIF(SHR) {
            CHECK_READ(ReadToken(fp, a, ARG_BUF_SIZE),
                  "failed read first SHR arg from")
            CHECK_ARG(IsRx(a), "SHL expected RX")
            CHECK_READ(ReadToken(fp, b, ARG_BUF_SIZE),
                  "failed read second SHR arg from")
            uint8_t imm4;
            CHECK_ARG(ParseImm4(b, &imm4), "failed parse #imm4")
            instruction = (uint16_t) ((24u << 11)
                                      | (RxToId(a) << 4)
                                      | imm4);
        } ELIF(RESET) {
            instruction = (uint16_t) (25u << 11);
        } ELIF(NOP) {
            instruction = (uint16_t) (26u << 11);
        } ELIF(JMP) {
            CHECK_READ(ReadToken(fp, cmd, CMD_BUF_SIZE), "failed read label for JMP from")

            size_t labelId = FindLabDesc(labels, cmd);
            if (labelId == labels->cnt) {
                CHECK(AddLabDesc(&labels, cmd), "failed to add new label descriptor", rv = ALLOC_FAIL, closeFile)
            }

            PUSH_BACK_P(&(ID(labels, labelId)->instructionIds), instructs->cnt, closeFile)

            instruction = (uint16_t) (27u << 11);
        } ELIF(JE) {
            CHECK_READ(ReadToken(fp, cmd, CMD_BUF_SIZE), "failed read label for JE from")
            CHECK_READ(ReadToken(fp, a, ARG_BUF_SIZE), "failed read RX for JE from")
            CHECK_ARG(IsRx(a), "JE expected RX")

            size_t labelId = FindLabDesc(labels, cmd);
            if (labelId == labels->cnt) {
                CHECK(AddLabDesc(&labels, cmd), "failed to add new label descriptor", rv = ALLOC_FAIL, closeFile)
            }

            PUSH_BACK_P(&(ID(labels, labelId)->instructionIds), instructs->cnt, closeFile)

            instruction = (uint16_t) ((28u << 11) | (RxToId(a) << 8));
        } ELIF(JNE) {
            CHECK_READ(ReadToken(fp, cmd, CMD_BUF_SIZE), "failed read label for JNE from")
            CHECK_READ(ReadToken(fp, a, ARG_BUF_SIZE), "failed read RX for JNE from")
            CHECK_ARG(IsRx(a), "JNE expected RX")

            size_t labelId = FindLabDesc(labels, cmd);
            if (labelId == labels->cnt) {
                CHECK(AddLabDesc(&labels, cmd), "failed to add new label descriptor", rv = ALLOC_FAIL, closeFile)
            }

            PUSH_BACK_P(&(ID(labels, labelId)->instructionIds), instructs->cnt, closeFile)

            instruction = (uint16_t) ((29u << 11) | (RxToId(a) << 8));
        } ELIF(IN) {
            CHECK_READ(ReadToken(fp, a, ARG_BUF_SIZE),
                  "failed read IN arg from")
            CHECK_ARG(IsRs(a), "IN expected RS")
            instruction = (uint16_t) ((30u << 11) | RsToId(a));
        } ELIF(OUT) {
            CHECK_READ(ReadToken(fp, a, ARG_BUF_SIZE),
                  "failed read OUT arg from")
            CHECK_ARG(IsRs(a), "OUT expected RS")
            instruction = (uint16_t) ((31u << 11) | RsToId(a));
        } else {
            printf("token not recognized\n");
            goto closeFile;
        }

#undef IF
#undef ELIF

        PUSH_BACK_P(&instructs, instruction, closeFile)
        continue;

        checkEOF:
        CHECK_F(feof(fp), "read command from", inPath, rv, UNKNOWN_COMMAND, closeFile)
    }

    for (size_t i = 0; i < CNT(labels); ++i) {
        LabelDescPtr descP = ID(labels, i);
        int64_t labelId = descP->labelId;

        V_size_t_Ptr instrIds = descP->instructionIds;
        for (size_t j = 0; j < CNT(instrIds); ++j) {
            int64_t instrId = ID(instrIds, j);
            const int64_t int64__addrDiff = (instrId - labelId) * 2;
            CHECK(INT8_MIN <= int64__addrDiff && int64__addrDiff <= INT8_MAX, "relative call address diff out of range", rv = RELATIVE_CALL_OUT_OF_RANGE, closeFile)
            const int8_t addrDiff = (const int8_t) int64__addrDiff;
            ID(instructs, instrId) |= addrDiff;
        }
    }

    CHECK_F(fclose(fp) == 0, "close", inPath, rv, CANT_CLOSE_FILE | fileType, exit)

    fileType = OUT_FILE_TYPE;
    
    CHECK_F(fp = fopen(inPath, "w"), "open write", outPath, rv, CANT_OPEN_FILE | fileType, freeInstrs)
    for (size_t i = 0; i < CNT(instructs); ++i) {
        int lower = ID(instructs, i) & UINT8_MAX;
        int upper = (ID(instructs, i) >> 8) & UINT8_MAX;
        CHECK_F(fputc(lower, fp) != EOF, "write instruction to", outPath, rv, CANT_WRITE_TO_FILE | fileType, closeFile)
        CHECK_F(fputc(upper, fp) != EOF, "write instruction to", outPath, rv, CANT_WRITE_TO_FILE | fileType, closeFile)
    }

    closeFile:
    ROLLBACK_F(fclose(fp) == 0, "close", inPath, rv, CANT_CLOSE_FILE | fileType)

    freeInstrs:
    FREE_V(instructs);

    freeLabels:
    for (size_t i = 0; i < CNT(labels); ++i) {
        FreeDesc(ID(labels, i));
    }
    FREE_V(labels);

    exit:
    return rv;
}
