#pragma once

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>

#include "utils/vectors.h"
#include "asm/error_check_tools.h"

struct LabelDesc {
    char *labelName;
    uint16_t address;
    V_size_t_Ptr instructionIds;
};

typedef struct LabelDesc *LabelDescPtr;
DEFINE_VECTOR(LabelDescPtr)

bool addLabelAddress(V_LabelDescPtr_Ptr* lds, char* label, uint16_t address) {
    size_t id = 0;
    while (id < CNT_P(lds) &&
           strcmp(ID_P(lds, id)->labelName, label) != 0)
        ++id;
    if (id >= CNT_P(lds)) {
        LabelDescPtr descPtr = malloc(sizeof(struct LabelDesc));
        CHECK(descPtr != NULL, "malloc fail", fault)

        PUSH_BACK_P(lds, descPtr, freeDesc)

        INIT_VEC(descPtr->instructionIds, 1, freeDesc)

        freeDesc:
        free(descPtr);
        goto fault;
    }

    ID_P(lds, id)->labelName = label;

    return true;

    fault:
    return false;
}
