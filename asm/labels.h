#pragma once

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>

#include "utils/vectors.h"
#include "asm/error_check_tools.h"

struct LabelDesc {
    const char *labelName;
    int16_t address;
    V_size_t_Ptr instructionIds;
};

typedef struct LabelDesc *LabelDescPtr;
DEFINE_VECTOR(LabelDescPtr)

size_t FindLabDesc(const V_LabelDescPtr_Ptr *lds, const char *label) {
    size_t id = 0;
    while (id < CNT_P(lds) &&
           strcmp(ID_P(lds, id)->labelName, label) != 0)
        ++id;
    return id;
}

bool AddLabDesc(V_LabelDescPtr_Ptr *lds, const char *label) {
    LabelDescPtr descPtr = malloc(sizeof(struct LabelDesc));
    CHECK(descPtr != NULL, "malloc fail", fault)

    char *labelCopy = malloc(sizeof(char) * (strlen(label) + 1));
    CHECK(labelCopy != NULL, "malloc fail", freeDesc)

    descPtr->labelName = label;

    PUSH_BACK_P(lds, descPtr, freeLabelCopy)

    INIT_VEC(descPtr->instructionIds, 1, freeLabelCopy)

    freeLabelCopy:
    freeDesc:
    free(descPtr);
    goto fault;

    fault:
    return false;
}

bool AddLabelAddress(V_LabelDescPtr_Ptr *lds, const char *label, int16_t address) {
    size_t id = FindLabDesc(lds, label);

    if (id == CNT_P(lds)) {
        LabelDescPtr descPtr = malloc(sizeof(struct LabelDesc));
        CHECK(descPtr != NULL, "malloc fail", fault)
        ID_P(lds, id)->labelName = label;

        PUSH_BACK_P(lds, descPtr, freeDesc)

        INIT_VEC(descPtr->instructionIds, 1, freeDesc)

        freeDesc:
        free(descPtr);
        goto fault;
    }

    ID_P(lds, id)->address = address;

    return true;

    fault:
    return false;
}

bool AddLabelRef()
