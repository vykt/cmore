#ifndef CMORE_H
#define CMORE_H

#ifdef __cplusplus
extern "C"{
#endif

//standard library
#include <stdbool.h>

//system headers
#include <unistd.h>



/*
 *  --- [DATA TYPES] ---
 */

// [byte]
typedef unsigned char cm_byte;



// [list]
struct _cm_list_node {

    void * data;
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
    void * data;

} cm_vector;



// [red-black tree]
enum cm_rb_tree_colour {RED, BLACK};
enum cm_rb_tree_eval {LESS, EQUAL, MORE, ROOT};


struct _cm_rb_tree_node {

    void * key;
    void * data;

    struct _cm_rb_tree_node * left;
    struct _cm_rb_tree_node * right;
    
    struct _cm_rb_tree_node * parent;
    enum cm_rb_tree_eval parent_eval; //if this node is parent's left or right

    enum cm_rb_tree_colour colour; 
};
typedef struct _cm_rb_tree_node cm_rb_tree_node;


typedef struct {

    int size;
    size_t key_size;
    size_t data_size;
    cm_rb_tree_node * root;

    enum cm_rb_tree_eval (*compare)(const void *, const void *);

} cm_rb_tree;

/*
 *  When using cmore's red-black trees, you must implement a 
 *  compare() function for use with the data held by the nodes of 
 *  the red-black tree. That compare function must return:
 *
 *      'enum cm_rb_tree_eval' that holds one of:
 *
 *          LESS, EQUAL, MORE
 *
 *  The ROOT value is reserved for internal use.
 */


/*
 *  --- [FUNCTIONS] ---
 */

// [list]
//0 = success, -1 = error, see cm_errno
extern int cm_list_get_val(const cm_list * list, const int index, void * buf);
//pointer = success, NULL = error, see cm_errno
extern void * cm_list_get_ref(const cm_list * list, const int index);
//pointer = success, NULL = error, see cm_errno
extern cm_list_node * cm_list_get_node(const cm_list * list, const int index);

//pointer = success, NULL = error, see cm_errno
extern cm_list_node * cm_list_set(cm_list * list, 
                                  const int index, const void * data);
extern cm_list_node * cm_list_set_node(cm_list * list,
                                       cm_list_node * node, const void * data);

//pointer = success, NULL = error, see cm_errno
extern cm_list_node * cm_list_insert(cm_list * list, 
                                     const int index, const void * data);
extern cm_list_node * cm_list_insert_before_node(cm_list * list,
                                                 cm_list_node * node,
                                                 const void * data);
extern cm_list_node * cm_list_insert_after_node(cm_list * list,
                                                cm_list_node * node,
                                                const void * data);

//pointer = success, NULL = error, see cm_errno
extern cm_list_node * cm_list_append(cm_list * list, const void * data);
extern cm_list_node * cm_list_unlink(cm_list * list, const int index);

//0 = success, -1 = error, see cm_errno
extern int cm_list_remove(cm_list * list, const int index);
extern int cm_list_remove_node(cm_list * list, cm_list_node * node);

//0 = success, -1 = error, see cm_errno
extern int cm_list_empty(cm_list * list);

//void return
extern void cm_new_list(cm_list * list, const size_t data_size);
//0 = success, -1 = error, see cm_errno
extern void cm_del_list(cm_list * list);
//void return
void cm_del_list_node(cm_list_node * node);



// [vector]
//0 = success, -1 = error, see cm_errno
extern int cm_vector_get_val(const cm_vector * vector, 
                             const int index, void * buf);
//pointer = success, NULL = error, see cm_errno
extern void * cm_vector_get_ref(const cm_vector * vector, 
                                const int index);

//0 = success, -1 = error, see cm_errno
extern int cm_vector_set(cm_vector * vector, 
                         const int index, const void * data);
extern int cm_vector_insert(cm_vector * vector, 
                            const int index, const void * data);
extern int cm_vector_append(cm_vector * vector, const void * data);
extern int cm_vector_remove(cm_vector * vector, const int index);
extern int cm_vector_fit(cm_vector * vector);
//void return
extern void cm_vector_empty(cm_vector * vector);

//void return
extern int cm_new_vector(cm_vector * vector, const size_t data_size);
//0 = success, -1 = error, see cm_errno
extern void cm_del_vector(cm_vector * vector);



// [red-black tree]
//0 = success, -1 = error, see cm_errno
extern int cm_rb_tree_get_val(const cm_rb_tree * tree, 
                              const void * key, void * buf);
//pointer = success, NULL = error, see cm_errno
extern void * cm_rb_tree_get_ref(const cm_rb_tree * tree, const void * key);
extern cm_rb_tree_node * cm_rb_tree_get_node(const cm_rb_tree * tree, 
                                             const void * key);

//pointer = success, NULL = error, see cm_errno
extern cm_rb_tree_node * cm_rb_tree_set(cm_rb_tree * tree,
                                        const void * key, const void * data);
//0 = success, -1 = error, see cm_errno
extern int cm_rb_tree_remove(cm_rb_tree * tree, const void * key);
//pointer = success, NULL = error, see cm_errno
extern cm_rb_tree_node * cm_rb_tree_unlink(cm_rb_tree * tree, const void * key);
//void return
extern void cm_rb_tree_empty(cm_rb_tree * tree);

//void return
extern void cm_new_rb_tree(cm_rb_tree * tree, const size_t key_size, 
                           const size_t data_size, enum cm_rb_tree_eval 
                           (*compare)(const void *, const void *));
extern void cm_del_rb_tree(cm_rb_tree * tree);
extern void cm_del_rb_tree_node(cm_rb_tree_node * node);


// [error handling]
//void return
extern void cm_perror(const char * prefix);
extern const char * cm_strerror(const int cm_errnum);



/*
 *  --- [ERROR HANDLING] ---
 */

extern __thread int cm_errno;


// [error codes]

// 1XX - user errors
#define CM_ERR_USER_INDEX       1100
#define CM_ERR_USER_KEY         1101

// 2XX - internal errors
#define CM_ERR_INTERNAL_INDEX   1200
#define CM_ERR_RB_INVALID_STATE 1201

// 3XX - environment errors
#define CM_ERR_MALLOC           1300
#define CM_ERR_REALLOC          1301


// [error code messages]

// 1XX - user errors
#define CM_ERR_USER_INDEX_MSG       "Index out of range.\n"
#define CM_ERR_USER_KEY_MSG         "Key not present in tree.\n"

// 2XX - internal errors
#define CM_ERR_INTERNAL_INDEX_MSG   "Internal indexing error.\n"
#define CM_ERR_RB_INVALID_STATE_MSG "Red-Black tree reached invalid state.\n"

// 3XX - environmental errors
#define CM_ERR_MALLOC_MSG           "Internal malloc() failed.\n"
#define CM_ERR_REALLOC_MSG          "Internal realloc() failed.\n"


#ifdef __cplusplus
}
#endif

#endif
