#include <stdio.h>

#include "libcmore.h"
#include "error.h"


_Thread_local int cm_errno;


void cm_perror() {

    switch (cm_errno) {

        // 1XX - user errors
        case CM_ERR_USER_INDEX:
            fprintf(stderr, CM_ERR_USER_INDEX_MSG);
            break;

        // 2XX - internal errors
        case CM_ERR_INTERNAL_INDEX:
            fprintf(stderr, CM_ERR_INTERNAL_INDEX_MSG);
            break;

        // 3XX - environmental errors
        case CM_ERR_MALLOC:
            fprintf(stderr, CM_ERR_MALLOC_MSG);
            break;

        case CM_ERR_REALLOC:
            fprintf(stderr, CM_ERR_REALLOC_MSG);
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
        case CM_ERR_USER_INDEX:
            return CM_ERR_USER_INDEX_MSG;

        // 2XX - internal errors
        case CM_ERR_INTERNAL_INDEX:
            return CM_ERR_INTERNAL_INDEX_MSG;

        // 3XX - environmental errors
        case CM_ERR_MALLOC:
            return CM_ERR_MALLOC_MSG;

        case CM_ERR_REALLOC:
            return CM_ERR_REALLOC_MSG;
        
        default:
            return "Undefined error code.\n";
    }
}
