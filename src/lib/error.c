#include <stdio.h>

#include "libcmore.h"
#include "error.h"


_Thread_local int cm_errno;


void cm_perror() {

    switch (cm_errno) {

        // 1XX - user errors
        case ERR_USER_INDEX:
            fprintf(stderr, ERR_USER_INDEX_MSG);
            break;

        // 2XX - internal errors
        case ERR_INTERNAL_INDEX:
            fprintf(stderr, ERR_INTERNAL_INDEX_MSG);
            break;

        // 3XX - environmental errors
        case ERR_MALLOC:
            fprintf(stderr, ERR_MALLOC_MSG);
            break;

        case ERR_REALLOC:
            fprintf(stderr, ERR_REALLOC_MSG);
            break;

        default:
            fprintf(stderr, "Undefined error code.\n");
            break;
    }

    return;
}


const char * cm_strerror(int cm_errnum) {

    switch (cm_errnum) {

        // 1XX - user errors
        case ERR_USER_INDEX:
            return ERR_USER_INDEX_MSG;

        // 2XX - internal errors
        case ERR_INTERNAL_INDEX:
            return ERR_INTERNAL_INDEX_MSG;

        // 3XX - environmental errors
        case ERR_MALLOC:
            return ERR_MALLOC_MSG;

        case ERR_REALLOC:
            return ERR_REALLOC_MSG;
        
        default:
            return "Undefined error code.\n";
    }
}
