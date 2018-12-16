#include "vmstruct.h"

const size_t REG_NUM = 8;
const size_t SP_ID = 7;

void initVM(struct VM* vm, uint16_t initAddr) {
    for (size_t i = 0; i < REG_NUM; ++i) {
        vm->R[i] = 0;
    }
    vm->R[SP_ID] = initAddr;
    vm->IP = initAddr;
}
