//standard library
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

//system headers
#include <unistd.h>

//local headers
#include "cmore.h"
#include "debug.h"
#include "lst.h"



/*
 *  --- [INTERNAL] ---
 */

DBG_STATIC 
cm_lst_node * _lst_traverse(const cm_lst * list, int index) {

    cm_lst_node * node = list->head;
    bool traverse_forward;

    //determine if traversing forwards or backwards
    if (index >= 0) {
        traverse_forward = true;
    } else {
        traverse_forward = false;
    }

    //traverse, checking for premature NULL pointers
    while ((node != NULL) && (index != 0)) {

        if (traverse_forward) {
            node = node->next;
            index--;
        } else {
            node = node->prev;
            index++;
        }
    }

    if (index) {
        cm_errno = CM_ERR_INTERNAL_INDEX;
        return NULL;
    }

    return node;
}



DBG_STATIC 
cm_lst_node * _lst_new_node(const cm_lst * list, const void * data) {

    //allocate node structure
    cm_lst_node * new_node = malloc(sizeof(cm_lst_node));
    if (!new_node) {
        cm_errno = CM_ERR_MALLOC;
        return NULL;
    }

    //allocate data
    new_node->data = malloc(list->data_sz);
    if (!new_node->data) {
        cm_errno = CM_ERR_MALLOC;
        return NULL;
    }

    //copy data into node
    memcpy(new_node->data, data, list->data_sz);

    return new_node;
}



DBG_STATIC 
void _lst_del_node(cm_lst_node * node) {

    free(node->data);
    free(node);

    return;
}



DBG_STATIC 
void _lst_set_head_node(cm_lst * list, cm_lst_node * node) {

    node->next = node->prev = NULL;
    list->head = node;
    list->len++;

    return;
}



DBG_STATIC 
void _lst_add_node(cm_lst * list, 
                   cm_lst_node * node, cm_lst_node * prev_node,
                   cm_lst_node * next_node, const int index) {

    prev_node->next = node;
    node->prev = prev_node;

    next_node->prev = node;
    node->next = next_node;

    if (index == 0) list->head = node;
    ++list->len;

    return;
}



DBG_STATIC 
void _lst_sub_node(cm_lst * list, cm_lst_node * prev_node, 
                   cm_lst_node * next_node, const int index) {

    //if there are no nodes left
    if (prev_node == NULL && next_node == NULL) {
        
        list->head = NULL;

    //if there is one node left
    } else if (prev_node == next_node) {
        
        prev_node->next = next_node->prev = NULL;

    //if there are multiple nodes left
    } else {

        prev_node->next = next_node;
        next_node->prev = prev_node;       
    }

    //update head if it was removed
    if (index == 0) list->head = next_node;

    return;
}



DBG_STATIC 
int _lst_emp(cm_lst * list) {
 
    cm_lst_node * node = list->head, * next_node;
    int index = list->len;

    while ((node != NULL) && (index != 0)) {

        next_node = node->next;
        _lst_del_node(node);
        node = next_node;
        --index;
    }

    if (index) {
        cm_errno = CM_ERR_INTERNAL_INDEX;
        return -1;
    }

    list->len = 0;
    list->head = NULL;

    return 0;
}



DBG_STATIC DBG_INLINE 
int _lst_assert_index_range(const cm_lst * list, 
                            const int index, enum _lst_index_mode mode) {
   
    /*
     *  If inserting, maximum index needs to be +1 higher than other operations.
     */

    if (abs(index) >= (list->len + (int) mode)) {
        cm_errno = CM_ERR_USER_INDEX;
        return -1;
    }

    return 0;
}



/*
 *  --- [EXTERNAL] ---
 */

int cm_lst_get(const cm_lst * list, const int index, void * buf) {

    if (_lst_assert_index_range(list, index, INDEX)) return -1;

    //get the node
    cm_lst_node * node = _lst_traverse(list, index);
    if (!node) return -1;

    memcpy(buf, node->data, list->data_sz);
    
    return 0;
}



void * cm_lst_get_p(const cm_lst * list, const int index) {

    if (_lst_assert_index_range(list, index, INDEX)) return NULL;

    //get the node
    cm_lst_node * node = _lst_traverse(list, index);
    if (!node) return NULL;

    return node->data;
}



cm_lst_node * cm_lst_get_n(const cm_lst * list, const int index) {

    if (_lst_assert_index_range(list, index, INDEX)) return NULL;

    //get the node
    return _lst_traverse(list, index);
}



cm_lst_node * cm_lst_set(cm_lst * list, 
                         const int index, const void * data) {

    if (_lst_assert_index_range(list, index, INDEX)) return NULL;

    //get the node
    cm_lst_node * node = _lst_traverse(list, index);
    if (!node) return NULL;

    memcpy(node->data, data, list->data_sz);

    return node;
}



cm_lst_node * cm_lst_set_n(cm_lst * list,
                           cm_lst_node * node, const void * data) {

    memcpy(node->data, data, list->data_sz);

    return node;
}



