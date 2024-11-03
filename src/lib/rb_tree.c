#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#include "libcmore.h"
#include "rb_tree.h"



/*
 *  --- [INTERNAL] ---
 */

/*
 *  Returns the node itself in case of a hit. Returns parent in case of a miss.
 */

static cm_rb_tree_node * _traverse(const cm_rb_tree * tree, 
                                   const cm_byte * key, 
                                   enum cm_rb_tree_eval * eval) {

    cm_rb_tree_node * node = tree->root;
    bool found = false;
    *eval = ROOT;


    //traverse tree
    while (node != NULL && !found) {

        *eval = tree->compare(key, node->key);

        switch (*eval) {

            case LESS:
                if (!node->left) return node;
                node = node->left;
                break;

            case EQUAL:
                found = true;
                break;

            case MORE:
                if (!node->right) return node;
                node = node->right;
                break;

            //makes clangd quiet
            case ROOT:
                break;

        } //end switch
    
    } //end while

    return node;
}



static cm_rb_tree_node * _new_cm_rb_tree_node(const cm_rb_tree * tree,
                                              const cm_byte * key, 
                                              const cm_byte * data) {

    //allocate node structure
    cm_rb_tree_node * new_node = malloc(sizeof(cm_rb_tree_node));
    if (!new_node) {
        cm_errno = CM_ERR_MALLOC;
        return NULL;
    }

    //allocate key
    new_node->key = malloc(tree->key_size);
    if (!new_node->key) {
        cm_errno = CM_ERR_MALLOC;
        return NULL;
    }

    //allocate data
    new_node->data = malloc(tree->data_size);
    if (!new_node->data) {
        cm_errno = CM_ERR_MALLOC;
        return NULL;
    }

    //copy data into node
    memcpy(new_node->data, data, tree->data_size);

    //null out parent until node is attached to tree
    new_node->parent      = NULL;

    //set colour to red
    new_node->colour = RED;

    return new_node;
}



static void _del_cm_rb_tree_node(cm_rb_tree_node * node) {

    free(node->key);
    free(node->data);
    free(node);

    return;
}



static inline void _set_root(cm_rb_tree * tree, cm_rb_tree_node * node) {

    node->colour      = BLACK;
    node->parent_eval = ROOT;
    
    tree->root = node;

    return;
}



static void _left_rotate(cm_rb_tree * tree, cm_rb_tree_node * node) {

    cm_rb_tree_node * right_child    = node->right;
    cm_rb_tree_node * parent         = node->parent;
    enum cm_rb_tree_eval parent_eval = node->parent_eval;

    //rotate node
    node->right       = node->right->left;
    node->parent      = right_child;
    node->parent_eval = LESS;

    //rotate node's former right child
    right_child->left        = node;
    right_child->parent      = parent;
    right_child->parent_eval = parent_eval;

    //rotate former right child's left child
    if (node->right != NULL) node->right->parent = node;

    //update tree's root node if necessary
    if (tree->root == node) _set_root(tree, right_child);

    return;
}



static void _right_rotate(cm_rb_tree * tree, cm_rb_tree_node * node) {

    cm_rb_tree_node * left_child     = node->left;
    cm_rb_tree_node * parent         = node->parent;
    enum cm_rb_tree_eval parent_eval = node->parent_eval;

    //rotate node
    node->left        = node->left->right;
    node->parent      = left_child;
    node->parent_eval = MORE;

    //rotate node's former right child
    left_child->right       = node;
    left_child->parent      = parent;
    left_child->parent_eval = parent_eval;

    //rotate former right child's left child
    if (node->left != NULL) node->left->parent = node;

    //update tree's root node if necessary
    if (tree->root == node) _set_root(tree, left_child);

    return;
}



/*
 *  If uncle is red, parent must be red & grandparent must be black. Set uncle and 
 *  parent to black, set grandparent to red
 */

//case 1
static inline void _case_1(const cm_rb_tree * tree, 
                           cm_rb_tree_node ** node, struct _fix_data * f_data) {

    //swap colours per red uncle case
    f_data->parent->colour = BLACK;
    f_data->uncle->colour  = BLACK;
    if (tree->root != f_data->grandparent) f_data->grandparent->colour = RED;

    //advance node to grandparent
    *node = f_data->grandparent;

    return;
}



/*
 *  If parent is red and grandparent is the root node, change parent to black
 */

//case 2
static inline void _case_2(const cm_rb_tree * tree, struct _fix_data * f_data) {

    f_data->parent->colour = BLACK;

    return;
}



/*
 *  Parent is red, grandparent is black and uncle is black. Nodes are organised
 *  such that if the node is a left child of the parent, then the parent is a 
 *  right child of the grandparent (or vice versa). Rotate the parent to make it 
 *  the child of the node. Transform into case 6.
 */

//case 3
static inline void _case_3(cm_rb_tree * tree,
                           cm_rb_tree_node ** node, struct _fix_data * f_data) {
    
    //node is left child
    if ((*node)->parent_eval == LESS) {
        _right_rotate(tree, f_data->parent);
        *node = (*node)->right;
    }

    //node is right child
    if ((*node)->parent_eval == MORE) {
        _left_rotate(tree, f_data->parent);
        *node = (*node)->left;
    }

    return;
}



/*
 *  Parent is red, grandparent is black and uncle is black. Nodes are organised 
 *  such that if the node is left child of the parent, then the parent is also a 
 *  left child of the grandparent (or vice versa). Rotate the grandparent to make 
 *  it the child of the parent.
 */

