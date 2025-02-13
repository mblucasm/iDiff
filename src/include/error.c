#include "error.h"
#include <assert.h>

void eset(Error *error, ErrorType type, const char *filePath, const char *msg) {
    if(error) {
        error->type = type;
        error->info[0] = filePath;
        error->info[1] = msg;
    }
}

void esett(Error *error, ErrorType type) {
    if(error) error->type = type;
}

char *etochar(ErrorType type) {
    assert(ERROR_COUNT == 7 && "ERROR: some errors were not implemented here");
    switch(type) {
        default: return "ERROR_UNKNOWN";
        case ERROR_OPENING_FILE: return "ERROR_OPENING_FILE";
        case ERROR_NO_ERROR: return "ERROR_NO_ERROR";
        case ERROR_MALLOC: return "ERROR_MALLOC";
        case ERROR_FWRITE: return "ERROR_FWRITE";
        case ERROR_FSEEK: return "ERROR_FSEEK";
        case ERROR_FREAD: return "ERROR_FREAD";
        case ERROR_FILE_WAS_NULL: return "ERROR_FILE_WAS_NULL";
    }
}