//standard library
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

//system headers
#include <unistd.h>

//local headers
#include "cmore.h"
#include "debug.h"
#include "list.h"



/*
 *  --- [INTERNAL] ---
 */

DBG_STATIC 
cm_list_node * _list_traverse(const cm_list * list, int index) {

    cm_list_node * node = list->head;
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
cm_list_node * _list_new_node(const cm_list * list, const void * data) {

    //allocate node structure
    cm_list_node * new_node = malloc(sizeof(cm_list_node));
    if (!new_node) {
        cm_errno = CM_ERR_MALLOC;
        return NULL;
    }

    //allocate data
    new_node->data = malloc(list->data_size);
    if (!new_node->data) {
        cm_errno = CM_ERR_MALLOC;
        return NULL;
    }

    //copy data into node
    memcpy(new_node->data, data, list->data_size);

    return new_node;
}



DBG_STATIC 
void _list_del_node(cm_list_node * node) {

    free(node->data);
    free(node);

    return;
}



DBG_STATIC 
void _list_set_head_node(cm_list * list, cm_list_node * node) {

    node->next = node->prev = NULL;
    list->head = node;
    list->len++;

    return;
}



DBG_STATIC 
void _list_add_node(cm_list * list, 
                    cm_list_node * node, cm_list_node * prev_node,
                    cm_list_node * next_node, const int index) {

    prev_node->next = node;
    node->prev = prev_node;

    next_node->prev = node;
    node->next = next_node;

    if (index == 0) list->head = node;
    ++list->len;

    return;
}



DBG_STATIC 
void _list_sub_node(cm_list * list, cm_list_node * prev_node, 
                    cm_list_node * next_node, const int index) {

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
cm_list_node * _list_insert_node(cm_list * list, cm_list_node * node, 
                                 const void * data, int index) {

    cm_list_node * prev_node, * next_node;

    //create new node
    cm_list_node * new_node = _list_new_node(list, data);

    //assign prev_node and next_node depending on case
    if (list->len == 1) {
        
        next_node = prev_node = node;
    
    } else {

        next_node = node;
        prev_node = node->prev;
        index = list->head == node ? 0 : -1;
    }

    _list_add_node(list, new_node, prev_node, next_node, 0);
    return new_node;
}



DBG_STATIC 
int _list_empty(cm_list * list) {
 
    cm_list_node * node = list->head, * next_node;
    int index = list->len;

    while ((node != NULL) && (index != 0)) {

        next_node = node->next;
        _list_del_node(node);
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
int _list_assert_index_range(const cm_list * list, 
                             const int index, enum _list_index_mode mode) {
   
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

int cm_list_get_val(const cm_list * list, const int index, void * buf) {

    if (_list_assert_index_range(list, index, INDEX)) return -1;

    //get the node
    cm_list_node * node = _list_traverse(list, index);
    if (!node) return -1;

    memcpy(buf, node->data, list->data_size);
    
    return 0;
}



void * cm_list_get_ref(const cm_list * list, const int index) {

    if (_list_assert_index_range(list, index, INDEX)) return NULL;

    //get the node
    cm_list_node * node = _list_traverse(list, index);
    if (!node) return NULL;

    return node->data;
}



cm_list_node * cm_list_get_node(const cm_list * list, const int index) {

    if (_list_assert_index_range(list, index, INDEX)) return NULL;

    //get the node
    return _list_traverse(list, index);
}



cm_list_node * cm_list_set(cm_list * list, 
                           const int index, const void * data) {

    if (_list_assert_index_range(list, index, INDEX)) return NULL;

    //get the node
    cm_list_node * node = _list_traverse(list, index);
    if (!node) return NULL;

    memcpy(node->data, data, list->data_size);

    return node;
}



cm_list_node * cm_list_set_node(cm_list * list,
                                cm_list_node * node, const void * data) {

    memcpy(node->data, data, list->data_size);

    return node;
}



cm_list_node * cm_list_insert(cm_list * list, 
                              const int index, const void * data) {

    cm_list_node * prev_node, * next_node;

    if (_list_assert_index_range(list, index, ADD_INDEX)) return NULL;

    //create new node
    cm_list_node * new_node = _list_new_node(list, data);
    if (!new_node) return NULL;
    
    //assign prev_node and next_node depending on case
    if (list->len == 0) {

        _list_set_head_node(list, new_node);
        return new_node;

    } else {
        
        if (list->len == 1) { 
        
            next_node = prev_node = list->head; 

        } else { 
        
            //get appropriate next_node depending on index +/-
            if (index >= 0) {
                next_node = _list_traverse(list, index);
            } else {
                next_node = _list_traverse(list, index + 1);
            }
            if (!next_node) {
                _list_del_node(new_node);
                return NULL;
            }
            prev_node = next_node->prev;
        }
        
        _list_add_node(list, new_node, prev_node, next_node, index);
        return new_node;
    
    } //end else
}



cm_list_node * cm_list_insert_node_before(cm_list * list,
                                          cm_list_node * node, 
                                          const void * data) {

    int index = 0;
    cm_list_node * new_node = _list_insert_node(list, node, data, index);

    return new_node;
}



cm_list_node * cm_list_insert_node_after(cm_list * list,
                                         cm_list_node * node, 
                                         const void * data) {

    int index = -1;
    cm_list_node * new_node = _list_insert_node(list, node, data, index);

    return new_node;
}



cm_list_node * cm_list_append(cm_list * list, const void * data) {

    cm_list_node * new_node = _list_new_node(list, data);
    if (!new_node) return NULL;

    //add node to list
    if (list->len == 0) {
        _list_set_head_node(list, new_node);

    } else if (list->len == 1) {
        _list_add_node(list, new_node, list->head, list->head, -1);
    
    } else {
        _list_add_node(list, new_node, list->head->prev, list->head, -1);
    }

    return new_node;
}



cm_list_node * cm_list_unlink(cm_list * list, const int index) {

    if (_list_assert_index_range(list, index, INDEX)) return NULL;
    
    //get the node
    cm_list_node * unlink_node = _list_traverse(list, index);
    if (!unlink_node) return NULL;

    //unlink it from the list
    _list_sub_node(list, unlink_node->prev, unlink_node->next, index);
    --list->len;

    return unlink_node;
}



cm_list_node * cm_list_unlink_node(cm_list * list, cm_list_node * node) {

    int index = list->head == node ? 0 : -1;

    //unlink the node from the list
    _list_sub_node(list, node->prev, node->next, index);
}



int cm_list_remove(cm_list * list, const int index) {
 
    if (_list_assert_index_range(list, index, INDEX)) return -1;

    //get the node
    cm_list_node * del_node = _list_traverse(list, index);
    if(!del_node) return -1;

    _list_sub_node(list, del_node->prev, del_node->next, index);
    _list_del_node(del_node);

    --list->len;

    return 0;
}



int cm_list_remove_node(cm_list * list, cm_list_node * node) {

    int index = list->head == node ? 0 : -1;

    _list_sub_node(list, node->prev, node->next, index);
    _list_del_node(node);

    --list->len;

    return 0;
}



int cm_list_empty(cm_list * list) {

    if (_list_empty(list)) return -1;

    return 0;
}



void cm_new_list(cm_list * list, const size_t data_size) {

    list->len = 0;
    list->data_size = data_size;
    list->head = NULL;
    return;
}



void cm_del_list(cm_list * list) {

    int len = list->len;
    cm_list_node * del_node;

    //delete each node in list
    for (int i = 0; i < len; ++i) {

        del_node = list->head;
        _list_sub_node(list, del_node->prev, del_node->next, 0);
        _list_del_node(del_node);
    
    } //end for

    return;
}



void cm_del_list_node(cm_list_node * node) {

    _list_del_node(node);

    return;
}
