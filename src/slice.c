#include "slice.h"

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib.h"

Slice slice_make(char const *data, size_t length)
{
    return (Slice) { .data = data, .length = length };
}

Slice slice_from_bounds(char const *begin, char const *end)
{
    return slice_make(begin, end - begin);
}

bool slice_isnull(Slice s)
{
    return s.data == NULL || s.length == 0;
}

int slice_cmp(Slice a, Slice b)
{
    return strncmp(a.data, b.data, max(a.length, b.length));
}

bool slice_eq(Slice a, Slice b)
{
    return slice_cmp(a, b) == 0;
}

bool slice_predicate(Slice s, int (*predicate)(int))
{
    if (s.length == 0) {
        return false;
    }
    for (size_t i = 0; i < s.length; i++) {
        if (!predicate(s.data[i])) {
            return false;
        }
    }
    return true;
}

size_t slice_usize(Slice s, Slice *endslice)
{
    size_t result = 0;
    while (s.length > 0 && isdigit(*s.data)) {
        result *= 10;
        result += *s.data - '0';
        s.length--;
        s.data++;
    }
    if (endslice != NULL) {
        *endslice = s;
    }
    return result;
}

uint64_t slice_uint64(Slice s, Slice *endslice)
{
    return slice_usize(s, endslice);
}

Slice slice_triml(Slice s, char const *targets)
{
    if (s.length == 0) {
        return SLICE_NULL;
    }
    size_t length = strspn(s.data, targets);
    if (length > s.length) {
        length = s.length;
    }
    return slice(s.data + length, s.length - length);
}

Slice slice_token(Slice *s, char const *delimiter)
{
    if (s->length == 0) {
        return SLICE_NULL;
    }
    size_t token_length;
    for (token_length = 1;
         token_length < s->length && strchr(delimiter, s->data[token_length]) == NULL;
         token_length++);
    Slice token = slice(s->data, token_length);
    s->data += token_length;
    s->length -= token_length;
    return token;
}
