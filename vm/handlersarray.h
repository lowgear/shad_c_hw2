#pragma once

#include "vmstruct.h"

const size_t HANDLERS_NUM = 32;

int (*handlers[HANDLERS_NUM])(struct VM*);
