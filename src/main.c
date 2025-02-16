// DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
// Version 1.0, February 2025
// Copyright (C) 2025 Lucas
// Everyone is permitted to copy, distribute, modify, merge, publish, sell, sublicense, or do whatever the fuck they want with this software, under the following conditions:
// You just DO WHAT THE FUCK YOU WANT TO.
// NO WARRANTY IS PROVIDED, EXPRESS OR IMPLIED. THE AUTHOR IS NOT RESPONSIBLE FOR ANY CONSEQUENCES RESULTING FROM THE USE, MODIFICATION, OR DISTRIBUTION OF THIS SOFTWARE. USE IT AT YOUR OWN RISK.

#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <assert.h>

#include "include/slice.h"
#include "include/error.h"

#define STB_DS_IMPLEMENTATION
#include "include/stb_ds.h"

#ifdef _WIN32
#   define DELIM "\\"
#else
#   define DELIM "/"
#endif

#define NFILES    (2)
#define BUFCAP    (1024)
#define PREF_DIV  ("<div")
#define PREF_HTML ("<html>")

#define FLAG_GETLIST1 ("--get-list-1")
#define FLAG_GETLIST2 ("--get-list-2")
#define FLAG_DETAHELP ("--detailed-help")

typedef struct {
    FILE *stream;
    bool print;
} ListOut;

typedef struct {
    ListOut lo[NFILES];
    const char *program;
    const char *paths[NFILES];
} Args;

Args args_parse(int *argc, char ***argv);
char *arg_shift(int *argc, char ***argv);

typedef struct {
    char *buf;
    size_t len;
    size_t cap;
} Buf;

Buf buf_from_file_raw(const char *file_contents, size_t file_len, ListOut lo);
Buf buf_from_file_html(const char *file_contents, size_t file_len, ListOut lo);
Buf buf_from_file_div(const char *file_contents, size_t file_len, ListOut lo);
bool bufput(Buf *buf, Slice slice);
char *bufapp(Buf *buf, Slice slice);
void buffree(Buf *buf);

typedef struct {
    char *key;
    int value;
} Dict;

Dict *dict_from_buf(Buf buf);

void usage(void);
void detahelp(void);
void elog(Error error);
void quit(int code);

size_t compar_to_file_raw(Dict *dict, const char *file_contents, size_t file_len);
size_t compar_to_file_html(Dict *dict, const char *file_contents, size_t file_len);
size_t compar_to_file_div(Dict *dict, const char *file_contents, size_t file_len);
char *file_read(const char *filePath, size_t *readedLen, Error *error);
bool starts_with(const char *buffer, size_t len, char *prefix, size_t plen);
size_t maxst(size_t a, size_t b);

// iDiff - Instagram User and List Difference Finder
// 
// iDiff is a tool for comparing two lists of usernames or general items. 
// It identifies elements that appear in the second list but not in the first. 
// Additionally, it can parse Instagram `<div>` elements to extract usernames automatically.
// 
// Features:
// - Compare two lists of usernames or general items.
// - Extract usernames from Instagram HTML snippets.
// - Display missing users/items in a clean format.
// 
// Uses the stb_ds.h library for hash tables.

