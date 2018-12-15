#include "mingw_stdio_fix.h"

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <inttypes.h>

#include "utils/strtools.h"
#include "utils/error_check_tools.h"
#include "utils/iotools.h"
#include "vm/exit_codes.h"
#include "vm/vmstruct.h"
#include "vm/handlersarray.h"


const int ARG_NUM = 3;

const size_t ADDR_NUM = 1 << 15;

uint8_t mem[ADDR_NUM];

int main(int argc, char *argv[]) {
    int rv = 0;
    CHECK(argc == ARG_NUM + 1, "Number of arguments should be 3", rv = INVALID_ARGS_NUM, exit)

    char *endOfNumber;
    unsigned long long maxIters = strtoull(argv[1], &endOfNumber, 0);
    CHECK(errno != ERANGE, "First argument is not an unsigned integer or out of range", rv = INVALID_ARG, exit)
    CHECK(strIsSpace(endOfNumber), "There are extra chars in first argument which should be integer", rv = INVALID_ARG,
          exit)

    unsigned long initAddr = strtoul(argv[3], &endOfNumber, 0);
    CHECK(errno != ERANGE, "Third argument is not an unsigned integer or out of range", rv = INVALID_ARG, exit)
    CHECK(strIsSpace(endOfNumber), "There are extra chars in third argument which should be integer", rv = INVALID_ARG,
          exit)
    CHECK(initAddr < ADDR_NUM, "Third argument should not be greater than 2^15-1", rv = INVALID_ARG, exit)
    CHECK(initAddr % 2 == 0, "initial address should be aligned", rv = UNALIGNED_INIT_ADDR, exit)

    const char *inPath = argv[2];

    FILE *fp;
    CHECK_F(fp = fopen(inPath, "r"), "open", inPath, rv, CANT_OPEN_FILE, exit)

    long fileSize = getFileSize(fp);
    CHECK(initAddr + fileSize <= ADDR_NUM,
          "program can not be loaded to such address as it would get out of addressed range",
          rv = PROGRAM_OUT_OF_MEMORY,
          closeFile)

    CHECK_F(fread(mem + initAddr, sizeof(uint8_t), fileSize, fp) == fileSize,
            "read whole file", inPath, rv, READ_FAIL, closeFile)

    ROLLBACK_F(fclose(fp) == 0, "close", inPath, rv, CANT_CLOSE_FILE)

    struct VM vm;
    initVM(&vm, (uint16_t) initAddr);

    for(unsigned long long i = 0; i < maxIters; ++i) {
        const int code = handlers[mem[vm.IP + 1] >> 11](&vm);
    }

    closeFile:
    ROLLBACK_F(fclose(fp) == 0, "close", inPath, rv, CANT_CLOSE_FILE)

    exit:
    return rv;
}