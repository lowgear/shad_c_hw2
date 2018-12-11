#define DEFINE_VECTOR(T) \
    struct Vector_##T { \
        size_t size; \
        T array[]; \
    };

