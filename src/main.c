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
#include <stdarg.h>

#include "include/slice.h"
#include "include/error.h"

#define STB_DS_IMPLEMENTATION
#include "include/stb_ds.h"

#ifdef _WIN32
#   define DELIM "\\"
#   define DELIMC '\\'
#   define ANTIDELIMC '/'
#else
#   define DELIM "/"
#   define DELIMC '/'
#   define ANTIDELIMC '\\'
#endif

#define NFILES    (2)
#define BUFCAP    (1024)
#define PREF_DIV  ("<div")
#define PREF_HTML ("<html>")

#define DEFAULT_STREAM1 ("iDiff-list1.txt")
#define DEFAULT_STREAM2 ("iDiff-list2.txt")

#define FLAG_HELP     ("--help")
#define FLAG_DUMPLIST ("--dump-list-")
#define FLAG_DETAHELP ("--detailed-help")
#define FLAG_IFOLDER  ("--instagram-folder=")

#define FLAG_IFOLDERLEN  (sizeof(FLAG_IFOLDER) / sizeof(FLAG_IFOLDER[0]) - 1)
#define FLAG_DUMPLISTLEN (sizeof(FLAG_DUMPLIST) / sizeof(FLAG_DUMPLIST[0]) - 1)

#define IPATH ("/connections/followers_and_following/")
#define IPATHLEN (sizeof(IPATH) / sizeof(IPATH[0]) - 1)

typedef struct {
    bool ifolder;
    const char *program;
    const char *paths[NFILES];
    FILE *list_streams[NFILES];
} Args;

void args_parse(int *argc, char ***argv);
char *arg_shift(int *argc, char ***argv);

typedef struct {
    char *buf;
    size_t len;
    size_t cap;
} Buf;

#define bufcatS(bufp, ...) _bufcatS((bufp), __VA_ARGS__, (Slice){0})

Buf buf_from_file_raw  (const char *file_contents, size_t file_len, FILE *stream);
Buf buf_from_file_div  (const char *file_contents, size_t file_len, FILE *stream);
Buf buf_from_file_html (const char *file_contents, size_t file_len, FILE *stream);

bool bufputs  (Buf *buf, Slice slice);
bool bufcats  (Buf *buf, Slice slice);
bool _bufcatS (Buf *buf, ...);
char *bufapps (Buf *buf, Slice slice);
void buffree  (Buf *buf);

void args_get_paths_from_folder(const char *args_paths[NFILES], Slice folder);

typedef struct {
    char *key;
    int value;
} Dict;

Dict *dict_from_buf(Buf buf);

typedef enum {
    PREFIX_NONE,
    PREFIX_HTML,
    PREFIX_DIV,
} Prefix;

Prefix get_prefix(const char *buffer, size_t len);

size_t compar_to_file_raw(Dict *dict, const char *file_contents, size_t file_len, FILE *stream);
size_t compar_to_file_div(Dict *dict, const char *file_contents, size_t file_len, FILE *stream);
size_t compar_to_file_html(Dict *dict, const char *file_contents, size_t file_len, FILE *stream);

size_t maxst(size_t a, size_t b);
char *file_read(const char *filePath, size_t *readedLen, Error *error);
bool starts_with(const char *buffer, size_t len, char *prefix, size_t plen);
void div_ensure_english(Slice contents);
void usage(void);
void detahelp(void);
void elog(Error error);
void quit(int code);
void replacec(char *buffer, size_t len, const char old, const char new);

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

const char * const DEFAULT_STREAMS[2] = {DEFAULT_STREAM1, DEFAULT_STREAM2};
Args gargs = {0};

