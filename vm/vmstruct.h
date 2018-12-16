#pragma once

#include <stddef.h>
#include <inttypes.h>

extern const size_t REG_NUM;
extern const size_t SP_ID;
#define ADDR_NUM (1 << 16)

struct VM {
    uint16_t R[8];
    uint16_t IP;
    uint8_t mem[ADDR_NUM];
};

void initVM(struct VM *vm, uint16_t initAddr);
