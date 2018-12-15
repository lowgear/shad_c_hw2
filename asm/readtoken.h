#pragma once

#include <stdbool.h>
#include <stdio.h>

#include "utils/strtools.h"

bool ReadToken(FILE* ifp, char* str, unsigned int bufSize);
