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

#define NFILES (2)
#define DIV_PREF ("<div")
#define DIV_PREFL (sizeof(DIV_PREF) / sizeof(DIV_PREF[0]) - 1)

#define FLAG_GETLIST1 ("--get-list-1")
#define FLAG_GETLIST2 ("--get-list-2")
#define FLAG_DETAHELP ("--detailed-help")

typedef struct {
    bool get_lists[2];
    const char *program;
    const char *paths[2];
} Args;

Args args_parse(int *argc, char ***argv);
char *arg_shift(int *argc, char ***argv);

typedef struct {
    char *key;
    int value;
} Dict;

Dict *dict_from_list(char **list);

void usage(void);
void detahelp(void);
void elog(Error error);
void quit(int code);

char *file_read(const char *filePath, size_t *readedLen, Error *error);
char **div_parse_into_list(char *div, size_t len);
char **list_parse_into_list(char *contents, size_t len);
void list_free(char **list);
void list_print(char **list);
bool starts_with(char *buffer, size_t len, char *prefix, size_t plen);

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
// Uses the stb_ds.h library for dynamic arrays and hash tables.

int main(int argc, char **argv) {
    
    Args args = args_parse(&argc, &argv);
    if(!args.paths[0] || !args.paths[1]) {usage(); quit(1);}
    
    char *file_contents[NFILES];
    size_t file_lens[NFILES];
    for(size_t i = 0; i < NFILES; ++i) {
        Error error;
        file_contents[i] = file_read(args.paths[i], file_lens + i, &error);
        if(!file_contents[i]) elog(error);    
    }
    
    char **lists[NFILES];
    for(size_t i = 0; i < NFILES; ++i) {
        char *buf = file_contents[i];
        size_t len = file_lens[i];
        lists[i] = starts_with(buf, len, DIV_PREF, DIV_PREFL) ? div_parse_into_list(buf, len) : list_parse_into_list(buf, len);
    }

    for(size_t i = 0; i < NFILES; ++i) {
        size_t len = printf("==== LIST %llu (%llu INSTANCES) ====\n", i + 1, arrlenu(lists[i]));
        if(args.get_lists[i]) list_print(lists[i]);
        for(size_t j = 0; j < len - 1; ++j) printf("=");
        printf("\n\n");
        free(file_contents[i]);
    }

    Dict *dict = dict_from_list(lists[0]);
    printf("===================NOT MATCHING LIST===================\n");
    size_t count = 0;
    for(size_t i = 0; i < arrlenu(lists[1]); ++i) if(shgeti(dict, lists[1][i]) == -1) printf("%llu >> %s\n", ++count, lists[1][i]);
    printf("=================NOT MATCHING LIST END=================\n");
    printf("-> Number of users that didn't match = %llu\n", count);

    shfree(dict);
    for(size_t i = 0; i < NFILES; ++i) list_free(lists[i]);
    quit(0);
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

char **div_parse_into_list(char *div, size_t len) {
    char **list = NULL;
    Slice slice = snewl(div, len);
    while(slice.len > 0) {
        (void)sslices(&slice, snew("alt=\""), true);
        Slice name = sslice(&slice, '\'', true);
        if(name.len > 0) {
            char *seg = malloc(sizeof(char) * (name.len + 1));
            memcpy(seg, name.ptr, sizeof(char) * name.len);
            seg[name.len] = '\0';
            arrput(list, seg);
        }
    } return list;
}

char **list_parse_into_list(char *contents, size_t len) {
    char **list = NULL;
    Slice s = snewl(contents, len);
    while(s.len > 0) {
        Slice name = sslice(&s, '\n', true);
        Slice chop = sslices(&name, snew(">>"), true);
        if(!name.len) name = chop;
        name = strim(name);
        if(name.len > 0) {
            char *seg = malloc(sizeof(char) * (name.len + 1));
            memcpy(seg, name.ptr, sizeof(char) * name.len);
            seg[name.len] = '\0';
            arrput(list, seg);
        }
    } return list;
}

Dict *dict_from_list(char **list) {
    Dict *dict = NULL;
    for(size_t i = 0; i < arrlenu(list); ++i) shput(dict, list[i], 1);
    return dict;
}

bool starts_with(char *buffer, size_t len, char *prefix, size_t plen) {
    if(len < plen) return false;
    for(size_t i = 0; i < plen; ++i) if(prefix[i] != buffer[i]) return false;
    return true;
}

void list_free(char **list) {
    for(size_t i = 0; i < arrlenu(list); ++i) free(list[i]);
    arrfree(list);
}

void list_print(char **list) {
    for(size_t i = 0; i < arrlenu(list); ++i) printf("%llu >> %s\n", i + 1, list[i]);
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
        char *arg = arg_shift(argc, argv);
        if  (strcmp(arg, FLAG_DETAHELP) == 0) {usage(); detahelp(); quit(0);}
        else if(strcmp(arg, FLAG_GETLIST1) == 0) args.get_lists[0] = true;
        else if(strcmp(arg, FLAG_GETLIST2) == 0) args.get_lists[1] = true;
        else if(!args.paths[0]) args.paths[0] = arg;
        else args.paths[1] = arg;
    } return args;
}

void elog(Error error) {
    fprintf(stderr, "[INSTA_ERROR]: Couldn't read from file '%s' : %s\n", error.info[0], error.info[1]);
    fprintf(stderr, "[INSTA_ERROR]: Got error '%s'\n", etochar(error.type));
    quit(1);
}