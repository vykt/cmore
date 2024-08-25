#ifndef LIBCMORE_H
#define LIBCMORE_H

#ifdef __cplusplus
extern "C"{
#endif

#include <unistd.h>


/*
 *  --- [DATA TYPES] ---
 */

// [byte]
typedef char cm_byte;
typedef unsigned char cm_ubyte;


// [list]
struct _cm_list_node {

    cm_byte * data;
    struct _cm_list_node * next;
    struct _cm_list_node * prev;

};
typedef struct _cm_list_node cm_list_node;


typedef struct {

    int len;
    size_t data_size;
    cm_list_node * head;

} cm_list;


// [vector]
typedef struct {

    int len;     //number of elements used
    size_t size; //number of elements allocated
    size_t data_size;
    cm_byte * data;

} cm_vector;


/*
 *  --- [FUNCTIONS] ---
 */

// [list]
//0 = success, -1 = error, see cm_errno
extern int cm_list_get_val(cm_list * list, int index, cm_byte * buf);
//pointer = success, NULL = error, see cm_errno
extern cm_byte * cm_list_get_ref(cm_list * list, int index);

//0 = success, -1 = error, see cm_errno
extern int cm_list_set(cm_list * list, int index, cm_byte * data);
extern int cm_list_insert(cm_list * list, int index, cm_byte * data);
extern int cm_list_append(cm_list * list, cm_byte * data);
extern int cm_list_remove(cm_list * list, int index);
extern int cm_list_empty(cm_list * list);

//void return
extern void cm_new_list(cm_list * list, size_t data_size);
//0 = success, -1 = error, see cm_errno
extern void cm_del_list(cm_list * list);


// [vector]
//0 = success, -1 = error, see cm_errno
extern int cm_vector_get_val(cm_vector * vector, int index, cm_byte * buf);
//pointer = success, NULL = error, see cm_errno
extern cm_byte * cm_vector_get_ref(cm_vector * vector, int index);

//0 = success, -1 = error, see cm_errno
extern int cm_vector_set(cm_vector * vector, int index, cm_byte * data);
extern int cm_vector_insert(cm_vector * vector, int index, cm_byte * data);
extern int cm_vector_append(cm_vector * vector, cm_byte * data);
//void return
extern int cm_vector_remove(cm_vector * vector, int index);
extern void cm_vector_empty(cm_vector * vector);

//void return
extern int cm_new_vector(cm_vector * vector, size_t data_size);
//0 = success, -1 = error, see cm_errno
extern void cm_del_vector(cm_vector * vector);


// [error handling]
//void return
extern void cm_perror();
extern const char * cm_strerror(int cm_errnum);


/*
 *  --- [ERROR HANDLING] ---
 */

extern _Thread_local int cm_errno;

// [error codes]

// 1XX - user errors
#define ERR_USER_INDEX     100

// 2XX - internal errors
#define ERR_INTERNAL_INDEX 200

// 3XX - environment errors
#define ERR_MALLOC         300
#define ERR_REALLOC        301


// [error code messages]

// 1XX - user errors
#define ERR_USER_INDEX_MSG     "Index out of range.\n"

// 2XX - internal errors
#define ERR_INTERNAL_INDEX_MSG "Internal indexing error.\n"

// 3XX - environmental errors
#define ERR_MALLOC_MSG         "Internal malloc() failed.\n"
#define ERR_REALLOC_MSG        "Internal realloc() failed.\n"


#ifdef __cplusplus
}
#endif

#endif