int main(int argc, char **argv) {
    
    Args args = args_parse(&argc, &argv);
    if(!args.paths[0] || !args.paths[1]) {usage(); quit(1);}
    
    Error error;
    size_t file_len;
    char *file_contents = file_read(args.paths[0], &file_len, &error);
    if(!file_contents) elog(error);
    
    Buf buf;
    if(starts_with(file_contents, file_len, PREF_HTML, strlen(PREF_HTML))) buf = buf_from_file_html(file_contents, file_len, args.lo[0]);
    else if(starts_with(file_contents, file_len, PREF_DIV, strlen(PREF_DIV))) buf = buf_from_file_div(file_contents, file_len, args.lo[0]);
    else buf = buf_from_file_raw(file_contents, file_len, args.lo[0]);
    free(file_contents);

    Dict *dict = dict_from_buf(buf);
    file_contents = file_read(args.paths[1], &file_len, &error);
    if(!file_contents) elog(error);
    
    size_t count;
    printf("===================NOT MATCHING LIST===================\n");
    if(starts_with(file_contents, file_len, PREF_HTML, strlen(PREF_HTML))) count = compar_to_file_html(dict, file_contents, file_len);
    else if(starts_with(file_contents, file_len, PREF_DIV, strlen(PREF_DIV))) count = compar_to_file_div(dict, file_contents, file_len);
    else count = compar_to_file_raw(dict, file_contents, file_len);
    printf("=================NOT MATCHING LIST END=================\n");
    printf("Total mismatches = %zu\n", count);

    free(file_contents);
    shfree(dict);
    buffree(&buf);
    for(size_t i = 0; i < NFILES; ++i) if(args.lo[i].stream) fclose(args.lo[i].stream);
    quit(0);
    return 0;
}

void quit(int code) {
    printf("\n<exit code %d>\n", code);
    exit(code);
}

void detahelp(void) {
    printf("\nThis program tells the USER wich instances appear in file2 but not in file1\n");
    printf("Supports the following formats of file:\n");
    printf("  - Either a list of users (raw). Each line of the file corresponds to a user, for example\n");
    printf("      instance1\n");
    printf("      instance2\n");
    printf("      instance3\n");
    printf("      ...\n");
    printf("    This list can also be prefixed with the following\n");
    printf("      [%%s]>> instance1\n");
    printf("      [%%s]>> instance2\n");
    printf("      [%%s]>> instance3\n");
    printf("      ...\n");
    printf("  - Or the <div> element containing the users (specific for Instagram). Go to Instagram, inspect the your followers/ing\n");
    printf("    and copy the hole div that contains them, just them (loading them first is required).\n");
    printf("    Read the README.md file for more detail\n");
}

void usage(void) {
    printf(">> Usage:\n");
    printf("."DELIM"idiff <file1> <file2> [%s] [%s] [%s]\n\n", FLAG_GETLIST1, FLAG_GETLIST2, FLAG_DETAHELP);
    printf("  %s    print the help message\n", FLAG_DETAHELP);
    printf("  %s       get the list 1 of instances\n", FLAG_GETLIST1);
    printf("  %s       get the list 2 of instances\n", FLAG_GETLIST2);
}

bool starts_with(const char *buffer, size_t len, char *prefix, size_t plen) {
    if(len < plen) return false;
    for(size_t i = 0; i < plen; ++i) if(prefix[i] != buffer[i]) return false;
    return true;
}

char *arg_shift(int *argc, char ***argv) {
    char *arg = NULL;
    if(*argc > 0) {
        arg = **argv;
        --(*argc); 
        ++(*argv);
    } return arg;
}

char *file_read(const char *filePath, size_t *readedLen, Error *error) {
    if(error) *error = ((Error){0});
    FILE *f = fopen(filePath, "rb");
    char *buffer = NULL;
    if(f) {
        if(fseek(f, 0, SEEK_END) == 0) {
            size_t len = ftell(f);
            rewind(f);
            buffer = malloc(sizeof(char) * (len + 1));
            if(buffer == NULL) __esettgoto(error, ERROR_MALLOC, defer1);
            size_t readed_len;
            if((readed_len = fread(buffer, sizeof(char), len, f)) == len) {
                buffer[len] = '\0';
                if(readedLen) *readedLen = len;
            } else __esetgoto(error, ERROR_FREAD, defer2);
        } else __esetgoto(error, ERROR_FSEEK, defer1);
        fclose(f);
    } else eset(error, ERROR_OPENING_FILE, filePath, strerror(errno));
    return buffer;
    defer2:
        free(buffer);
        buffer = NULL;
    defer1:
        fclose(f);
        return buffer;
}

