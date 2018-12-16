#include "handlersarray.h"
#include "handlers.h"

int (*handlers[HANDLERS_NUM])(struct VM *, uint16_t instr) = {
        movRR,      // 0
        movRR,      // 1
        movRRp,     // 2
        movRRp,     // 3
        movRpR,     // 4
        movRpR,     // 5
        movRsI,     // 6
        movRsI,     // 7
        push,       // 8
        push,       // 9
        pop,        //10
        pop,        //11
        call,       //12
        call,       //13
        ret,        //14
        add,        //15
        sub,        //16
        mul,        //17
        _div,        //18
        _and,        //19
        _or,         //20
        _xor,        //21
        _not,        //22
        shl,        //23
        shr,        //24
        reset,      //25
        nop,        //26
        jmp,        //27
        je,         //28
        jne,        //29
        in,         //30
        out         //31
};
