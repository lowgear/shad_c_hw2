#pragma once

#include <stdbool.h>

#include "utils/vector_size_t.h"

struct LabelDesc {
    char *labelName;
    size_t labelId;
    V_size_t_Ptr instructionIds;
};

typedef struct LabelDesc *LabelDescPtr;
DEFINE_VECTOR(LabelDescPtr)

size_t FindLabDesc(V_LabelDescPtr_Ptr lds, const char *label);

bool AddLabDesc(V_LabelDescPtr_Ptr *lds, const char *label);

void FreeDesc(LabelDescPtr descP);
