#include <string.h>


#define VERIFY_F(condition, operation, filename, rv, value, exit) \
do { \
    if (!(condition)) { \
        fprintf(stderr, "failed to %s %s: %s\n", \
            operation, filename, strerror(errno)); \
        rv = value; \
        exit; \
    } \
} while (0);


#define CHECK_F(condition, operation, filename, rv, value, label) \
VERIFY_F(condition, operation, filename, rv, value, goto label)


#define ROLLBACK_F(condition, operation, filename, rv, value) \
VERIFY_F(condition, operation, filename, rv, value, (void)0)

#define VERIFY(condition, message, exit) \
do { \
    if (!(condition)) { \
        fprintf(stderr, message); \
        exit; \
    } \
} while (0);

#define CHECK(condition, message, label) \
VERIFY(condition, message, goto label)


#define ROLLBACK(condition, operation, filename, rv, value) \
VERIFY(condition, message, (void)0)