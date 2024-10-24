#ifndef ASTRA_LIB_H_
#define ASTRA_LIB_H_

#define eprintf(fmt, ...) fprintf(stderr, fmt __VA_OPT__(, ) __VA_ARGS__)
#define panic(fmt, ...)                                      \
    do {                                                     \
        eprintf("%s::%s::%d::" fmt "\n", __FILE__, __func__, \
                __LINE__ __VA_OPT__(, ) __VA_ARGS__);        \
        exit(1);                                             \
    } while (0)
#define expect(cond, fmt, ...)                                                \
    do {                                                                      \
        if (!(cond)) {                                                        \
            panic("Expected \"" #cond "\": " fmt __VA_OPT__(, ) __VA_ARGS__); \
        }                                                                     \
    } while (0)

#define unwrap_en(cond, fmt, ...)                                          \
    do {                                                                   \
        if (cond) {                                                        \
            panic(fmt ": %s" __VA_OPT__(, ) __VA_ARGS__, strerror(errno)); \
        }                                                                  \
    } while (0)

#ifndef max
    #define max(a, b) ((a) > (b) ? (a) : (b))
#endif // max

#endif // ASTRA_LIB_H_
