#include "sprout.h"

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "lib.h"

#define SPROUT_INIT_CAP 8
static void sprout_grow(Sprout *s, size_t size);

void sprout_pluck(Sprout *s)
{
    if (s->capacity > 0) {
        free(s->data);
    }
    *s = (Sprout) {0};
}

void sprout_insert(Sprout *s, size_t index, char const *data, size_t length)
{
    expect(index <= s->length,
           "Index to insert `data` at should be within the sprout's bounds.");
    sprout_grow(s, length);
    memmove(s->data + index + length, s->data + index, s->length - index);
    memcpy(s->data + index, data, length);
    s->length += length;
    s->data[s->length] = '\0';
}

void sprout_insert_fmt(Sprout *s, size_t index, char const *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int fmtlen = vsnprintf(NULL, 0, fmt, ap);
    unwrap_en(fmtlen < 0, "vsnprintf");
    va_end(ap);

    va_start(ap, fmt);
    char *buffer = malloc(fmtlen + 1);
    vsnprintf(buffer, fmtlen + 1, fmt, ap);
    va_end(ap);
    
    sprout_insert(s, index, buffer, fmtlen);

    free(buffer);
}

static void sprout_grow(Sprout *s, size_t size)
{
    size_t cap = s->capacity;
    if (cap == 0) {
        cap = SPROUT_INIT_CAP;
    }
    while (cap < s->length + size + 1) { // 1 byte for '\0'
        cap *= 2;
    }
    if (cap != s->capacity) {
        s->capacity = cap;
        s->data = realloc(s->data, s->capacity);
    }
}
