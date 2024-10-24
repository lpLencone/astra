#include "arena.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib.h"

#define ARENA_INIT_CAP (4 * 1024 * 1024) // 4 MiB

void arena_free(Arena *a /*, DeallocFn dealloc*/)
{
    if (a->capacity > 0) {
        free(a->data);
    }
}

static void arena_grow(Arena *a, size_t size);

void *arena_alloc(Arena *a, size_t size)
{
    arena_grow(a, size);
    a->count += size;
    return a->data + a->count - size;
}

static void arena_grow(Arena *a, size_t size)
{
    size_t cap = a->capacity;
    if (cap < ARENA_INIT_CAP) {
        cap = ARENA_INIT_CAP;
    }
    while (a->count + size > cap) {
        cap *= 2;
    }
    if (cap != a->capacity) {
        a->capacity = cap;
        a->data = realloc(a->data, a->capacity);
        expect(a->data != NULL, "Buy more RAM");
    }
}
