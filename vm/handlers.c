#include <stdbool.h>
#include <stdio.h>

#include "handlers.h"
#include "vmstruct.h"
#include "inst_return_codes.h"

void set16AtAddr(struct VM* vm, uint16_t addr, uint16_t value) {
    vm->mem[addr] = (uint8_t) value;
    vm->mem[addr + 1] = (uint8_t) (value >> 8);
}

int movRR(struct VM *vm, uint16_t instr) {
    uint8_t to = GET_ARG(instr, 3, 3);
    uint8_t from = GET_ARG(instr, 3, 0);

    vm->R[to] = vm->R[from];

    return CONTINUE;
}

int movRRp(struct VM *vm, uint16_t instr) {
    uint8_t to = GET_ARG(instr, 3, 3);
    uint8_t from = GET_ARG(instr, 3, 0);

    uint16_t addr = vm->R[from];
    if (addr & 1) {
        return UNALIGNED_MEM_ACCESS;
    }

    vm->R[to] = GET(vm, addr);

    return CONTINUE;
}

int movRpR(struct VM *vm, uint16_t instr) {
    uint8_t to = GET_ARG(instr, 3, 3);
    uint8_t from = GET_ARG(instr, 3, 0);

    uint16_t addr = vm->R[to];
    if (addr & 1) {
        return UNALIGNED_MEM_ACCESS;
    }

    set16AtAddr(vm, addr, vm->R[from]);

    return CONTINUE;
}

int movRsI(struct VM *vm, uint16_t instr) {
    uint8_t to = GET_ARG(instr, 4, 8);
    uint16_t imm8 = GET_ARG(instr, 8, 0);

    uint8_t rId =to >> 1;
    if (to & 1U) {
        vm->R[rId] = (uint16_t) ((vm->R[rId] & 255U) | (imm8 << 8));}
    else {
        vm->R[rId] = (uint16_t) ((vm->R[rId] & (255U << 8)) | imm8);
    }

    return CONTINUE;
}

int push(struct VM *vm, uint16_t instr) {
    if (vm->R[SP_ID] & 1)
        return UNALIGNED_STACK_ACCESS;

    if (vm->R[SP_ID] < 2)
        return OVERFLOW;

    vm->R[SP_ID] -= 2;

    uint8_t from = GET_ARG(instr, 3, 0);
    uint16_t val = vm->R[from];

    set16AtAddr(vm, vm->R[SP_ID], val);

    return CONTINUE;
}

int pop(struct VM *vm, uint16_t instr) {
    if (vm->R[SP_ID] & 1)
        return UNALIGNED_STACK_ACCESS;

    if (vm->R[SP_ID] > UINT16_MAX - 2)
        return OVERFLOW;

    uint8_t to = GET_ARG(instr, 3, 0);
    uint16_t val = GET(vm, vm->R[SP_ID]);

    vm->R[SP_ID] -= 2;

    vm->R[to] = val;

    return CONTINUE;
}

int call(struct VM *vm, uint16_t instr) {
    if (vm->R[SP_ID] < 2)
        return OVERFLOW;
    if (vm->R[SP_ID] & 1)
        return UNALIGNED_STACK_ACCESS;
    int32_t newIP = vm->IP - 2 + (int8_t)GET_ARG(instr, 8, 0);
    if (newIP < 0 || newIP > ADDR_NUM)
        return OVERFLOW;
    if (newIP & 1)
        return UNALIGNED_MEM_ACCESS;

    set16AtAddr(vm, vm->R[SP_ID], vm->IP);
    vm->R[SP_ID] -= 2;
    vm->IP = (uint16_t) newIP;

    return CONTINUE;
}

int ret(struct VM *vm, uint16_t instr) {
    if (vm->R[SP_ID] & 1)
        return UNALIGNED_STACK_ACCESS;
    if (vm->R[SP_ID] == UINT16_MAX - 1)
        return OVERFLOW;
    uint16_t newIP = GET(vm, vm->R[SP_ID]);
    if (newIP & 1)
        return UNALIGNED_MEM_ACCESS;

    vm->R[SP_ID] += 2;
    vm->IP = newIP;

    return CONTINUE;
}

#define BIN_OV_OP(op) do { \
    uint64_t a = GET_ARG(instr, 3, 3); \
    uint64_t b = GET_ARG(instr, 3, 0); \
    int64_t newVal = vm->R[a] op vm->R[b]; \
    if (newVal < 0 || UINT16_MAX < newVal) \
        return OVERFLOW; \
    vm->R[a] = newVal; \
    return CONTINUE; \
} while(0);

