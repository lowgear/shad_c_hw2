#pragma once

enum RetCodes {
    RETURN = 0,
    CONTINUE = 1,
    DBZ = 2,
    UNALIGNED_STACK_ACCESS = 3,
    UNALIGNED_MEM_ACCESS = 4,
    OVERFLOW = 5,
    IO_ERROR = 6
};
