#ifndef RB_TREE_H
#define RB_TREE_H

#include <unistd.h>

#include "libcmore.h"



//stores pointers to nodes relevant for correction operations
struct _fix_data {

    cm_rb_tree_node * parent;
    cm_rb_tree_node * grandparent;
    cm_rb_tree_node * uncle;
};


int cm_rb_tree_get_val(const cm_rb_tree * tree, const cm_byte * key, cm_byte * buf);
cm_byte * cm_rb_tree_get_ref(const cm_rb_tree * tree, const cm_byte * key);
cm_rb_tree_node * cm_rb_tree_get_node(const cm_rb_tree * tree, const cm_byte * key);
cm_rb_tree_node * cm_rb_tree_set(cm_rb_tree * tree,
                                 const cm_byte * key, const cm_byte * data);
int cm_rb_tree_remove(cm_rb_tree * tree, const cm_byte * key);
int cm_rb_tree_unlink(cm_rb_tree * tree, const cm_byte * key);
int cm_rb_tree_empty(cm_rb_tree * tree);

void cm_new_rb_tree(cm_rb_tree * tree, const size_t data_size);
void cm_del_rb_tree(cm_rb_tree * tree);


#endif
