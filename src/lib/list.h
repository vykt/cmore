#ifndef LIST_H
#define LIST_H

//system headers
#include <unistd.h>

//local headers
#include "cmore.h"
#include "debug.h"


//controls if user provided index should be verified for accessing elements
//or for adding new elements
enum _list_index_mode {INDEX = 0, ADD_INDEX = 1};


//internal
cm_list_node * _list_traverse(const cm_list * list, int index);
cm_list_node * _list_new_cm_list_node(const cm_list * list, const cm_byte * data);
void _list_del_cm_list_node(cm_list_node * node);
void _list_set_head_node(cm_list * list, cm_list_node * node);
void _list_add_node(cm_list * list, cm_list_node * node, cm_list_node * prev_node,
                    cm_list_node * next_node, const int index);
void _list_sub_node(cm_list * list, cm_list_node * prev_node, 
                    cm_list_node * next_node, const int index);
int _list_empty(cm_list * list);
int _list_assert_index_range(const cm_list * list, 
                             const int index, enum _list_index_mode mode);


//external
int cm_list_get_val(const cm_list * list, const int index, cm_byte * buf);
cm_byte * cm_list_get_ref(const cm_list * list, const int index);
cm_list_node * cm_list_get_node(const cm_list * list, const int index);

cm_list_node * cm_list_set(cm_list * list, const int index, const cm_byte * data);
cm_list_node * cm_list_insert(cm_list * list, const int index, const cm_byte * data);
cm_list_node * cm_list_append(cm_list * list, const cm_byte * data);

cm_list_node * cm_list_unlink(cm_list * list, const int index);
int cm_list_remove(cm_list * list, const int index);
int cm_list_empty(cm_list * list);

void cm_new_list(cm_list * list, const size_t data_size);
void cm_del_list(cm_list * list);
void cm_del_list_node(cm_list_node * node);

#endif
