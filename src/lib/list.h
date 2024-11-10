#ifndef LIST_H
#define LIST_H

#include <unistd.h>

#include "cmore.h"


//controls if user provided index should be verified for accessing elements
//or for adding new elements
enum _index_mode {INDEX = 0, ADD_INDEX = 1};


//function declarations
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


#endif