cm_lst_node * cm_lst_ins(cm_lst * list, 
                         const int index, const void * data) {

    cm_lst_node * prev_node, * next_node;

    if (_lst_assert_index_range(list, index, ADD_INDEX)) return NULL;

    //create new node
    cm_lst_node * new_node = _lst_new_node(list, data);
    if (!new_node) return NULL;
    
    //assign prev_node and next_node depending on case
    if (list->len == 0) {

        _lst_set_head_node(list, new_node);
        return new_node;

    } else {
        
        if (list->len == 1) { 
        
            next_node = prev_node = list->head; 

        } else { 
        
            //get appropriate next_node depending on index +/-
            if (index >= 0) {
                next_node = _lst_traverse(list, index);
            } else {
                next_node = _lst_traverse(list, index + 1);
            }
            if (!next_node) {
                _lst_del_node(new_node);
                return NULL;
            }
            prev_node = next_node->prev;
        }
        
        _lst_add_node(list, new_node, prev_node, next_node, index);
        return new_node;
    
    } //end else
}



cm_lst_node * cm_lst_ins_nb(cm_lst * list,
                            cm_lst_node * node, const void * data) {

    int index = 0;
    cm_lst_node * prev_node, * next_node;

    //create new node
    cm_lst_node * new_node = _lst_new_node(list, data);

    //assign prev_node and next_node depending on case
    if (list->len == 1) {
        
        next_node = prev_node = node;
    
    } else {

        next_node = node;
        prev_node = node->prev;
        index = list->head == node ? 0 : -1;
    }

    _lst_add_node(list, new_node, prev_node, next_node, index);

    return new_node;
}



cm_lst_node * cm_lst_ins_na(cm_lst * list,
                            cm_lst_node * node, const void * data) {

    cm_lst_node * prev_node, * next_node;

    //create new node
    cm_lst_node * new_node = _lst_new_node(list, data);

    //assign prev_node and next_node depending on case
    if (list->len == 1) {
        
        next_node = prev_node = node;
    
    } else {

        next_node = node->next;
        prev_node = node;
    }

    _lst_add_node(list, new_node, prev_node, next_node, -1);

    return new_node;
}



cm_lst_node * cm_lst_apd(cm_lst * list, const void * data) {

    cm_lst_node * new_node = _lst_new_node(list, data);
    if (!new_node) return NULL;

    //add node to list
    if (list->len == 0) {
        _lst_set_head_node(list, new_node);

    } else if (list->len == 1) {
        _lst_add_node(list, new_node, list->head, list->head, -1);
    
    } else {
        _lst_add_node(list, new_node, list->head->prev, list->head, -1);
    }

    return new_node;
}



cm_lst_node * cm_lst_uln(cm_lst * list, const int index) {

    if (_lst_assert_index_range(list, index, INDEX)) return NULL;
    
    //get the node
    cm_lst_node * unlink_node = _lst_traverse(list, index);
    if (!unlink_node) return NULL;

    //unlink it from the list
    _lst_sub_node(list, unlink_node->prev, unlink_node->next, index);
    
    --list->len;

    return unlink_node;
}



cm_lst_node * cm_lst_uln_n(cm_lst * list, cm_lst_node * node) {

    int index = list->head == node ? 0 : -1;

    //unlink the node from the list
    _lst_sub_node(list, node->prev, node->next, index);
    
    --list->len;

    return node;
}



int cm_lst_rmv(cm_lst * list, const int index) {
 
    if (_lst_assert_index_range(list, index, INDEX)) return -1;

    //get the node
    cm_lst_node * del_node = _lst_traverse(list, index);
    if(!del_node) return -1;

    _lst_sub_node(list, del_node->prev, del_node->next, index);
    _lst_del_node(del_node);
    
    --list->len;

    return 0;
}



int cm_lst_rmv_n(cm_lst * list, cm_lst_node * node) {

    int index = list->head == node ? 0 : -1;

    _lst_sub_node(list, node->prev, node->next, index);
    _lst_del_node(node);

    --list->len;

    return 0;
}



int cm_lst_emp(cm_lst * list) {

    if (_lst_emp(list)) return -1;

    return 0;
}



int cm_lst_cpy(cm_lst * dst_list, cm_lst * src_list) {

    cm_lst_node * ret, * node;

    //initialise the destination list
    cm_new_lst(dst_list, src_list->data_sz);

    //copy each node to the destination list
    node = src_list->head;
    for (int i = 0; i < src_list->len; ++i) {

        //copy node `i` from the source list to the destination list
        ret = cm_lst_apd(dst_list, node->data);
        if (ret == NULL) {
            cm_del_lst(dst_list);
            return 0;
        }

        //advance iteration
        node = node->next;
        
    } //end for

    return 0;
}



void cm_new_lst(cm_lst * list, const size_t data_sz) {

    list->len = 0;
    list->data_sz = data_sz;
    list->head = NULL;
    list->is_init = true;
    return;
}



void cm_del_lst(cm_lst * list) {

    int len = list->len;
    cm_lst_node * del_node;

    //delete each node in list
    for (int i = 0; i < len; ++i) {

        del_node = list->head;
        _lst_sub_node(list, del_node->prev, del_node->next, 0);
        _lst_del_node(del_node);
    
    } //end for

    list->is_init = false;
    return;
}



void cm_del_lst_node(cm_lst_node * node) {

    _lst_del_node(node);

    return;
}
