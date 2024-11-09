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
    if (node->right != NULL) {
        node->right->parent = node;
        node->right->parent_eval = node->right->parent_eval == MORE ? LESS : MORE;
    }

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
    if (node->left != NULL) {
        node->left->parent = node;
        node->right->parent_eval = node->right->parent_eval == MORE ? LESS : MORE;
    }

    //update tree's root node if necessary
    if (tree->root == node) _set_root(tree, left_child);

    return;
}



static void _transplant(cm_rb_tree * tree, 
                        cm_rb_tree_node * subj_node, cm_rb_tree_node * tgt_node) {

    //if deleting root, disconnect subject and target nodes and set target node as root
    if (tree->root == subj_node) {

        tree->root            = tgt_node;
        tgt_node->parent      = NULL;
        tgt_node->parent_eval = ROOT;
   
    } else {
        
        if (subj_node->parent_eval == MORE) subj_node->parent->right = tgt_node;
        if (subj_node->parent_eval == LESS) subj_node->parent->left  = tgt_node;
        
        if (tgt_node != NULL) {
            tgt_node->parent      = subj_node->parent;
            tgt_node->parent_eval = subj_node->parent_eval;
        }
    }

    return;
}



static cm_rb_tree_node * _right_min(cm_rb_tree_node * node) {

    //bootstrap min traversal
    if (node->right == NULL) return node;
    node = node->right;

    //iterate until min reached
    while (node->left != NULL) {
        node = node->left;
    }

    //return min
    return node;
}



//get colour of node's children
static void _get_child_colours(const cm_rb_tree_node * node,
                               enum cm_rb_tree_colour * left,
                               enum cm_rb_tree_colour * right) {

    //get sibling's left child's colour
    if (node->left == NULL) {
        *left = BLACK;
    } else {
        *left = node->left->colour == BLACK ? BLACK : RED;
    }

    //get sibling's right child's colour
    if (node->right == NULL) {
        *right = BLACK;
    } else {
        *right = node->right->colour == BLACK ? BLACK : RED;
    }

    return;
}



/*
 *  If uncle is red, parent must be red & grandparent must be black. Set uncle and 
 *  parent to black, set grandparent to red.
 */

