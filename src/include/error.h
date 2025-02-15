#ifndef ERROR_H
#define ERROR_H

#include <errno.h>

#define __esettgoto(error, type, label) do {esett(error, type); goto label;} while(0)
#define __esetgoto(error, type, label) do {eset(error, type, filePath, strerror(errno)); goto label;} while(0)

typedef enum {
    ERROR_NO_ERROR,
    ERROR_OPENING_FILE,
    ERROR_MALLOC,
    ERROR_FSEEK,
    ERROR_FWRITE,
    ERROR_FILE_WAS_NULL,
    ERROR_FREAD,
    ERROR_COUNT,
} ErrorType;

typedef struct {
    ErrorType type;
    const char *info[2];
} Error;

void eset(Error *error, ErrorType type, const char *filePath, const char *msg);
void esett(Error *error, ErrorType type);
char *etochar(ErrorType type);

#endif // ERROR_H