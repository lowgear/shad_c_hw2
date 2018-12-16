#pragma once

#include <inttypes.h>

#include "vmstruct.h"

#define GET_ARG(instr, size, offset) ((uint8_t)(((instr) >> (offset)) & (~((~0ULL) << size))))

#define GET(vm, addr) ((uint16_t)((vm)->mem[addr] | (((uint16_t)((vm)->mem[(addr) + 1])) << 8)))

void set16AtAddr(struct VM *vm, uint16_t addr, uint16_t value);

int movRR(struct VM *vm, uint16_t instr);
int movRRp(struct VM *vm, uint16_t instr);
int movRpR(struct VM *vm, uint16_t instr);
int movRsI(struct VM *vm, uint16_t instr);
int push(struct VM *vm, uint16_t instr);
int pop(struct VM *vm, uint16_t instr);
int call(struct VM *vm, uint16_t instr);
int ret(struct VM *vm, uint16_t instr);
int add(struct VM *vm, uint16_t instr);
int sub(struct VM *vm, uint16_t instr);
int mul(struct VM *vm, uint16_t instr);
int _div(struct VM *vm, uint16_t instr);
int _and(struct VM *vm, uint16_t instr);
int _or(struct VM *vm, uint16_t instr);
int _xor(struct VM *vm, uint16_t instr);
int _not(struct VM *vm, uint16_t instr);
int shl(struct VM *vm, uint16_t instr);
int shr(struct VM *vm, uint16_t instr);
int reset(struct VM *vm, uint16_t instr);
int nop(struct VM *vm, uint16_t instr);
int jmp(struct VM *vm, uint16_t instr);
int je(struct VM *vm, uint16_t instr);
int jne(struct VM *vm, uint16_t instr);
int in(struct VM *vm, uint16_t instr);
int out(struct VM *vm, uint16_t instr);