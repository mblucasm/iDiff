#ifndef ERROR_H
#define ERROR_H

#include <errno.h>

#define esetgoto(errorp, type, info0, info1, label) do {eset(errorp, type, info0, info1); goto label;} while(0)
#define esettgoto(errorp, type, label) do {esett(errorp, type); goto label;} while(0)

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

void eset(Error *error, ErrorType type, const char *info0, const char *info1);
void esett(Error *error, ErrorType type);
char *etochar(ErrorType type);

#endif // ERROR_H