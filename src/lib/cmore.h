#ifndef CMORE_H
#define CMORE_H

#ifdef __cplusplus
extern "C"{
#endif

//standard library
#include <stdbool.h>
#include <limits.h>

//system headers
#include <unistd.h>
#include <linux/limits.h>



/*
 *  --- [DATA TYPES] ---
 */

// [byte]
#define CM_BYTE_MAX UCHAR_MAX
typedef unsigned char cm_byte;



// [list]
struct _cm_lst_node {

    void * data;
    struct _cm_lst_node * next;
    struct _cm_lst_node * prev;

};
typedef struct _cm_lst_node cm_lst_node;


typedef struct {

    int len;
    size_t data_sz;
    cm_lst_node * head;
    bool is_init;

} cm_lst;



// [vector]
typedef struct {

    int len;     //number of elements used
    size_t sz;   //number of elements allocated
    size_t data_sz;
    void * data;
    bool is_init;

} cm_vct;



// [red-black tree]
enum cm_rbt_colour {RED, BLACK};
enum cm_rbt_side {LESS, EQUAL, MORE, ROOT};


struct _cm_rbt_node {

    void * key;
    void * data;

    struct _cm_rbt_node * left;
    struct _cm_rbt_node * right;
    
    struct _cm_rbt_node * parent;
    enum cm_rbt_side parent_side; //if this node is parent's left or right

    enum cm_rbt_colour colour; 
};
typedef struct _cm_rbt_node cm_rbt_node;


typedef struct {

    int size;
    size_t key_sz;
    size_t data_sz;
    cm_rbt_node * root;
    bool is_init;

    enum cm_rbt_side (*compare)(const void *, const void *);

} cm_rbt;

/*
 *  When using cmore's red-black trees, you must implement a 
 *  compare() function for use with the data held by the nodes of 
 *  the red-black tree. That compare function must return:
 *
 *      'enum cm_rbt_side' that holds one of:
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
extern int cm_lst_get(const cm_lst * list, const int index, void * buf);
//pointer = success, NULL = error, see cm_errno
extern void * cm_lst_get_p(const cm_lst * list, const int index);
//pointer = success, NULL = error, see cm_errno
extern cm_lst_node * cm_lst_get_n(const cm_lst * list, const int index);

//pointer = success, NULL = error, see cm_errno
extern cm_lst_node * cm_lst_set(cm_lst * list, 
                                const int index, const void * data);
extern cm_lst_node * cm_lst_set_n(cm_lst * list,
                                  cm_lst_node * node, const void * data);

//pointer = success, NULL = error, see cm_errno
extern cm_lst_node * cm_lst_ins(cm_lst * list, 
                                const int index, const void * data);
extern cm_lst_node * cm_lst_ins_nb(cm_lst * list,
                                   cm_lst_node * node, const void * data);
extern cm_lst_node * cm_lst_ins_na(cm_lst * list,
                                   cm_lst_node * node, const void * data);

//pointer = success, NULL = error, see cm_errno
extern cm_lst_node * cm_lst_apd(cm_lst * list, const void * data);

//pointer = success, NULL = error, see cm_errno
extern cm_lst_node * cm_lst_uln(cm_lst * list, const int index);
extern cm_lst_node * cm_lst_uln_n(cm_lst * list, cm_lst_node * node);

//0 = success, -1 = error, see cm_errno
extern int cm_lst_rmv(cm_lst * list, const int index);
extern int cm_lst_rmv_n(cm_lst * list, cm_lst_node * node);

//0 = success, -1 = error, see cm_errno
extern int cm_lst_emp(cm_lst * list);
extern int cm_lst_cpy(cm_lst * dst_list, cm_lst * src_list);

//void return
extern void cm_new_lst(cm_lst * list, const size_t data_sz);
//0 = success, -1 = error, see cm_errno
extern void cm_del_lst(cm_lst * list);
//void return
void cm_del_lst_node(cm_lst_node * node);



// [vector]
//0 = success, -1 = error, see cm_errno
extern int cm_vct_get(const cm_vct * vector, const int index, void * buf);
//pointer = success, NULL = error, see cm_errno
extern void * cm_vct_get_p(const cm_vct * vector, const int index);

//0 = success, -1 = error, see cm_errno
extern int cm_vct_set(cm_vct * vector, const int index, const void * data);
extern int cm_vct_ins(cm_vct * vector, const int index, const void * data);
extern int cm_vct_apd(cm_vct * vector, const void * data);
extern int cm_vct_rmv(cm_vct * vector, const int index);
extern int cm_vct_fit(cm_vct * vector);
extern int cm_vct_rsz(cm_vct * vector, const int entries);
//void return
extern void cm_vct_emp(cm_vct * vector);
//0 = success, -1 = error, see cm_errno
extern int cm_vct_cpy(cm_vct * dst_vector, const cm_vct * src_vector);

//0 = success, -1 = error, see cm_errno
extern int cm_new_vct(cm_vct * vector, const size_t data_sz);
//void return
extern void cm_del_vct(cm_vct * vector);



// [red-black tree]
//0 = success, -1 = error, see cm_errno
extern int cm_rbt_get(const cm_rbt * tree, const void * key, void * buf);
//pointer = success, NULL = error, see cm_errno
extern void * cm_rbt_get_p(const cm_rbt * tree, const void * key);
extern cm_rbt_node * cm_rbt_get_n(const cm_rbt * tree, const void * key);

//pointer = success, NULL = error, see cm_errno
extern cm_rbt_node * cm_rbt_set(cm_rbt * tree, 
                                const void * key, const void * data);
//0 = success, -1 = error, see cm_errno
extern int cm_rbt_rmv(cm_rbt * tree, const void * key);
//pointer = success, NULL = error, see cm_errno
extern cm_rbt_node * cm_rbt_uln(cm_rbt * tree, const void * key);
//void return
extern void cm_rbt_emp(cm_rbt * tree);
//0 = success, -1 = error, see cm_errno
extern int cm_rbt_cpy(cm_rbt * dst_tree, cm_rbt * src_tree);

//void return
extern void cm_new_rbt(cm_rbt * tree, const size_t key_sz, const size_t data_sz,
                       enum cm_rbt_side (*compare)(const void *, const void *));
extern void cm_del_rbt(cm_rbt * tree);
extern void cm_del_rbt_node(cm_rbt_node * node);



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