//case 4
static inline void _case_4(cm_rb_tree * tree,
                           cm_rb_tree_node ** node, struct _fix_data * f_data) {

    //node is left child
    if ((*node)->parent_eval == LESS)
        _right_rotate(tree, f_data->grandparent);

    //node is right child
    if ((*node)->parent_eval == MORE)
        _left_rotate(tree, f_data->grandparent);

    //recolour parent and grandparent
    f_data->grandparent->colour = RED;
    f_data->parent->colour      = BLACK;

    return;
}



static inline void _populate_fix_data(const cm_rb_tree_node * node,
                                      struct _fix_data * f_data) {

    memset(f_data, 0, sizeof(*f_data));
    f_data->parent = node->parent;
    
    if (f_data->parent != NULL) {
        
        f_data->grandparent = f_data->parent->parent;
    
        if (f_data->parent->parent_eval == LESS)
            f_data->uncle = f_data->grandparent->right;
        if (f_data->parent->parent_eval == MORE)
            f_data->uncle = f_data->grandparent->left;
    }

    return;
}



static inline int _determine_case(const cm_rb_tree * tree, 
                                  const cm_rb_tree_node * node,
                                  const struct _fix_data * f_data) {

    //if parent is black, no fix necessary
    if (f_data->parent->colour == BLACK) return 0;

    //if grandparent is root, case 2
    if (tree->root == f_data->grandparent) return 2;

    //if uncle is red, case 1
    if (f_data->uncle == RED) return 1;

    //if red nodes form a 'line', case 4
    if (node->parent_eval == f_data->parent->parent_eval) return 4;

    //if red nodes form a 'triange', case 3
    if (node->parent_eval != f_data->parent->parent_eval) return 3;

    //otherwise invalid state, error out
    cm_errno = CM_ERR_RB_INVALID_STATE;
    return -1;
}



static int _fix_insert(cm_rb_tree * tree, cm_rb_tree_node * node) {

    int fix_case;
    struct _fix_data f_data;

    //move up tree and correct violations until fixed or root is reached
    while (node != tree->root) {

        _populate_fix_data(node, &f_data);
        
        //if fix case is 0, no further corrections necessary
        fix_case = _determine_case(tree, node, &f_data);
        if (fix_case <= 0) return fix_case; //-1 or 0
        
        /*
         *  indexable call table doesn't make sense here
         */

        //dispatch fix cases
        switch (fix_case) {

            case 1:
                _case_1(tree, &node, &f_data);
                break;
            
            case 2:
                _case_2(tree, &f_data);
                break;

            case 3:
                _case_3(tree, &node, &f_data);
                _case_4(tree, &node, &f_data);
                break;;

            case 4:
                _case_4(tree, &node, &f_data);
                break;
        } //end switch 

    } //end while

    return 0;
}



static int _add_node(cm_rb_tree * tree, cm_rb_tree_node * node, 
                     cm_rb_tree_node * parent, const enum cm_rb_tree_eval eval) {

    int ret;

    //if tree is empty, set root
    if (tree->size == 0) {
        _set_root(tree, node);
    
    //else connect node
    } else {
        if (eval == LESS) parent->left = node;
        if (eval == MORE) parent->right = node;
    }

    //increment tree size
    tree->size += 1;

    //fix violations
    return _fix_insert(tree, node);
}



/*
 *  --- [EXTERNAL] ---
 */

int cm_rb_tree_get_val(const cm_rb_tree * tree, const cm_byte * key, cm_byte * buf) {

    enum cm_rb_tree_eval eval;

    //get the node
    cm_rb_tree_node * node = _traverse(tree, key, &eval);
    if (eval != EQUAL) {
        
        cm_errno = CM_ERR_USER_KEY;
        return -1;
    }

    memcpy(buf, node->data, tree->data_size);

    return 0;
}



cm_byte * cm_rb_tree_get_ref(const cm_rb_tree * tree, const cm_byte * key) {

    enum cm_rb_tree_eval eval;

    //get the node
    cm_rb_tree_node * node = _traverse(tree, key, &eval);
    if (eval != EQUAL) {
    
        cm_errno = CM_ERR_USER_KEY;
        return NULL;
    }

    return node->data;
}



cm_rb_tree_node * cm_rb_tree_get_node(const cm_rb_tree * tree, const cm_byte * key) {

    enum cm_rb_tree_eval eval;

    //get the node
    cm_rb_tree_node * node = _traverse(tree, key, &eval);
    if (eval != EQUAL) {
        
        cm_errno = CM_ERR_USER_KEY;
        return NULL;
    }

    return node;
}



cm_rb_tree_node * cm_rb_tree_set(cm_rb_tree * tree,
                                 const cm_byte * key, const cm_byte * data) {

    int ret;

    enum cm_rb_tree_eval eval;
    cm_rb_tree_node * new_node;

    //get relevant node
    cm_rb_tree_node * node = _traverse(tree, key, &eval);

    //if a node already exists for this key, update its value
    if (eval == EQUAL) {
        memcpy(node->data, data, tree->data_size);
        return node;

    //else create a new node
    } else { 
        new_node = _new_cm_rb_tree_node(tree, key, data);
        ret = _add_node(tree, new_node, node, eval);
        if (ret == -1) return NULL;
        return new_node;
    }
}
