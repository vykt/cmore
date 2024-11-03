#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <unistd.h>

#include "libcmore.h"
#include "list.h"



/*
 *  --- [INTERNAL] ---
 */

static inline int _normalise_index(const cm_list * list, int index) {

    if (index < 0) { 
        index = list->len + index;
    }

    return index;
}



static cm_list_node * _traverse(const cm_list * list, int index) {

    cm_list_node * node = list->head;
    bool traverse_forward;

    //determine if traversing forwards or backwards
    if (index >= 0) {
        traverse_forward = true;
    } else {
        index *= -1;
        traverse_forward = false;
    }

    //traverse, checking for premature NULL pointers
    while ((node != NULL) && (index != 0)) {

        if (traverse_forward) {
            node = node->next;
        } else {
            node = node->prev;
        }

        --index;
    }

    if (index) {
        cm_errno = CM_ERR_INTERNAL_INDEX;
        return NULL;
    }
    return node;
}



static cm_list_node * _new_cm_list_node(const cm_list * list, const cm_byte * data) {

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



static void _del_cm_list_node(cm_list_node * node) {

    free(node->data);
    free(node);

    return;
}



static void _set_head_node(cm_list * list, cm_list_node * node) {

    node->next = node->prev = NULL;

    list->head = node;
    ++list->len;

    return;
}



static void _add_node(cm_list * list, cm_list_node * node, cm_list_node * prev_node, 
                      cm_list_node * next_node, const int index) {

    prev_node->next = node;
    node->prev = prev_node;

    next_node->prev = node;
    node->next = next_node;

    if (index == 0) list->head = node;
    ++list->len;

    return;
}



static void _sub_node(cm_list * list, cm_list_node * prev_node, 
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



static int _empty(cm_list * list) {
 
    cm_list_node * node = list->head, * next_node;
    int index = list->len;

    while ((node != NULL) && (index != 0)) {

        next_node = node->next;
        _del_cm_list_node(node);
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



static inline int _assert_index_range(const cm_list * list, 
                                      const int index, enum _index_mode mode) {
   
    /*
     *  if inserting, maximum index needs to be +1 higher than for other operations
     */
    if (abs(index) >= (list->len + mode)) {
        cm_errno = CM_ERR_USER_INDEX;
        return -1;
    }

    return 0;
}



/*
 *  --- [EXTERNAL] ---
 */

int cm_list_get_val(const cm_list * list, const int index, cm_byte * buf) {

    if (_assert_index_range(list, index, INDEX)) return -1;

    //get the node
    cm_list_node * node = _traverse(list, index);
    if (!node) return -1;

    memcpy(buf, node->data, list->data_size);
    
    return 0;
}



cm_byte * cm_list_get_ref(const cm_list * list, const int index) {

    if (_assert_index_range(list, index, INDEX)) return NULL;

    //get the node
    cm_list_node * node = _traverse(list, index);
    if (!node) return NULL;

    return node->data;
}



cm_list_node * cm_list_get_node(const cm_list * list, const int index) {

    if (_assert_index_range(list, index, INDEX)) return NULL;

    //get the node
    return _traverse(list, index);
}



cm_list_node * cm_list_set(cm_list * list, const int index, const cm_byte * data) {

    if (_assert_index_range(list, index, INDEX)) return NULL;

    //get the node
    cm_list_node * node = _traverse(list, index);
    if (!node) return NULL;

    memcpy(node->data, data, list->data_size);

    return node;
}



cm_list_node * cm_list_insert(cm_list * list, const int index, const cm_byte * data) {

    cm_list_node * prev_node, * next_node;
    int normalised_index;

    if (_assert_index_range(list, index, ADD_INDEX)) return NULL;

    //create new node
    cm_list_node * new_node = _new_cm_list_node(list, data);
    if (!new_node) return NULL;

    //to simplify, convert a negative index to a positive equivalent
    normalised_index = _normalise_index(list, index);
    
    //get the _prev_ and _next_ of the new node as required
    if (list->len == 0) {
        _set_head_node(list, new_node);
    
    } else if (list->len == 1) { 
        next_node = prev_node = list->head; 
        
        _add_node(list, new_node, prev_node, next_node, normalised_index);

    } else { 
        if (normalised_index == list->len) {
            prev_node = list->head->prev;
        } else {
            prev_node = _traverse(list, normalised_index-1);
            if (!prev_node) {
                _del_cm_list_node(new_node);
                return NULL;
            }
        }
        next_node = prev_node->next;

        _add_node(list, new_node, prev_node, next_node, normalised_index);
    }

    return new_node;
}



cm_list_node * cm_list_append(cm_list * list, const cm_byte * data) {

    cm_list_node * new_node = _new_cm_list_node(list, data);
    if (!new_node) return NULL;

    //add node to list
    if (list->len == 0) {
        _set_head_node(list, new_node);
    
    } else if (list->len == 1) {
        _add_node(list, new_node, list->head, list->head, -1);
    
    } else {
        _add_node(list, new_node, list->head->prev, list->head, -1);
    }

    return new_node;
}



int cm_list_remove(cm_list * list, const int index) {
 
    if (_assert_index_range(list, index, INDEX)) return -1;

    //get the node
    cm_list_node * del_node = _traverse(list, index);
    if(!del_node) return -1;

    _sub_node(list, del_node->prev, del_node->next, index);
    _del_cm_list_node(del_node);

    --list->len;

    return 0;
}



int cm_list_unlink(cm_list * list, const int index) {

    if (_assert_index_range(list, index, INDEX)) return -1;
    
    //get the node
    cm_list_node * unlink_node = _traverse(list, index);
    if (!unlink_node) return -1;

    _sub_node(list, unlink_node->prev, unlink_node->next, index);

    --list->len;

    return 0;
}



int cm_list_empty(cm_list * list) {

    if (_empty(list)) return -1;

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
        _sub_node(list, del_node->prev, del_node->next, 0);
        _del_cm_list_node(del_node);
    
    } //end for

    return;
}
