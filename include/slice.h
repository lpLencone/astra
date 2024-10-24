#ifndef ASTRA_SLICE_H_
#define ASTRA_SLICE_H_

#include <stddef.h>
#include <stdint.h>

typedef struct {
    char const *data;
    size_t length;
} Slice;

#define SLICE_NULL (Slice) { .data = NULL, .length = 0 }
#define SLICEFMT(s) (int) (s).length, (s).data

Slice slice_make(char const *begin, size_t length);
Slice slice_from_bounds(char const *begin, char const *end);
#define slice(begin, arg)                                                                \
    _Generic(arg, int: slice_make, size_t: slice_make, char const *: slice_from_bounds)( \
            begin, arg)
#define slice_cstr(cstr) slice_make(cstr, strlen(cstr))

int slice_cmp(Slice a, Slice b);
#define slice_cmp_cstr(s, cstr) slice_cmp(s, slice_cstr(cstr))
bool slice_eq(Slice a, Slice b);
#define slice_eq_cstr(s, cstr) slice_eq(s, slice_cstr(cstr))
bool slice_isnull(Slice s);

bool slice_predicate(Slice s, int (*predicate)(int));
#define slice_isalnum(s) slice_predicate(s, isalnum)
#define slice_isdigit(s) slice_predicate(s, isdigit)

size_t slice_usize(Slice s, Slice *endslice);
uint64_t slice_uint64(Slice s, Slice *endslice);

Slice slice_triml(Slice s, char const *targets);
Slice slice_token(Slice *s, char const *delimiter);

#endif // ASTRA_SLICE_H_
