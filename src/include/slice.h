#ifndef SLICE_H
#define SLICE_H

#include <stddef.h>
#include <stdbool.h>

typedef struct {
    const char *ptr;
    size_t len;
} Slice;

Slice snew(const char *ptr);
Slice snewl(const char *ptr, size_t len);
Slice sslice(Slice *s, const char delim, bool skipDelim);
Slice sslices(Slice *s, Slice delim, bool skipDelim);
Slice striml(Slice s);
Slice strimr(Slice s);
Slice strim(Slice s);
bool sfind(Slice hayStack, Slice needle, size_t *idx);
bool seq(Slice s1, Slice s2);

#endif // SLICE_H