int main(int argc, char **argv) {

    args_parse(&argc, &argv);
    if(!gargs.paths[0] || !gargs.paths[1]) {usage(); quit(1);}
    
    Error error;
    size_t file_len;
    char *file_contents = file_read(gargs.paths[0], &file_len, &error);
    if(!file_contents) elog(error);
    
    Buf buf;
    Prefix pref = get_prefix(file_contents, file_len);
    switch(pref) {
        case PREFIX_NONE: buf = buf_from_file_raw(file_contents, file_len, gargs.list_streams[0]); break;
        case PREFIX_HTML: buf = buf_from_file_html(file_contents, file_len, gargs.list_streams[0]); break;
        case PREFIX_DIV:  buf = buf_from_file_div(file_contents, file_len, gargs.list_streams[0]); break;
    } free(file_contents);

    Dict *dict = dict_from_buf(buf);
    file_contents = file_read(gargs.paths[1], &file_len, &error);
    if(!file_contents) elog(error);
    
    printf("===================NOT MATCHING LIST===================\n");
    size_t count = 0;
    pref = get_prefix(file_contents, file_len);
    switch(pref) {
        case PREFIX_NONE: count = compar_to_file_raw(dict, file_contents, file_len, gargs.list_streams[1]); break;
        case PREFIX_HTML: count = compar_to_file_html(dict, file_contents, file_len, gargs.list_streams[1]); break;
        case PREFIX_DIV:  count = compar_to_file_div(dict, file_contents, file_len, gargs.list_streams[1]); break;
    }
    printf("=================NOT MATCHING LIST END=================\n");
    printf("Total mismatches = %zu\n", count);

    free(file_contents);
    shfree(dict);
    buffree(&buf);
    for(size_t i = 0; i < NFILES; ++i) if(gargs.list_streams[i]) fclose(gargs.list_streams[i]);
    if(gargs.ifolder) for(size_t i = 0; i < NFILES; ++i) free((char*)gargs.paths[i]);
    quit(0);
    return 0;
}

void quit(int code) {
    printf("\n<exit code %d>\n", code);
    exit(code);
}

void detahelp(void) {
    printf("\nThis program tells the USER wich instances appear in file2 but not in file1\n");
    printf("Supports the following formats of file:\n\n");
    printf("  - Either a list of users (raw). Each line of the file corresponds to a user, for example\n");
    printf("      instance1\n");
    printf("      instance2\n");
    printf("      instance3\n");
    printf("      ...\n");
    printf("    This list can also be prefixed with the following\n");
    printf("      [%%s]>> instance1\n");
    printf("      [%%s]>> instance2\n");
    printf("      [%%s]>> instance3\n");
    printf("      ...\n\n");
    printf("  - The <html> of followers/ing (specific for Instagram). Go to Instagram, and download your data\n");
    printf("    You don't need to dowload all of your data, you can specify only followers/ing and a time period (All time recommended)\n\n");
    printf("  - The <div> element containing the users (specific for Instagram). Go to Instagram, inspect your followers/ing\n");
    printf("    and copy the hole div that contains them, just them (loading them first is required).\n");
    printf("    Read the README.md file for more detail\n");
}

