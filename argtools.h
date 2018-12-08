#pragma once

#include <stdio.h>

#define CHECK_FALL(cond, ...) \
    if (!(cond)) \
    { \
        printf(__VA_ARGS__); \
        return 1; \
    }
