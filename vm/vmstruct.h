#pragma once

const size_t REG_NUM;
const size_t SP_ID;

struct VM {
    union {
        uint16_t X;
        struct {
            uint8_t L;
            uint8_t H;
        } S;
    } R[8];
    uint16_t IP;
};

void initVM(struct VM* vm, uint16_t initAddr) {
    for (size_t i = 0; i < REG_NUM; ++i) {
        vm->R[i].X = 0;
    }
    vm->R[SP_ID].X = initAddr;
    vm->IP = initAddr;
}