void usage(void) {
    printf(">> Usage:\n");
    printf("."DELIM"idiff <<file1> <file2> | %s<folder_path>> [options]\n\n", FLAG_IFOLDER);
    printf("Options:\n");
    printf("  %s             print this help message and quit\n", FLAG_HELP);
    printf("  %s    print the detailed help message and quit\n", FLAG_DETAHELP);
    printf("  %s1      dump the list 1 of instances to %s, if followed by =<file> (%s1=<file>) to a custom file \n", FLAG_DUMPLIST, DEFAULT_STREAMS[0], FLAG_DUMPLIST);
    printf("  %s2      dump the list 2 of instances to %s, if followed by =<file> (%s2=<file>) to a custom file \n", FLAG_DUMPLIST, DEFAULT_STREAMS[1], FLAG_DUMPLIST);
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

FILE *get_list_stream(Slice arg, const char *default_stream) {
    const char *path = default_stream;
    if(FLAG_DUMPLISTLEN + 2 < arg.len) {
        if(arg.ptr[FLAG_DUMPLISTLEN + 1] == '=') path = arg.ptr + FLAG_DUMPLISTLEN + 2;
        else {
            fprintf(stderr, "[ERROR]: %sX can only be followed by '=', got '%c' instead\n", FLAG_DUMPLIST, arg.ptr[FLAG_DUMPLISTLEN + 1]);
            quit(1);
        }
    } else if(FLAG_DUMPLISTLEN + 2 == arg.len) {
        if(arg.ptr[FLAG_DUMPLISTLEN + 1] == '=') fprintf(stderr, "[ERROR]: Expected a path after the '=' in %.*s\n", (int)arg.len, arg.ptr);
        else fprintf(stderr, "[ERROR]: %sX can only be followed by '=', got '%c' instead\n", FLAG_DUMPLIST, arg.ptr[FLAG_DUMPLISTLEN + 1]);
        quit(1);
    }
    FILE *f = fopen(path, "wb");
    if(f == NULL) {
        fprintf(stderr, "[ERROR]: Coundn't open file '%s' that was specified as a path with %.*s\n", path, (int)arg.len, arg.ptr);
        fprintf(stderr, "[ERROR]: The reason is: '%s'\n", strerror(errno));
    } return f;
}

void replacec(char *buffer, size_t len, const char old, const char new) {
    for(size_t i = 0; i < len; ++i) if(buffer[i] == old) buffer[i] = new;
}

void args_get_paths_from_folder(const char *args_paths[NFILES], Slice folder) {
    while(folder.len > 1 && (folder.ptr[folder.len - 1] == DELIMC || folder.ptr[folder.len - 1] == ANTIDELIMC)) --folder.len;
    Buf buf1 = {0};
    Buf buf2 = {0};
    bufcatS(&buf1, folder, snewl(IPATH, IPATHLEN), snew("followers_1.html"));
    bufcatS(&buf2, folder, snewl(IPATH, IPATHLEN), snew("following.html"));
    replacec(buf1.buf, buf1.len, ANTIDELIMC, DELIMC);
    replacec(buf2.buf, buf2.len, ANTIDELIMC, DELIMC);
    args_paths[0] = buf1.buf;
    args_paths[1] = buf2.buf;
}

void args_parse(int *argc, char ***argv) {
    gargs = (Args){0};
    gargs.program = arg_shift(argc, argv);
    while(*argc > 0) {
        Slice arg = snew(arg_shift(argc, argv));
        if(strcmp(arg.ptr, FLAG_HELP) == 0) {usage(); quit(0);}
        else if(strcmp(arg.ptr, FLAG_DETAHELP) == 0) {usage(); detahelp(); quit(0);}
        else if(starts_with(arg.ptr, arg.len, FLAG_IFOLDER, FLAG_IFOLDERLEN)) {
            gargs.ifolder = true;
            (void)sslice(&arg, '=', true);
            if(arg.len > 0) args_get_paths_from_folder(gargs.paths, arg);
            else {
                fprintf(stderr, "[ERROR]: '%s' flag NEEDS to be followed by a folder path\n", FLAG_IFOLDER);
                quit(1);
            }
        } else if(starts_with(arg.ptr, arg.len, FLAG_DUMPLIST, FLAG_DUMPLISTLEN)) {
            if(!(arg.len == FLAG_DUMPLISTLEN || (arg.ptr[FLAG_DUMPLISTLEN] != '1' && arg.ptr[FLAG_DUMPLISTLEN] != '2'))) {
                int target = arg.ptr[FLAG_DUMPLISTLEN] - '1';
                if(gargs.list_streams[target]) fclose(gargs.list_streams[target]);
                gargs.list_streams[target] = get_list_stream(arg, DEFAULT_STREAMS[target]);
                if(gargs.list_streams[target] == NULL) goto defer;
            } else {
                fprintf(stderr, "[ERROR]: '%s' flag NEEDS to be followed by either a 1 or a 2: %s1 or %s2\n", FLAG_DUMPLIST, FLAG_DUMPLIST, FLAG_DUMPLIST);
                fprintf(stderr, "[ERROR]: Aditionally it CAN be followed by =path, where path is the desired output path for that specific list\n");
                quit(1);
            }
        } else if(!gargs.paths[0]) gargs.paths[0] = arg.ptr;
        else gargs.paths[1] = arg.ptr;
    }
    defer:
        for(size_t i = 0; i < NFILES; ++i) if(gargs.list_streams[i]) fclose(gargs.list_streams[i]);
        quit(1);
}

void elog(Error error) {
    fprintf(stderr, "[ERROR]: Couldn't read from file '%s' : %s\n", error.info[0], error.info[1]);
    fprintf(stderr, "[ERROR]: Got error '%s'\n", etochar(error.type));
    quit(1);
}

size_t maxst(size_t a, size_t b) {
    return a < b ? b : a;
}

bool bufputs(Buf *buf, Slice slice) {
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

char *bufapps(Buf *buf, Slice slice) {
    if(buf->cap < buf->len + 1 + slice.len + 1) {
        size_t newcap = buf->cap + maxst(BUFCAP, 1 + slice.len + 1);
        char *dst = realloc(buf->buf, sizeof(char) * newcap);
        if(dst == NULL) return NULL;
        buf->cap = newcap;
        buf->buf = dst;
    } 
    char *dst = buf->buf + (sizeof(char) * (buf->len + (buf->len != 0)));
    memcpy(dst, slice.ptr, sizeof(char) * slice.len);
    buf->len += slice.len + (buf->len != 0);
    buf->buf[buf->len] = '\0';
    return dst;
}

bool bufcats(Buf *buf, Slice slice) {
    if(buf->cap < buf->len + slice.len + 1) {
        size_t newcap = buf->cap + maxst(BUFCAP, slice.len + 1);
        char *dst = realloc(buf->buf, sizeof(char) * newcap);
        if(dst == NULL) return false;
        buf->cap = newcap;
        buf->buf = dst;
    } 
    memcpy(buf->buf + (sizeof(char) * buf->len), slice.ptr, sizeof(char) * slice.len);
    buf->len += slice.len;
    buf->buf[buf->len] = '\0';
    return true;
}

void buffree(Buf *buf) {
    free(buf->buf);
    buf->buf = NULL;
    buf->cap = 0;
    buf->len = 0;
}

Dict *dict_from_buf(Buf buf) {
    Dict *dict = NULL;
    char *ptr = buf.buf;
    while(ptr < buf.buf + buf.len) {
        shput(dict, ptr, 1);
        ptr += strlen(ptr) + 1;
    } return dict;
}

Buf buf_from_file_raw(const char *file_contents, size_t file_len, FILE *stream) {
    Buf buf = {0};
    size_t count = 0;
    Slice slice = snewl(file_contents, file_len);
    while(slice.len > 0) {
        Slice name = sslice(&slice, '\n', true);
        Slice chop = sslices(&name, snew(">>"), true);
        if(!name.len) name = chop;
        name = strim(name);
        if(name.len > 0) {
            char *dst = bufapps(&buf, name);
            if(stream) fprintf(stream, "%zu >> %s\n", ++count, dst);
            if(!dst) {fprintf(stderr, "[ERROR]: Ran out of memory, list too long\n"); quit(1);}
        }
    } return buf;
}

Buf buf_from_file_html(const char *file_contents, size_t file_len, FILE *stream) {
    Buf buf = {0};
    size_t count = 0;
    Slice dcs = snew(".com/");
    Slice slice = snewl(file_contents, file_len);
    while(slice.len > 0) {
        (void)sslices(&slice, dcs, true);
        Slice name = strim(sslice(&slice, '"', true));
        if(name.len > 0) {
            char *dst = bufapps(&buf, name);
            if(stream) fprintf(stream, "%zu >> %s\n", ++count, dst);
            if(!dst) {fprintf(stderr, "[ERROR]: Ran out of memory, list too long\n"); quit(1);}
        }
    } return buf;
}

Buf buf_from_file_div(const char *file_contents, size_t file_len, FILE *stream) {
    Buf buf = {0};
    size_t count = 0;
    Slice aeq = snew("alt=\"");
    Slice slice = snewl(file_contents, file_len);
    div_ensure_english(slice);
    while(slice.len > 0) {
        (void)sslices(&slice, aeq, true);
        Slice name = strim(sslice(&slice, '\'', true));
        if(name.len > 0) {
            char *dst = bufapps(&buf, name);
            if(stream) fprintf(stream, "%zu >> %s\n", ++count, dst);
            if(!dst) {fprintf(stderr, "[ERROR]: Ran out of memory, list too long\n"); quit(1);}
        }
    } return buf;
}

size_t compar_to_file_raw(Dict *dict, const char *file_contents, size_t file_len, FILE *stream) {
    Buf buf = {0};
    size_t count = 0;
    size_t count2 = 0;
    Slice slice = snewl(file_contents, file_len);
    while(slice.len > 0) {
        Slice name = sslice(&slice, '\n', true);
        Slice chop = sslices(&name, snew(">>"), true);
        if(!name.len) name = chop;
        name = strim(name);
        if(name.len > 0) {
            bufputs(&buf, name);
            if(stream) fprintf(stream, "%zu >> %s\n", ++count2, buf.buf);
            if(shgeti(dict, buf.buf) == -1) printf("%zu >> %.*s\n", ++count, (int)name.len, name.ptr);
        }
    } buffree(&buf);
    return count;
}

size_t compar_to_file_html(Dict *dict, const char *file_contents, size_t file_len, FILE *stream) {
    Buf buf = {0};
    size_t count = 0;
    size_t count2 = 0;
    Slice dcs = snew(".com/");
    Slice slice = snewl(file_contents, file_len);
    while(slice.len > 0) {
        (void)sslices(&slice, dcs, true);
        Slice name = strim(sslice(&slice, '"', true));
        if(name.len > 0) {
            bufputs(&buf, name);
            if(stream) fprintf(stream, "%zu >> %s\n", ++count2, buf.buf);
            if(shgeti(dict, buf.buf) == -1) printf("%zu >> %.*s\n", ++count, (int)name.len, name.ptr);
        }
    } buffree(&buf);
    return count;
}

size_t compar_to_file_div(Dict *dict, const char *file_contents, size_t file_len, FILE *stream) {
    Buf buf = {0};
    size_t count = 0;
    size_t count2 = 0;
    Slice aeq = snew("alt=\"");
    Slice slice = snewl(file_contents, file_len);
    div_ensure_english(slice);
    while(slice.len > 0) {
        (void)sslices(&slice, aeq, true);
        Slice name = strim(sslice(&slice, '\'', true));
        if(name.len > 0) {
            bufputs(&buf, name);
            if(stream) fprintf(stream, "%zu >> %s\n", ++count2, buf.buf);
            if(shgeti(dict, buf.buf) == -1) printf("%zu >> %.*s\n", ++count, (int)name.len, name.ptr);
        }
    } buffree(&buf);
    return count;
}

Prefix get_prefix(const char *buffer, size_t len) {
    if(starts_with(buffer, len, PREF_HTML, strlen(PREF_HTML))) return PREFIX_HTML;
    if(starts_with(buffer, len, PREF_DIV, strlen(PREF_DIV))) return PREFIX_DIV;
    return PREFIX_NONE;
}

void div_ensure_english(Slice contents) {
    if(!sfind(contents, snew("'s profile picture"), NULL)) {
        fprintf(stderr, "[ERROR]: Error while parsing <div> file\n");
        fprintf(stderr, "[ERROR]: Seems like your Instagram's language is NOT English\n");
        fprintf(stderr, "[ERROR]: Please try switching it to English in settings and try again or try other file formats %s\n\n", FLAG_DETAHELP);
        fprintf(stderr, "[NOTE]: It could also be that no followers/ing where present on that <div> file...\n");
        quit(1);
    }
}

bool _bufcatS(Buf *buf, ...) {
    Slice s;
    va_list va;
    va_start(va, buf);
    bool ret = true;
    while((s = va_arg(va, Slice)).ptr != NULL) if(!bufcats(buf, s)) {ret = false; goto defer;}
    defer:
        va_end(va);
        return ret;
}