Args args_parse(int *argc, char ***argv) {
    Args args = {0};
    args.program = arg_shift(argc, argv);
    while(*argc > 0) {
        Slice arg = snew(arg_shift(argc, argv));
        if(strcmp(arg.ptr, FLAG_DETAHELP) == 0) {usage(); detahelp(); quit(0);}
        else if(starts_with(arg.ptr, arg.len, FLAG_GETLIST1, strlen(FLAG_GETLIST1))) {
            args.lo[0].print = true;
            (void)sslice(&arg, '=', true);
            if(arg.len > 0) {
                args.lo[0].stream = fopen(arg.ptr, "wb");
                if(args.lo[0].stream == NULL) {fprintf(stderr, "[ERROR]: Unable to open %s : %s\n", arg.ptr, strerror(errno)); goto defer1;}
            }
        } else if(starts_with(arg.ptr, arg.len, FLAG_GETLIST2, strlen(FLAG_GETLIST2))) {
            args.lo[1].print = true;
            (void)sslice(&arg, '=', true);
            if(arg.len > 0) {
                args.lo[1].stream = fopen(arg.ptr, "wb");
                if(args.lo[1].stream == NULL) {fprintf(stderr, "[ERROR]: Unable to open %s : %s\n", arg.ptr, strerror(errno)); goto defer2;}
            }
        } else if(!args.paths[0]) args.paths[0] = arg.ptr;
        else args.paths[1] = arg.ptr;
    } 
    return args;
    defer2:
        if(args.lo[0].stream) fclose(args.lo[0].stream);
    defer1:
        quit(1);
        return (Args){0};
}

void elog(Error error) {
    fprintf(stderr, "[ERROR]: Couldn't read from file '%s' : %s\n", error.info[0], error.info[1]);
    fprintf(stderr, "[ERROR]: Got error '%s'\n", etochar(error.type));
    quit(1);
}

size_t maxst(size_t a, size_t b) {
    return a < b ? b : a;
}

char *bufapp(Buf *buf, Slice slice) {
    if(buf->cap < buf->len + slice.len + 1) {
        size_t newcap = buf->cap + maxst(BUFCAP, slice.len + 1);
        char *dst = realloc(buf->buf, sizeof(char) * newcap);
        if(dst == NULL) return NULL;
        buf->cap = newcap;
        buf->buf = dst;
    } 
    char *dst = buf->buf + (sizeof(char) * buf->len);
    memcpy(dst, slice.ptr, sizeof(char) * slice.len);
    buf->len += slice.len;
    buf->buf[buf->len++] = '\0';
    return dst;
}

void buffree(Buf *buf) {
    free(buf->buf);
    buf->buf = NULL;
    buf->cap = 0;
    buf->len = 0;
}

bool bufput(Buf *buf, Slice slice) {
    if(buf->cap < slice.len + 1) {
        size_t newcap = buf->cap + maxst(BUFCAP, slice.len + 1);
        char *dst = realloc(buf->buf, sizeof(char) * newcap);
        if(dst == NULL) return false;
        buf->cap = newcap;
        buf->buf = dst;
    } 
    memcpy(buf->buf, slice.ptr, sizeof(char) * slice.len);
    buf->len = slice.len;
    buf->buf[buf->len] = '\0';
    return true;
}

Dict *dict_from_buf(Buf buf) {
    Dict *dict = NULL;
    char *ptr = buf.buf;
    while(ptr < buf.buf + buf.len) {
        shput(dict, ptr, 1);
        ptr += strlen(ptr) + 1;
    } return dict;
}

