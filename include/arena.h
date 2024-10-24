#ifndef ASTRA_ARENA_H_
#define ASTRA_ARENA_H_

#include <stddef.h>
#include <stdint.h>

typedef struct {
    size_t capacity;
    size_t count;
    uint8_t *data;
} Arena;

void arena_free(Arena *a /*, DeallocFn dealloc*/);

void *arena_alloc(Arena *a, size_t size);

#endif // ASTRA_ARENA_H_
