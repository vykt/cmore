#ifndef RBT_H
#define RBT_H

//system headers
#include <unistd.h>

//local headers
#include "cmore.h"
#include "debug.h"


// -- [red-black tree]

//stores pointers to nodes relevant for correction operations
struct _rbt_fix_data {

    cm_rbt_node * grandparent;
    cm_rbt_node * parent;
    cm_rbt_node * uncle;
    cm_rbt_node * sibling;
};


#ifdef CM_DEBUG
//internal
cm_rbt_node * _rbt_traverse(const cm_rbt * tree, 
                            const void * key, enum cm_rbt_side * side);

cm_rbt_node * _rbt_new_node(const cm_rbt * tree, 
                            const void * key, const void * data);
void _rbt_del_node(cm_rbt_node * node);

void _rbt_left_rotate(cm_rbt * tree, cm_rbt_node * node);
void _rbt_right_rotate(cm_rbt * tree, cm_rbt_node * node);
void _rbt_transplant(cm_rbt * tree, 
                     cm_rbt_node * subj_node, cm_rbt_node * tgt_node);

cm_rbt_node * _rbt_left_max(cm_rbt_node * node);
enum cm_rbt_colour _rbt_get_colour(const cm_rbt_node * node);

void _rbt_populate_fix_data(const cm_rbt_node * node,
                            struct _rbt_fix_data * f_data);

void _rbt_ins_case_1(cm_rbt * tree, 
                     cm_rbt_node ** node, struct _rbt_fix_data * f_data);
void _rbt_ins_case_2(cm_rbt * tree, 
                     cm_rbt_node ** node, struct _rbt_fix_data * f_data);
void _rbt_ins_case_3(cm_rbt * tree, 
                     cm_rbt_node ** node, struct _rbt_fix_data * f_data);
void _rbt_ins_case_4(cm_rbt * tree, 
                     cm_rbt_node ** node, struct _rbt_fix_data * f_data);
void _rbt_rmv_case_1(cm_rbt * tree, 
                     cm_rbt_node ** node, struct _rbt_fix_data * f_data);
void _rbt_rmv_case_2(cm_rbt * tree, 
                     cm_rbt_node ** node, struct _rbt_fix_data * f_data);
void _rbt_rmv_case_3(cm_rbt * tree, 
                     cm_rbt_node ** node, struct _rbt_fix_data * f_data);
void _rbt_rmv_case_4(cm_rbt * tree, 
                     cm_rbt_node ** node, struct _rbt_fix_data * f_data);

int _rbt_determine_ins_case(const cm_rbt_node * node,
                            const struct _rbt_fix_data * f_data);
int _rbt_determine_rmv_case(const cm_rbt * tree, const cm_rbt_node * node,
                            const struct _rbt_fix_data * f_data);

int _rbt_fix_ins(cm_rbt * tree, cm_rbt_node * node);
int _rbt_fix_rmv(cm_rbt * tree, cm_rbt_node * node, 
                 enum cm_rbt_colour node_colour, 
                 struct _rbt_fix_data * f_data);

cm_rbt_node * _rbt_add_node(cm_rbt * tree, const void * key, 
                            const void * data, cm_rbt_node * parent, 
                            const enum cm_rbt_side side, bool is_raw,
                            enum cm_rbt_colour colour);
cm_rbt_node * _rbt_uln_node(cm_rbt * tree, const void * key);

void _rbt_emp_recurse(cm_rbt_node * node);
#endif


//external
int cm_rbt_get(const cm_rbt * tree, const void * key, void * buf);
void * cm_rbt_get_p(const cm_rbt * tree, const void * key);
cm_rbt_node * cm_rbt_get_n(const cm_rbt * tree, const void * key);

int cm_rbt_idx_get(const cm_rbt * tree, const int idx, void * buf);
void * cm_rbt_idx_get_p(const cm_rbt * tree, const int idx);
cm_rbt_node * cm_rbt_idx_get_n(const cm_rbt * tree, const int idx);

cm_rbt_node * cm_rbt_set(cm_rbt * tree, 
                         const void * key, const void * data);
int cm_rbt_rmv(cm_rbt * tree, const void * key);
cm_rbt_node * cm_rbt_uln(cm_rbt * tree, const void * key);
void cm_rbt_emp(cm_rbt * tree);
int cm_rbt_cpy(cm_rbt * dst_tree, const cm_rbt * src_tree);
void cm_rbt_mov(cm_rbt * dst_tree, cm_rbt * src_tree);

void cm_new_rbt(cm_rbt * tree, const size_t key_sz, const size_t data_sz, 
                enum cm_rbt_side (*compare)(const void *, const void *));
void cm_del_rbt(cm_rbt * tree);
void cm_del_rbt_node(cm_rbt_node * node);

#endif