//insert case 1
static inline void _ins_case_1(const cm_rb_tree * tree, 
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
 *  If parent is red and grandparent is the root node, change parent to black.
 */

//insert case 2
static inline void _ins_case_2(const cm_rb_tree * tree, struct _fix_data * f_data) {

    f_data->parent->colour = BLACK;

    return;
}



/*
 *  Parent is red, grandparent is black and uncle is black. Nodes are organised
 *  such that if the node is a left child of the parent, then the parent is a 
 *  right child of the grandparent (or vice versa). Rotate the parent to make it 
 *  the child of the node. Transform into case 6.
 */

//insert case 3
static inline void _ins_case_3(cm_rb_tree * tree,
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

//insert case 4
static inline void _ins_case_4(cm_rb_tree * tree,
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



/*
 *  Sibling is red.
 *
 *  Swap sibling and parent colours, then rotate parent left to make sibling 
 *  the new grandparent.
 */

//remove case 1
static inline void _rem_case_1(cm_rb_tree * tree,
                               cm_rb_tree_node * node, struct _fix_data * f_data) {

    //recolour sibling and parent
    f_data->sibling->colour = BLACK;
    f_data->parent->colour  = RED;

    //rotate parent to make sibling the new grandparent
    _left_rotate(tree, f_data->parent);

    return;
}



/*
 *  Sibling is black, both of sibling's children are black. Set sibling to red and 
 *  advance target node up to parent.
 */

//remove case 2
static inline void _rem_case_2(cm_rb_tree * tree,
                               cm_rb_tree_node ** node, struct _fix_data * f_data) {

    f_data->sibling->colour = RED;
    *node = (*node)->parent; 

    return;
}



/*
 *  Sibling is black, sibling's left child is red, sibling's right child is black.
 *  Swap sibling's and sibling's left child's colours, then right rotate sibling.
 */

//remove case 3
static inline void _rem_case_3(cm_rb_tree * tree,
                               cm_rb_tree_node * node, struct _fix_data * f_data) {

    f_data->sibling->left->colour = BLACK;
    f_data->sibling->colour       = RED;

    _right_rotate(tree, f_data->sibling);

    return;
}



/*
 *  Sibling is black, sibling's right child is red. Set sibling's colour to 
 *  parent's colour. Set sibling's right child's colour to black. Set parent's 
 *  colour to black. Then left rotate parent and advance node up to root.
 */

//remove case 4
static inline void _rem_case_4(cm_rb_tree * tree,
                               cm_rb_tree_node ** node, struct _fix_data * f_data) {

    //recolour nodes
    f_data->sibling->colour        = f_data->parent->colour;
    f_data->sibling->right->colour = BLACK;
    f_data->parent->colour         = BLACK;

    //rotate parent left
    _left_rotate(tree, f_data->parent);

    *node = tree->root;

    return;
}



static inline void _populate_fix_data(const cm_rb_tree_node * node,
                                          struct _fix_data * f_data) {

    memset(f_data, 0, sizeof(*f_data));
    
    //if unable to get parent, then unable to get everything
    if (node->parent == NULL) return;
    
    //get parent & grandparent
    f_data->parent = node->parent;
    f_data->grandparent = f_data->parent->parent;

    //get uncle
    if (f_data->parent->parent_eval == LESS)
        f_data->uncle = f_data->grandparent->right;
    if (f_data->parent->parent_eval == MORE)
        f_data->uncle = f_data->grandparent->left;

    //get sibling
    if (f_data->parent->parent_eval == LESS)
        f_data->sibling = f_data->parent->right;
    if (f_data->parent->parent_eval == MORE)
        f_data->sibling = f_data->parent->left;

    return;
}



//determines which case applies for inserting nodes
static inline int _determine_ins_case(const cm_rb_tree * tree, 
                                      const cm_rb_tree_node * node,
                                      const struct _fix_data * f_data) {

    bool uncle_black;

    //if parent is black, no fix necessary
    if (f_data->parent->colour == BLACK) return 0;

    //if uncle is red, case 1
    if (f_data->uncle != NULL)
        if (f_data->uncle->colour == RED) return 1;

    //determine if uncle is black
    uncle_black = (f_data->uncle == NULL);
    if (f_data->uncle != NULL) {
        if (f_data->uncle->colour == BLACK) uncle_black = true;
        if (f_data->uncle->colour == RED) uncle_black |= true;
    }

    //determine 'triangle' or 'line' case
    if (uncle_black) {

        //if red nodes form a 'triange', case 3
        if (node->parent_eval != f_data->parent->parent_eval) return 3;

        //if red nodes form a 'line', case 4
        if (node->parent_eval == f_data->parent->parent_eval) return 4;

    }

    //if grandparent is root, case 2
    if (tree->root == f_data->grandparent) return 2;

    //otherwise invalid state, error out
    cm_errno = CM_ERR_RB_INVALID_STATE;
    return -1;
}



//determines which case applies for removing nodes
static inline int _determine_rem_case(const cm_rb_tree * tree,
                                      const cm_rb_tree_node * node,
                                      const struct _fix_data * f_data) {
    
    enum cm_rb_tree_colour left_colour;
    enum cm_rb_tree_colour right_colour;

    //if sibling doesn't exist, no fix necessary
    if (f_data->sibling == NULL) return 0;

    //if sibling is red, case 1
    if (f_data->sibling->colour == RED) return 1;

    //get sibling's child colours
    _get_child_colours(f_data->sibling, &left_colour, &right_colour);

    //if both sibling's children are black, case 2
    if ((left_colour == BLACK) && (right_colour == BLACK)) return 2;

    //if sibling's left child is red and right child is black, case 3
    if ((left_colour == RED) && (right_colour == BLACK)) return 3;

    //if sibling's right child's colour is red, case 4
    if (right_colour == RED) return 4;

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
        fix_case = _determine_ins_case(tree, node, &f_data);
        if (fix_case <= 0) return fix_case; //-1 or 0
        
        /*
         *  indexable call table doesn't make sense here
         */

        //dispatch fix cases
        switch (fix_case) {

            case 1:
                _ins_case_1(tree, &node, &f_data);
                break;
            
            case 2:
                _ins_case_2(tree, &f_data);
                return 0;

            case 3:
                _ins_case_3(tree, &node, &f_data);
                _ins_case_4(tree, &node, &f_data);
                return 0;

            case 4:
                _ins_case_4(tree, &node, &f_data);
                return 0;
        
        } //end switch 

    } //end while

    return 0;
}



static int _fix_remove(cm_rb_tree * tree, 
                       cm_rb_tree_node * node, struct _fix_data * f_data) {

    int fix_case;

    //move up tree and correct violations until fixed or root is reached
    while (node != tree->root && node->colour == BLACK) {

        //determine remove case
        fix_case = _determine_rem_case(tree, node, f_data);
        if (fix_case <= 0) return fix_case; //-1 or 0

        //dispatch remove case
        switch(fix_case) {

            case 1:
                _rem_case_1(tree, node, f_data);
                break;

            case 2:
                _rem_case_2(tree, &node, f_data);

            case 3:
                _rem_case_3(tree, node, f_data);
                _populate_fix_data(node, f_data);
                _rem_case_4(tree, &node, f_data);
                return 0;
            
            case 4:
                _rem_case_4(tree, &node, f_data);
                return 0;

        }

        //update fix_data struct for next iteration
        _populate_fix_data(node, f_data); 

    } //end while

    return 0;
}



static inline int _add_node(cm_rb_tree * tree, 
                            const cm_byte * key, const cm_byte * data, 
                            cm_rb_tree_node * parent, 
                            const enum cm_rb_tree_eval eval) {
    int ret;

    //create new node
    cm_rb_tree_node * node = _new_cm_rb_tree_node(tree, key, data);

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



static int _remove_node(cm_rb_tree * tree, cm_rb_tree_node * node) {
 
    int ret;
    
    cm_rb_tree_node * min_node;
    struct _fix_data f_data;


    //node has no children
    if (node->left == NULL && node->right == NULL) {

        //bootstrap fix_data if node removal will require fixes
        if (node->colour == BLACK) _populate_fix_data(node, &f_data);

    //node only has a right child
    } else if (node->left == NULL) {

        _populate_fix_data(node, &f_data);
        _transplant(tree, node, node->right);

    //node only has a left child
    } else if (node->right == NULL) {

        _populate_fix_data(node, &f_data);
        _transplant(tree, node, node->left);

    //node has both a left and a right child
    } else {
       
        //get minimum node in right subtree
        min_node = _right_min(node);
        _populate_fix_data(min_node, &f_data);

        //cut minimum node from right subtree
        _transplant(tree, min_node, min_node->right);
        
        //re-attach minimum node as root of right subtree
        min_node->right              = node->right;
        min_node->right->parent      = min_node;
        min_node->right->parent_eval = MORE;

        //set minimum node as new root of whole tree
        _transplant(tree, node, min_node);

        //re-attach minimum node to left subtree
        min_node->left              = node->left;
        min_node->left->parent      = min_node;
        min_node->left->parent_eval = LESS;
    }

    //remove node from tree
    tree->size -= 1;

    //if a black node was removed, must correct tree
    if (node->colour == BLACK) ret = _fix_remove(tree, node, &f_data);
    if (ret == -1) return -1;
    
    return 0;
}



static void _empty_recurse(cm_rb_tree_node * node) {

    if (node == NULL) return;
    if (node->left != NULL) _del_cm_rb_tree_node(node->left);
    if (node->right != NULL) _del_cm_rb_tree_node(node->right);

    return;
}



static cm_rb_tree_node * _unlink_node(cm_rb_tree * tree, const cm_byte * key) {

    int ret;

    enum cm_rb_tree_eval eval;

    //get relevant node
    cm_rb_tree_node * node = _traverse(tree, key, &eval);

    //if a node doesn't exist for this key, error out
    if (eval != EQUAL || node == NULL) {
        cm_errno = CM_ERR_USER_KEY;
        return NULL;
    }

    //remove node
    ret = _remove_node(tree, node);
    if (ret == -1) return NULL;

    return node;
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
        ret = _add_node(tree, key, data, node, eval);
        if (ret == -1) return NULL;
        return new_node;
    }
}



int cm_rb_tree_remove(cm_rb_tree * tree, const cm_byte * key) {

    //get relevant node
    cm_rb_tree_node * node = _unlink_node(tree, key);
    if (node == NULL) return -1;

    _del_cm_rb_tree_node(node);

    return 0;
}



int cm_rb_tree_unlink(cm_rb_tree * tree, const cm_byte * key) {

    //get relevant node
    cm_rb_tree_node * node = _unlink_node(tree, key);
    if (node == NULL) return -1;

    return 0;
}



void cm_rb_tree_empty(cm_rb_tree * tree) {

    _empty_recurse(tree->root);

    return;
}



void cm_new_rb_tree(cm_rb_tree * tree, const size_t data_size,
                    enum cm_rb_tree_eval (*compare)
                    (const cm_byte *, const cm_byte *)) {

    tree->size      = 0;
    tree->data_size = data_size;
    tree->root      = NULL;

    return;
}



void cm_del_rb_tree(cm_rb_tree * tree) {

    _empty_recurse(tree->root);

    return;
}
