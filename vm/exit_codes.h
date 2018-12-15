#pragma once

typedef const int CI;

CI IN_FILE_TYPE = 0;
CI OUT_FILE_TYPE = 1 << 10;

CI CANT_OPEN_FILE = 1;
CI UNKNOWN_COMMAND = 2;
CI CANT_WRITE_TO_FILE = 3;
CI CANT_CLOSE_FILE = 4;
CI ALLOC_FAIL = 5;
CI READ_FAIL = 7;
CI UNWNOWN_ERROR = 8;
CI RELATIVE_CALL_OUT_OF_RANGE = 9;
CI INVALID_ARGS_NUM = 10;
CI INVALID_ARG = 11;
CI PROGRAM_OUT_OF_MEMORY = 12;
CI UNALIGNED_INIT_ADDR = 13;
