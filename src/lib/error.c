//standard library
#include <stdio.h>

//local headers
#include "cmore.h"
#include "error.h"


__thread int cm_errno;


void cm_perror(const char * prefix) {

    switch (cm_errno) {

        // 1XX - user errors
        case CM_ERR_USER_INDEX:
            fprintf(stderr, "%s: %s", prefix, CM_ERR_USER_INDEX_MSG);
            break;

        case CM_ERR_USER_KEY:
            fprintf(stderr, "%s: %s", prefix, CM_ERR_USER_KEY_MSG);
            break;

        // 2XX - internal errors
        case CM_ERR_INTERNAL_INDEX:
            fprintf(stderr, "%s: %s", prefix, CM_ERR_INTERNAL_INDEX_MSG);
            break;

        case CM_ERR_RB_INVALID_STATE:
            fprintf(stderr, "%s: %s", prefix, CM_ERR_RB_INVALID_STATE_MSG);
            break;

        // 3XX - environmental errors
        case CM_ERR_MALLOC:
            fprintf(stderr, "%s: %s", prefix, CM_ERR_MALLOC_MSG);
            break;

        case CM_ERR_REALLOC:
            fprintf(stderr, "%s: %s", prefix, CM_ERR_REALLOC_MSG);
            break;

        default:
            fprintf(stderr, "%s: %s", prefix, "Undefined error code.\n");
            break;
    }

    return;
}



const char * cm_strerror(const int cm_errnum) {

    switch (cm_errnum) {

        // 1XX - user errors
        case CM_ERR_USER_INDEX:
            return CM_ERR_USER_INDEX_MSG;

        case CM_ERR_USER_KEY:
            return CM_ERR_USER_KEY_MSG;

        // 2XX - internal errors
        case CM_ERR_INTERNAL_INDEX:
            return CM_ERR_INTERNAL_INDEX_MSG;

        case CM_ERR_RB_INVALID_STATE:
            return CM_ERR_RB_INVALID_STATE_MSG;

        // 3XX - environmental errors
        case CM_ERR_MALLOC:
            return CM_ERR_MALLOC_MSG;

        case CM_ERR_REALLOC:
            return CM_ERR_REALLOC_MSG;
        
        default:
            return "Undefined error code.\n";
    }
}
