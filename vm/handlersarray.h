#pragma once

#include <inttypes.h>

#include "vmstruct.h"

#define HANDLERS_NUM (32)

extern int (*handlers[HANDLERS_NUM])(struct VM*, uint16_t instr);
