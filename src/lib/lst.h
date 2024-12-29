#ifndef LST_H
#define LST_H

//system headers
#include <unistd.h>

//local headers
#include "cmore.h"
#include "debug.h"


//controls if user provided index should be verified for accessing elements
//or for adding new elements
enum _lst_index_mode {INDEX = 0, ADD_INDEX = 1};


#ifdef DEBUG
//internal
cm_lst_node * _lst_traverse(const cm_lst * list, int index);

cm_lst_node * _lst_new_node(const cm_lst * list, const void * data);
void _lst_del_node(cm_lst_node * node);

void _lst_set_head_node(cm_lst * list, cm_lst_node * node);
void _lst_add_node(cm_lst * list, 
                   cm_lst_node * node, cm_lst_node * prev_node, 
                   cm_lst_node * next_node, const int index);
void _lst_sub_node(cm_lst * list, cm_lst_node * prev_node, 
                   cm_lst_node * next_node, const int index);

int _lst_emp(cm_lst * list);
int _lst_assert_index_range(const cm_lst * list, 
                            const int index, enum _lst_index_mode mode);
#endif


//external
int cm_lst_get(const cm_lst * list, const int index, void * buf);
void * cm_lst_get_p(const cm_lst * list, const int index);
cm_lst_node * cm_lst_get_n(const cm_lst * list, const int index);

cm_lst_node * cm_lst_set(cm_lst * list, 
                         const int index, const void * data);
cm_lst_node * cm_lst_set_n(cm_lst * list,
                           cm_lst_node * node, const void * data);

cm_lst_node * cm_lst_ins(cm_lst * list, 
                         const int index, const void * data);
cm_lst_node * cm_lst_ins_nb(cm_lst * list, 
                            cm_lst_node * node, const void * data);
cm_lst_node * cm_lst_ins_na(cm_lst * list,
                            cm_lst_node * node, const void * data);

cm_lst_node * cm_lst_apd(cm_lst * list, const void * data);

cm_lst_node * cm_lst_uln(cm_lst * list, const int index);
cm_lst_node * cm_lst_uln_n(cm_lst * list, cm_lst_node * node);

int cm_lst_rmv(cm_lst * list, const int index);
int cm_lst_rmv_n(cm_lst * list, cm_lst_node * node);

int cm_lst_emp(cm_lst * list);

void cm_new_lst(cm_lst * list, const size_t data_sz);
void cm_del_lst(cm_lst * list);
void cm_del_lst_node(cm_lst_node * node);

#endif