#define BIN_OP(op) do { \
    uint8_t a = GET_ARG(instr, 3, 3); \
    uint8_t b = GET_ARG(instr, 3, 0); \
    int16_t newVal = vm->R[a] op vm->R[b]; \
    vm->R[a] = (uint16_t) newVal; \
    return CONTINUE; \
} while(0);

int add(struct VM *vm, uint16_t instr) {
    BIN_OV_OP(+)
}

int sub(struct VM *vm, uint16_t instr) {
    BIN_OV_OP(-)
}

int mul(struct VM *vm, uint16_t instr) {
    BIN_OV_OP(*)
}

int _div(struct VM *vm, uint16_t instr) {
    uint8_t a = GET_ARG(instr, 3, 3);
    uint8_t b = GET_ARG(instr, 3, 0);
    if (vm->R[b] == 0)
        return DBZ;
    int64_t newVal = vm->R[a] / vm->R[b];
    vm->R[a] = (uint16_t) newVal;
    return CONTINUE;
}

int _and(struct VM *vm, uint16_t instr) {
    BIN_OP(&)
}

int _or(struct VM *vm, uint16_t instr) {
    BIN_OP(|)
}

int _xor(struct VM *vm, uint16_t instr) {
    BIN_OP(^)
}

int _not(struct VM *vm, uint16_t instr) {
    uint8_t a = GET_ARG(instr, 3, 0);
    vm->R[a] = ~vm->R[a];

    return CONTINUE;
}

int shl(struct VM *vm, uint16_t instr) {
    uint8_t a = GET_ARG(instr, 3, 4);
    uint8_t imm4 = GET_ARG(instr, 4, 0);
    vm->R[a] <<= imm4;

    return CONTINUE;
}

int shr(struct VM *vm, uint16_t instr) {
    uint8_t a = GET_ARG(instr, 3, 4);
    uint8_t imm4 = GET_ARG(instr, 4, 0);
    vm->R[a] >>= imm4;

    return CONTINUE;
}

int reset(struct VM *vm, uint16_t instr) {
    return RETURN;
}

int nop(struct VM *vm, uint16_t instr) {
    return CONTINUE;
}

int jmp(struct VM *vm, uint16_t instr) {
    int32_t newIP = vm->IP - 2 + (int8_t)GET_ARG(instr, 8, 0);
    if (newIP < 0 || newIP > ADDR_NUM)
        return OVERFLOW;
    if (newIP & 1)
        return UNALIGNED_MEM_ACCESS;

    vm->IP = (uint16_t) newIP;

    return CONTINUE;
}

int je(struct VM *vm, uint16_t instr) {
    uint8_t a = GET_ARG(instr, 3, 8);
    if (vm->R[a] != 0)
        return CONTINUE;

    int32_t newIP = vm->IP - 2 + (int8_t)GET_ARG(instr, 8, 0);
    if (newIP < 0 || newIP > ADDR_NUM)
        return OVERFLOW;
    if (newIP & 1)
        return UNALIGNED_MEM_ACCESS;

    vm->IP = (uint16_t) newIP;

    return CONTINUE;
}

int jne(struct VM *vm, uint16_t instr) {
    uint8_t a = GET_ARG(instr, 3, 8);
    if (vm->R[a] == 0)
        return CONTINUE;

    int32_t newIP = vm->IP - 2 + (int8_t)GET_ARG(instr, 8, 0);
    if (newIP < 0 || newIP > ADDR_NUM)
        return OVERFLOW;
    if (newIP & 1)
        return UNALIGNED_MEM_ACCESS;

    vm->IP = (uint16_t) newIP;

    return CONTINUE;
}

int in(struct VM *vm, uint16_t instr) {
    uint8_t a = GET_ARG(instr, 4, 0);

    bool high = (bool) (a & 1U);
    a >>= 1;
    uint8_t value = (uint8_t) getchar();
    if (high) {
        vm->R[a] = (uint16_t) ((vm->R[a] & UINT8_MAX) | (value << 8));
    } else {
        vm->R[a] = (uint16_t) ((vm->R[a] & (UINT8_MAX << 8)) | value);
    }

    return CONTINUE;
}

int out(struct VM *vm, uint16_t instr) {
    uint8_t a = GET_ARG(instr, 4, 0);

    bool high = (bool) (a & 1U);
    a >>= 1;
    if (high) {
        if (putchar(vm->R[a] >> 8) == EOF)
            return IO_ERROR;
    } else {
        if (putchar(vm->R[a]) == EOF)
            return IO_ERROR;
    }

    return CONTINUE;
}