#ifndef ASTRA_LIB_H_
#define ASTRA_LIB_H_

#define eprintf(...) fprintf(stderr, __VA_ARGS__)
#define panic(...)            \
    do {                      \
        eprintf(__VA_ARGS__); \
        eprintf("\n");        \
        exit(1);              \
    } while (0)
#define panic_if(cond, ...)     \
    do {                        \
        if (cond) {             \
            panic(__VA_ARGS__); \
        }                       \
    } while (0)
#define expect(cond, ...)               \
    do {                                \
        panic_if(!(cond), __VA_ARGS__); \
    } while (0)

#define unwrap_en(cond, ...)                                           \
    do {                                                               \
        if (cond) {                                                    \
            panic(__VA_ARGS__ __VA_OPT__(": ") "%s", strerror(errno)); \
        }                                                              \
    } while (0)

#ifndef max
    #define max(a, b) ((a) > (b) ? (a) : (b))
#endif // max

#endif // ASTRA_LIB_H_
