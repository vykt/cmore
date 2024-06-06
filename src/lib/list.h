#ifndef LIST_H
#define LIST_H

#include <unistd.h>

#include "libcmore.h"


#define LIST_ADD_INDEX 1
#define LIST_INDEX 0


int cm_list_get_val(cm_list * list, int index, cm_byte * buf);
cm_byte * cm_list_get_ref(cm_list * list, int index);

int cm_list_set(cm_list * list, int index, cm_byte * data);
int cm_list_insert(cm_list * list, int index, cm_byte * data);
int cm_list_append(cm_list * list, cm_byte * data);
int cm_list_remove(cm_list * list, int index);
int cm_list_empty(cm_list * list);

void cm_new_list(cm_list * list, size_t data_size);
int cm_del_list(cm_list * list);


#endif