Buf buf_from_file_raw(const char *file_contents, size_t file_len, ListOut lo) {
    Buf buf = {0};
    size_t count = 0;
    Slice slice = snewl(file_contents, file_len);
    while(slice.len > 0) {
        Slice name = sslice(&slice, '\n', true);
        Slice chop = sslices(&name, snew(">>"), true);
        if(!name.len) name = chop;
        name = strim(name);
        if(name.len > 0) {
            char *dst = bufapp(&buf, name);
            if(lo.print) fprintf(lo.stream ? lo.stream : stdout, "%zu >> %s\n", ++count, dst);
            if(!dst) {fprintf(stderr, "[ERROR]: Ran out of memory, list too long\n"); quit(1);}
        }
    } return buf;
}

Buf buf_from_file_html(const char *file_contents, size_t file_len, ListOut lo) {
    Buf buf = {0};
    size_t count = 0;
    Slice dcs = snew(".com/");
    Slice slice = snewl(file_contents, file_len);
    while(slice.len > 0) {
        (void)sslices(&slice, dcs, true);
        Slice name = strim(sslice(&slice, '"', true));
        if(name.len > 0) {
            char *dst = bufapp(&buf, name);
            if(lo.print) fprintf(lo.stream ? lo.stream : stdout, "%zu >> %s\n", ++count, dst);
            if(!dst) {fprintf(stderr, "[ERROR]: Ran out of memory, list too long\n"); quit(1);}
        }
    } return buf;
}

Buf buf_from_file_div(const char *file_contents, size_t file_len, ListOut lo) {
    Buf buf = {0};
    size_t count = 0;
    Slice aeq = snew("alt=\"");
    Slice slice = snewl(file_contents, file_len);
    while(slice.len > 0) {
        (void)sslices(&slice, aeq, true);
        Slice name = strim(sslice(&slice, '\'', true));
        if(name.len > 0) {
            char *dst = bufapp(&buf, name);
            if(lo.print) fprintf(lo.stream ? lo.stream : stdout, "%zu >> %s\n", ++count, dst);
            if(!dst) {fprintf(stderr, "[ERROR]: Ran out of memory, list too long\n"); quit(1);}
        }
    } return buf;
}

size_t compar_to_file_raw(Dict *dict, const char *file_contents, size_t file_len) {
    Buf buf = {0};
    size_t count = 0;
    Slice slice = snewl(file_contents, file_len);
    while(slice.len > 0) {
        Slice name = sslice(&slice, '\n', true);
        Slice chop = sslices(&name, snew(">>"), true);
        if(!name.len) name = chop;
        name = strim(name);
        if(name.len > 0) {
            bufput(&buf, name);
            if(shgeti(dict, buf.buf) == -1) printf("%zu >> %.*s\n", ++count, (int)name.len, name.ptr);
        }
    } buffree(&buf);
    return count;
}

size_t compar_to_file_html(Dict *dict, const char *file_contents, size_t file_len) {
    Buf buf = {0};
    size_t count = 0;
    Slice dcs = snew(".com/");
    Slice slice = snewl(file_contents, file_len);
    while(slice.len > 0) {
        (void)sslices(&slice, dcs, true);
        Slice name = strim(sslice(&slice, '"', true));
        if(name.len > 0) {
            bufput(&buf, name);
            if(shgeti(dict, buf.buf) == -1) printf("%zu >> %.*s\n", ++count, (int)name.len, name.ptr);
        }
    } buffree(&buf);
    return count;
}

size_t compar_to_file_div(Dict *dict, const char *file_contents, size_t file_len) {
    Buf buf = {0};
    size_t count = 0;
    Slice aeq = snew("alt=\"");
    Slice slice = snewl(file_contents, file_len);
    while(slice.len > 0) {
        (void)sslices(&slice, aeq, true);
        Slice name = strim(sslice(&slice, '\'', true));
        if(name.len > 0) {
            bufput(&buf, name);
            if(shgeti(dict, buf.buf) == -1) printf("%zu >> %.*s\n", ++count, (int)name.len, name.ptr);
        }
    } buffree(&buf);
    return count;
}