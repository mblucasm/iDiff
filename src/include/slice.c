#include "slice.h"
#include <ctype.h>
#include <string.h>

Slice snew(const char *ptr) {
    return (Slice) {
        .ptr = ptr,
        .len = strlen(ptr),
    };
}

Slice snewl(const char *ptr, size_t len) {
    return (Slice) {
        .ptr = ptr,
        .len = len,
    };
}

Slice sslice(Slice *s, const char delim, bool skipDelim) {
    size_t i = 0;
    while (i < s->len && s->ptr[i] != delim) ++i;
    Slice ret = snewl(s->ptr, i);
    s->ptr += i;
    s->len -= i;
    if(skipDelim && s->len != 0) {
        ++s->ptr;
        --s->len;
    } return ret;
}

bool sfind(Slice hayStack, Slice needle, size_t *idx) {
    if(needle.len > hayStack.len) return false;
    for(size_t i = 0; i < hayStack.len; ++i) {
        if(hayStack.ptr[i] == needle.ptr[0]) {
            if(needle.len > hayStack.len - i) return false;
            if(seq(snewl(hayStack.ptr + i, needle.len), needle)) {
                if(idx) *idx = i;
                return true;
            }
        }
    } return false;
}

bool seq(Slice s1, Slice s2) {
    if(s1.len != s2.len) return false;
    if(s1.len == 0) return true;
    return memcmp(s1.ptr, s2.ptr, sizeof(char) * s1.len) == 0;
}

Slice sslices(Slice *s, Slice delim, bool skipDelim) {
    size_t idx  = s->len;
    sfind(*s, delim, &idx);
    Slice rem = snewl(s->ptr, idx);
    s->ptr += idx;
    s->len -= idx;
    if(skipDelim && s->len >= delim.len) {
        s->ptr += delim.len;
        s->len -= delim.len;
    }
    return rem;
}

Slice striml(Slice s) {
    size_t i = 0;
    while(i < s.len && isspace(s.ptr[i])) ++i;
    return snewl(s.ptr + i, s.len - i);
}

Slice strimr(Slice s) {
    size_t i = 0;
    while(i < s.len && isspace(s.ptr[s.len - i - 1])) ++i;
    return snewl(s.ptr, s.len - i);
}

Slice strim(Slice s) {
    return striml(strimr(s));
}