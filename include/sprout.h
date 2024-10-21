#ifndef ASTRA_SPROUT_H_
#define ASTRA_SPROUT_H_

#include <stddef.h>

typedef struct {
    size_t capacity;
    size_t length;
    char *data;
} Sprout;

void sprout_pluck(Sprout *s);

void sprout_insert(Sprout *s, size_t index, char const *data, size_t length);
#define sprout_insert_cstr(s, index, cstr) sprout_insert(s, index, cstr, strlen(cstr))
#define sprout_push(s, data, len) sprout_insert(s, (s)->length, data, len)
#define sprout_push_cstr(s, cstr) sprout_push(s, cstr, strlen(cstr))

void sprout_insert_fmt(Sprout *s, size_t index, char const *fmt, ...);
#define sprout_push_fmt(s, fmt, ...) sprout_insert_fmt(s, (s)->length, fmt, __VA_ARGS__)

#endif // ASTRA_SPROUT_H_
