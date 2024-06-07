#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <unistd.h>

#include "libcmore.h"
#include "list.h"



/*
 *  --- [INTERNAL] ---
 */

static inline int _cm_list_normalise_index(cm_list * list, int index) {

    if (index < 0) { 
        index = list->len + index;
    }

    return index;
}



static cm_list_node * _cm_list_traverse(cm_list * list, int index) {

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
        cm_errno = ERR_INTERNAL_INDEX;
        return NULL;
    }
    return node;
}



static cm_list_node * _cm_new_list_node(cm_list * list, cm_byte * data) {

    //allocate node structure
    cm_list_node * new_node = malloc(sizeof(cm_list_node));
    if (!new_node) {
        cm_errno = ERR_MALLOC;
        return NULL;
    }

    //allocate data
    new_node->data = malloc(list->data_size);
    if (!new_node) {
        cm_errno = ERR_MALLOC;
        return NULL;
    }

    //copy data into node
    memcpy(new_node->data, data, list->data_size);

    return new_node;
}



static void _cm_list_add_head_node(cm_list * list, cm_list_node * node) {

    node->next = node->prev = NULL;

    list->head = node;
    ++list->len;

    return;
}



static void _cm_list_add_node(cm_list * list, cm_list_node * node,
                   cm_list_node * prev_node, cm_list_node * next_node,
                   int index) {

    prev_node->next = node;
    node->prev = prev_node;

    next_node->prev = node;
    node->next = next_node;

    if (index == 0) list->head = node;
    ++list->len;

    return;
}



static void _cm_list_del_node(cm_list_node * node) {

    free(node->data);
    free(node);

    return;
}



static void _cm_list_sub_node(cm_list * list, 
                   cm_list_node * prev_node, cm_list_node * next_node, int index) {

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



static int _cm_list_empty(cm_list * list) {
 
    cm_list_node * node = list->head, * next_node;
    int index = list->len;

    while ((node != NULL) && (index != 0)) {

        next_node = node->next;
        _cm_list_del_node(node);
        node = next_node;
        --index;
    }

    if (index) {
        cm_errno = ERR_INTERNAL_INDEX;
        return -1;
    }

    list->len = 0;
    list->head = NULL;

    return 0;
}



static inline int _cm_list_assert_index_range(cm_list * list, int index, int mode) {
   
    /*
     *  if inserting, maximum index needs to be +1 higher than for other operations
     */
    if (abs(index) >= (list->len + mode)) {
        cm_errno = ERR_USER_INDEX;
        return -1;
    }

    return 0;
}



/*
 *  --- [EXTERNAL] ---
 */

int cm_list_get_val(cm_list * list, int index, cm_byte * buf) {

    if (_cm_list_assert_index_range(list, index, LIST_INDEX)) return -1;

    //get the node
    cm_list_node * node = _cm_list_traverse(list, index);
    if (!node) return -1;

    memcpy(buf, node->data, list->data_size);
    
    return 0;
}



cm_byte * cm_list_get_ref(cm_list * list, int index) {

    //get the node

    //get the node
    cm_list_node * node = _cm_list_traverse(list, index);
    if (!node) return NULL;

    return node->data;
}



int cm_list_set(cm_list * list, int index, cm_byte * data) {

    if (_cm_list_assert_index_range(list, index, LIST_INDEX)) return -1;

    //get the node
    cm_list_node * node = _cm_list_traverse(list, index);
    if (!node) return -1;

    memcpy(node->data, data, list->data_size);

    return 0;
}



int cm_list_insert(cm_list * list, int index, cm_byte * data) {

    cm_list_node * prev_node, * next_node;

    if (_cm_list_assert_index_range(list, index, LIST_ADD_INDEX)) return -1;

    //create new node
    cm_list_node * new_node = _cm_new_list_node(list, data);
    if (!new_node) return -1;

    //to simplify, convert a negative index to a positive equivalent
    index = _cm_list_normalise_index(list, index);
    
    //get the _prev_ and _next_ of the new node as required
    if (list->len == 0) {
        _cm_list_add_head_node(list, new_node);
    
    } else if (list->len == 1) { 
        next_node = prev_node = list->head; 
        
        _cm_list_add_node(list, new_node, prev_node, next_node, index);

    } else { 
        if (index == list->len) {
            prev_node = list->head->prev;
        } else {
            prev_node = _cm_list_traverse(list, index-1);
            if (!prev_node) {
                _cm_list_del_node(new_node);
                return -1;
            }
        }
        next_node = prev_node->next;

        _cm_list_add_node(list, new_node, prev_node, next_node, index);
    }

    return 0;
}



int cm_list_append(cm_list * list, cm_byte * data) {

    cm_list_node * new_node = _cm_new_list_node(list, data);
    if (!new_node) return -1;

    //add node to list
    if (list->len == 0) {
        _cm_list_add_head_node(list, new_node);
    
    } else if (list->len == 1) {
        _cm_list_add_node(list, new_node, list->head, list->head, -1);
    
    } else {
        _cm_list_add_node(list, new_node, list->head->prev, list->head, -1);
    }

    return 0;
}



int cm_list_remove(cm_list * list, int index) {

    
    if (_cm_list_assert_index_range(list, index, LIST_INDEX)) return -1;

    //get the node
    cm_list_node * del_node = _cm_list_traverse(list, index);
    if(!del_node) return -1;

    _cm_list_sub_node(list, del_node->prev, del_node->next, index);
    _cm_list_del_node(del_node);

    --list->len;

    return 0;
}



int cm_list_empty(cm_list * list) {

    if (_cm_list_empty(list)) return -1;

    return 0;
}



void cm_new_list(cm_list * list, size_t data_size) {

    list->len = 0;
    list->data_size = data_size;
    return;
}



int cm_del_list(cm_list * list) {

    if (_cm_list_empty(list)) return -1;

    return 0;
}
