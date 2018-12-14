#pragma once

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>

#include "utils/vectors.h"
#include "asm/error_check_tools.h"

struct LabelDesc {
    const char *labelName;
    size_t labelId;
    V_size_t_Ptr instructionIds;
};

typedef struct LabelDesc *LabelDescPtr;
DEFINE_VECTOR(LabelDescPtr)

size_t FindLabDesc(V_LabelDescPtr_Ptr lds, const char *label) {
    size_t id = 0;
    while (id < CNT(lds) &&
           strcmp(ID(lds, id)->labelName, label) != 0)
        ++id;
    return id;
}

bool AddLabDesc(V_LabelDescPtr_Ptr *lds, const char *label) {
    LabelDescPtr descPtr = malloc(sizeof(struct LabelDesc));
    CHECK(descPtr != NULL, "malloc fail", fault)

    char *labelCopy = malloc(sizeof(char) * (strlen(label) + 1));
    CHECK(labelCopy != NULL, "malloc fail", freeDesc)
    strcpy(labelCopy, label);

    descPtr->labelName = labelCopy;

    PUSH_BACK_P(lds, descPtr, freeLabelCopy)

    INIT_VEC(descPtr->instructionIds, 1, freeLabelCopy)

    return true;

    freeLabelCopy:
    free(labelCopy);

    freeDesc:
    free(descPtr);

    fault:
    return false;
}

bool AddLabelRef() {

}
