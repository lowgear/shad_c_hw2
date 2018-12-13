#include <stdbool.h>
#include <string.h>
#include <malloc.h>

#define DEFINE_VECTOR(T) \
    struct V_##T { \
        size_t size; \
        size_t cnt; \
        T array[]; \
    }; \
    \
    typedef struct V_##T* V_##T##_Ptr;


#define ID(vec, id) ((vec)->array[(id)])
#define CNT(vec) ((vec)->cnt)
#define SIZE(vec) ((vec)->size)

#define ID_P(vec, id) ID(*vec, id)
#define CNT_P(vec) CNT(*vec)
#define SIZE_P(vec) SIZE(*vec)
#define STRUCT_SIZE(vec) (sizeof(vec) + (vec).size * sizeof((vec).array[0]))

#define INIT_VEC(vec, s, faultLabel) \
    do { \
        (vec) = malloc(sizeof((*(vec))) + (s) * sizeof(ID_P(&vec, 0))); \
        if ((vec) == NULL) \
            goto faultLabel; \
        (vec)->size = s; \
        (vec)->cnt = 0; \
    } while (0);

#define PUSH_BACK_P(vecP, val, faultLabel) \
    do { \
        if (CNT_P(vecP) == SIZE_P(vecP)) { \
            __typeof__(*vecP) newVec = realloc(*vecP, STRUCT_SIZE(**vecP) + SIZE_P(vecP)); \
            if (newVec == NULL) \
                goto faultLabel; \
            *vecP = newVec; \
        } \
        ID_P(vecP, CNT_P(vecP)) = (val); \
        ++CNT_P(vecP); \
    } while (0);
