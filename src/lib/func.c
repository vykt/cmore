//standard library
#include <stdlib.h>
#include <string.h>

//local headers
#include "cmore.h"
#include "debug.h"



/*
 *  --- [META TYPE - EXTERNAL] ---
 */

void cm_meta_type_set(cm_meta_type * value, const void * data) {

    //copy the data
    memcpy(value->data, data, value->sz);

    return;
}



int cm_meta_type_upd(cm_meta_type * value, const int type_id,
                     const void * data, const size_t sz) {

    //update the type id
    value->type_id = type_id;

    //reallocate space for the type
    value->data = realloc(value->data, sz);
    if (value->data == NULL) {
        cm_errno = CM_ERR_REALLOC;
        return -1;
    }

    //copy the data
    value->sz = sz;
    memcpy(value->data, data, value->sz);

    return 0;
}



int cm_meta_type_cpy(cm_meta_type * dst_value,
                     const cm_meta_type * src_value) {

    //copy the type
    int ret = cm_new_meta_type(dst_value, src_value->type_id,
                               src_value->data, src_value->sz);
    if (ret != 0) return -1;

    return 0;
}



int cm_new_meta_type(cm_meta_type * value, const int type_id, 
                     const void * data, const size_t sz) {

    //set the type id
    value->type_id = type_id;

    //allocate space for the type
    value->data = malloc(sz);
    if (value->data == NULL) {
        cm_errno = CM_ERR_MALLOC;
        return -1;
    }

    //copy the data
    value->sz = sz;
    memcpy(value->data, data, value->sz);

    //set meta type as initialised
    value->is_init = true;
    
    return 0;
}



void cm_del_meta_type(cm_meta_type * value) {

    //deallocate space for the type
    free(value->data);

    //set meta type as uninitialised
    value->is_init = false;

    return;
}



/*
 *  --- [MONAD - EXTERNAL] ---
 */

int cm_monad_compose(cm_monad * monad,
                     cm_meta_type * (* cb)(cm_meta_type *, void * ctx)) {

    cm_lst_node * new_cb_node = cm_lst_apd(&monad->thunk, &cb);
    if (new_cb_node == NULL) return -1;

    return 0;
}



int cm_monad_eval(cm_monad * monad, cm_meta_type * value, void * ctx) {

    cm_meta_type * tmp_value;
    cm_lst_node * thunk_node;
    cm_meta_type * (* cb)(cm_meta_type *, void *);


    //setup iteration
    tmp_value = value;
    thunk_node = monad->thunk.head;

    //execute all callbacks
    for (int i = 0; i < monad->thunk.len; ++i) {

        //call this callback
        cb = *(cm_meta_type * (**)(cm_meta_type *, void *)) thunk_node->data;
        tmp_value = cb(tmp_value, ctx);

        //terminate early on error
        if (tmp_value->type_id == CM_MONAD_FAIL_TYPE) return -1;

        //advance iteration
        thunk_node = thunk_node->next;
    }

    return 0;
}



void cm_new_monad(cm_monad * monad) {

    //initialise the function list
    cm_new_lst(&monad->thunk, sizeof(void *));

    //set monad as initialised
    monad->is_init = true;

    return;
}



void cm_del_monad(cm_monad * monad) {

    //delete the function list
    cm_del_lst(&monad->thunk);

    //set monad as uninitialised
    monad->is_init = false;

    return;
}
