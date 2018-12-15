#include <stdint.h>
#include <string.h>

#include <stddef.h>

#include "labels.h"
#include "utils/error_check_tools.h"

size_t FindLabDesc(V_LabelDescPtr_Ptr lds, const char *label) {
    size_t id = 0;
    while (id < CNT(lds) &&
           strcmp(ID(lds, id)->labelName, label) != 0)
        ++id;
    return id;
}

bool AddLabDesc(V_LabelDescPtr_Ptr *lds, const char *label) {
    LabelDescPtr descPtr = malloc(sizeof(struct LabelDesc));
    CHECK(descPtr != NULL, "malloc fail", (void)0, fault)

    char *labelCopy = malloc(sizeof(char) * (strlen(label) + 1));
    CHECK(labelCopy != NULL, "malloc fail", (void)0, freeDesc)
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

void FreeDesc(LabelDescPtr descP) {
    free(descP->instructionIds);
    free(descP->labelName);
    free(descP);
}
