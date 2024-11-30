#ifndef RB_TREE_H
#define RB_TREE_H

//system headers
#include <unistd.h>

//local headers
#include "cmore.h"
#include "debug.h"


//stores pointers to nodes relevant for correction operations
struct _rb_tree_fix_data {

    cm_rb_tree_node * grandparent;
    cm_rb_tree_node * parent;
    cm_rb_tree_node * uncle;
    cm_rb_tree_node * sibling;
};


#ifdef DEBUG
//internal
cm_rb_tree_node * _rb_tree_traverse(const cm_rb_tree * tree, const cm_byte * key,
                                    enum cm_rb_tree_eval * eval);
cm_rb_tree_node * _rb_tree_new_node(const cm_rb_tree * tree,
                                    const cm_byte * key, const cm_byte * data);
void _rb_tree_del_node(cm_rb_tree_node * node);
void _rb_tree_left_rotate(cm_rb_tree * tree, cm_rb_tree_node * node);
void _rb_tree_right_rotate(cm_rb_tree * tree, cm_rb_tree_node * node);
void _rb_tree_transplant(cm_rb_tree * tree, 
                         cm_rb_tree_node * subj_node, cm_rb_tree_node * tgt_node);
cm_rb_tree_node * _rb_tree_right_min(cm_rb_tree_node * node);
enum cm_rb_tree_colour _rb_tree_get_colour(const cm_rb_tree_node * node);
void _rb_tree_ins_case_1(const cm_rb_tree * tree, cm_rb_tree_node ** node,
                         struct _rb_tree_fix_data * f_data);
void _rb_tree_ins_case_2(struct _rb_tree_fix_data * f_data);
void _rb_tree_ins_case_3(cm_rb_tree * tree, cm_rb_tree_node ** node,
                         struct _rb_tree_fix_data * f_data);
void _rb_tree_ins_case_4(cm_rb_tree * tree, cm_rb_tree_node ** node,
                         struct _rb_tree_fix_data * f_data);
void _rb_tree_rem_case_1(cm_rb_tree * tree, struct _rb_tree_fix_data * f_data);
void _rb_tree_rem_case_2(cm_rb_tree_node ** node, struct _rb_tree_fix_data * f_data);
void _rb_tree_rem_case_3(cm_rb_tree * tree, struct _rb_tree_fix_data * f_data);
void _rb_tree_rem_case_4(cm_rb_tree * tree, cm_rb_tree_node ** node, 
                         struct _rb_tree_fix_data * f_data);
void _rb_tree_populate_fix_data(const cm_rb_tree_node * node,
                                struct _rb_tree_fix_data * f_data);
int _rb_tree_determine_ins_case(const cm_rb_tree_node * node,
                                const struct _rb_tree_fix_data * f_data);
int _rb_tree_determine_rem_case(const struct _rb_tree_fix_data * f_data);
int _rb_tree_fix_insert(cm_rb_tree * tree, cm_rb_tree_node * node);
int _rb_tree_fix_remove(cm_rb_tree * tree, cm_rb_tree_node * node, 
                        struct _rb_tree_fix_data * f_data);
cm_rb_tree_node * _rb_tree_add_node(cm_rb_tree * tree, const cm_byte * key, 
                                    const cm_byte * data, cm_rb_tree_node * parent, 
                                    const enum cm_rb_tree_eval eval);
int _rb_tree_remove_node(cm_rb_tree * tree, cm_rb_tree_node * node);
void _rb_tree_empty_recurse(cm_rb_tree_node * node);
cm_rb_tree_node * _rb_tree_unlink_node(cm_rb_tree * tree, const cm_byte * key);
#endif


//external
int cm_rb_tree_get_val(const cm_rb_tree * tree, const cm_byte * key, cm_byte * buf);
cm_byte * cm_rb_tree_get_ref(const cm_rb_tree * tree, const cm_byte * key);
cm_rb_tree_node * cm_rb_tree_get_node(const cm_rb_tree * tree, const cm_byte * key);
cm_rb_tree_node * cm_rb_tree_set(cm_rb_tree * tree,
                                 const cm_byte * key, const cm_byte * data);
int cm_rb_tree_remove(cm_rb_tree * tree, const cm_byte * key);
int cm_rb_tree_unlink(cm_rb_tree * tree, const cm_byte * key);
void cm_rb_tree_empty(cm_rb_tree * tree);

void cm_new_rb_tree(cm_rb_tree * tree, const size_t key_size, const size_t data_size,
                    enum cm_rb_tree_eval (*compare)(const cm_byte *, const cm_byte *));
void cm_del_rb_tree(cm_rb_tree * tree);
void cm_del_rb_tree_node(cm_rb_tree_node * node);

#endif
