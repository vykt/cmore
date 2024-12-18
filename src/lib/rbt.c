//standard library
#include <stdlib.h>
#include <string.h>

//system headers
#include <unistd.h>

//local headers
#include "cmore.h"
#include "rbt.h"



/*
 *  TODO: Future refactor should consider creating indexable function tables
 *        for both insert and remove cases, and call these functions using
 *        an index returned by `determine_{ins,rem}_case()`.
 */



/*
 *  --- [INTERNAL] ---
 */

/*
 *  Returns the node itself in case of a hit. Returns parent in case of a miss.
 */

DBG_STATIC 
cm_rbt_node * _rbt_traverse(const cm_rbt * tree, 
                            const void * key, enum cm_rbt_side * side) {

    cm_rbt_node * node = tree->root;
    bool found = false;
    *side = ROOT;


    //traverse tree
    while (node != NULL && !found) {

        *side = tree->compare(key, node->key);

        switch (*side) {

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



DBG_STATIC 
cm_rbt_node * _rbt_new_node(const cm_rbt * tree,
                            const void * key, const void * data) {

    //allocate node structure
    cm_rbt_node * new_node = malloc(sizeof(cm_rbt_node));
    if (!new_node) {
        cm_errno = CM_ERR_MALLOC;
        return NULL;
    }

    //allocate key
    new_node->key = malloc(tree->key_sz);
    if (!new_node->key) {
        cm_errno = CM_ERR_MALLOC;
        return NULL;
    }

    //allocate data
    new_node->data = malloc(tree->data_sz);
    if (!new_node->data) {
        cm_errno = CM_ERR_MALLOC;
        return NULL;
    }

    //copy the key into the node
    memcpy(new_node->key, key, tree->key_sz);

    //copy the data into the node
    memcpy(new_node->data, data, tree->data_sz);

    //null out pointers
    new_node->parent = NULL;
    new_node->left   = NULL;
    new_node->right  = NULL;

    //set colour to red
    new_node->colour = RED;

    return new_node;
}



DBG_STATIC 
void _rbt_del_node(cm_rbt_node * node) {

    free(node->key);
    free(node->data);
    free(node);

    return;
}



DBG_STATIC DBG_INLINE 
void _rbt_set_root(cm_rbt * tree, cm_rbt_node * node) {

    node->colour      = BLACK;
    node->parent_side = ROOT;    
    tree->root = node;

    return;
}



DBG_STATIC 
void _rbt_left_rotate(cm_rbt * tree, cm_rbt_node * node) {

    cm_rbt_node * right_child    = node->right;
    cm_rbt_node * parent         = node->parent;
    enum cm_rbt_side parent_side = node->parent_side;

    //rotate node
    node->right       = node->right->left;
    node->parent      = right_child;
    node->parent_side = LESS;

    //rotate node's former right child
    right_child->left        = node;
    right_child->parent      = parent;
    right_child->parent_side = parent_side;

    //rotate former right child's left child
    if (node->right != NULL) {
        node->right->parent = node;
        node->right->parent_side 
            = node->right->parent_side == MORE ? LESS : MORE;
    }

    //update parent
    if (tree->root == node) {    
        _rbt_set_root(tree, right_child);
    
    } else {
        if (parent_side == LESS) parent->left = right_child;
        if (parent_side == MORE) parent->right = right_child;
    }

    return;
}



DBG_STATIC 
void _rbt_right_rotate(cm_rbt * tree, cm_rbt_node * node) {

    cm_rbt_node * left_child     = node->left;
    cm_rbt_node * parent         = node->parent;
    enum cm_rbt_side parent_side = node->parent_side;

    //rotate node
    node->left        = node->left->right;
    node->parent      = left_child;
    node->parent_side = MORE;

    //rotate node's former right child
    left_child->right       = node;
    left_child->parent      = parent;
    left_child->parent_side = parent_side;

    //rotate former right child's left child
    if (node->left != NULL) {
        node->left->parent = node;
        node->left->parent_side 
            = node->left->parent_side == MORE ? LESS : MORE;
    }

    //update parent
    if (tree->root == node) {
        _rbt_set_root(tree, left_child);
    
    } else {
        if (parent_side == LESS) parent->left = left_child;
        if (parent_side == MORE) parent->right = left_child;
    }

    return;
}



/*
 *  transplant() can only be called on subject nodes with a single child.
 */

DBG_STATIC 
void _rbt_transplant(cm_rbt * tree, 
                     cm_rbt_node * subj_node, cm_rbt_node * tgt_node) {

    //if deleting root, disconnect subject and 
    //target nodes and set target node as root
    if (tree->root == subj_node) {

        //set root
        tree->root            = tgt_node;
        tgt_node->parent      = NULL;
        _rbt_set_root(tree, tgt_node);

        //re-attach right branch
        tgt_node->right         = subj_node->right;
        tgt_node->right->parent = tgt_node;
   
    } else {

        //update subject node's parent
        if (subj_node->parent_side == MORE) subj_node->parent->right = tgt_node;
        if (subj_node->parent_side == LESS) subj_node->parent->left  = tgt_node;

        //if target node is not NULL
        if (tgt_node != NULL) {

            //update target node
            tgt_node->parent      = subj_node->parent;
            tgt_node->parent_side = subj_node->parent_side;
        
            //set target node to the colour of subject node
            tgt_node->colour = subj_node->colour;
        }

        
    }

    return;
}



DBG_STATIC 
cm_rbt_node * _rbt_left_max(cm_rbt_node * node) {

    //bootstrap min traversal
    if (node->left == NULL) return node;
    node = node->left;

    //iterate until min reached
    while (node->right != NULL) {
        node = node->right;
    }

    //return min
    return node;
}



DBG_STATIC DBG_INLINE 
enum cm_rbt_colour _rbt_get_colour(const cm_rbt_node * node) {

    if (node == NULL) return BLACK;
    return node->colour == BLACK ? BLACK : RED;
}



DBG_STATIC DBG_INLINE 
void _rbt_populate_fix_data(const cm_rbt_node * node,
                            struct _rbt_fix_data * f_data) {

    memset(f_data, 0, sizeof(*f_data));
    
    //if unable to get parent, then unable to get everything
    if (node->parent == NULL) return;
    
    //get parent & grandparent
    f_data->parent = node->parent;
    f_data->grandparent = f_data->parent->parent;

    //get uncle
    if (f_data->parent->parent_side == LESS)
        f_data->uncle = f_data->grandparent->right;
    if (f_data->parent->parent_side == MORE)
        f_data->uncle = f_data->grandparent->left;

    //get sibling
    if (node->parent_side == LESS)
        f_data->sibling = f_data->parent->right;
    if (node->parent_side == MORE)
        f_data->sibling = f_data->parent->left;

    return;
}



/*
 *  If uncle is red, parent must be red & grandparent must be black. 
 *  Set uncle and parent to black, set grandparent to red.
 */

//insert case 1
DBG_STATIC DBG_INLINE 
void _rbt_ins_case_1(cm_rbt * tree, 
                     cm_rbt_node ** node, struct _rbt_fix_data * f_data) {

    //swap colours per red uncle case
    f_data->parent->colour = BLACK;
    f_data->uncle->colour  = BLACK;
    if (tree->root != f_data->grandparent) f_data->grandparent->colour = RED;

    //advance node to grandparent
    *node = f_data->grandparent;

    return;
}



/*
 *  If parent is red and is the root node, change parent to black.
 */

//insert case 2
DBG_STATIC DBG_INLINE 
void _rbt_ins_case_2(cm_rbt * tree, 
                     cm_rbt_node ** node, struct _rbt_fix_data * f_data) {

    f_data->parent->colour = BLACK;

    return;
}



/*
 *  Parent is red, grandparent is black and uncle is black. Nodes are organised
 *  such that if the node is a left child of the parent, then the parent is a 
 *  right child of the grandparent (or vice versa). Rotate the parent to make 
 *  it the child of the node. Transform into case 6.
 */

//insert case 3
DBG_STATIC DBG_INLINE 
void _rbt_ins_case_3(cm_rbt * tree, 
                     cm_rbt_node ** node, struct _rbt_fix_data * f_data) {
    
    //node is left child
    if ((*node)->parent_side == LESS) {
        _rbt_right_rotate(tree, f_data->parent);
        *node = (*node)->right;
    
    } else {
        _rbt_left_rotate(tree, f_data->parent);
        *node = (*node)->left;
    }

    return;
}



/*
 *  Parent is red, grandparent is black and uncle is black. Nodes are organised 
 *  such that if the node is left child of the parent, then the parent is also 
 *  a left child of the grandparent (or vice versa). Rotate the grandparent to 
 *  make it the child of the parent.
 */

//insert case 4
DBG_STATIC DBG_INLINE 
void _rbt_ins_case_4(cm_rbt * tree, 
                     cm_rbt_node ** node, struct _rbt_fix_data * f_data) {

    //node is left child
    if ((*node)->parent_side == LESS) {
        _rbt_right_rotate(tree, f_data->grandparent);
    
    } else {
        _rbt_left_rotate(tree, f_data->grandparent);
    }

    //recolour parent and grandparent
    f_data->grandparent->colour = RED;
    f_data->parent->colour      = BLACK;

    return;
}



/*
 *  Sibling is red.
 *
 *  Swap sibling and parent colours, then rotate parent to make sibling 
 *  the new grandparent.
 */

//remove case 1
DBG_STATIC DBG_INLINE 
void _rbt_rem_case_1(cm_rbt * tree, 
                     cm_rbt_node ** node, struct _rbt_fix_data * f_data) {

    //get side of sibling
    enum cm_rbt_side sibling_side = f_data->sibling->parent_side;

    //recolour sibling and parent
    f_data->sibling->colour = BLACK;
    f_data->parent->colour  = RED;

    //rotate parent to make sibling the new grandparent 
    if (f_data->sibling->parent_side == LESS) {
        _rbt_right_rotate(tree, f_data->parent);
    
    } else {
        _rbt_left_rotate(tree, f_data->parent);
    }

    //update fix data
    f_data->grandparent = f_data->parent->parent;
    if (sibling_side == MORE) {
        f_data->sibling = f_data->parent->right;
    } else {
        f_data->sibling = f_data->parent->left;
    }
    
    return;
}



/*
 *  Sibling is black, both of sibling's children are black. Parent is red.
 *  Set sibling to red and advance target node up to parent.
 */

//remove case 2
DBG_STATIC DBG_INLINE 
void _rbt_rem_case_2(cm_rbt * tree, 
                     cm_rbt_node ** node, struct _rbt_fix_data * f_data) {

    f_data->sibling->colour = RED;
    
    if (f_data->parent->colour == RED) {

        //apply fix
        *node = tree->root;

        //update fix data        
        f_data->parent->colour = BLACK;
    
    } else {

        //apply fix
        *node = f_data->parent;
        
        //update fix data
        _rbt_populate_fix_data(*node, f_data);
    }

    return;
}



/*
 *  Sibling is black, sibling's close child is red, sibling's far child is 
 *  black. Swap sibling's and sibling's close child's colours, then rotate 
 *  sibling to make it the new grandparent.
 */

//remove case 3
DBG_STATIC DBG_INLINE 
void _rbt_rem_case_3(cm_rbt * tree, 
                     cm_rbt_node ** node, struct _rbt_fix_data * f_data) {

    //colour close nephew black, set sibling's colour to RED, rotate 
    if (f_data->sibling->parent_side == LESS) {    

        //perform fix
        f_data->sibling->right->colour = BLACK;
        f_data->sibling->colour       = RED;
        _rbt_left_rotate(tree, f_data->sibling);

        //update fix data
        f_data->sibling = f_data->parent->left;
    
    } else {

        //perform fix
        f_data->sibling->left->colour = BLACK; 
        f_data->sibling->colour       = RED;
        _rbt_right_rotate(tree, f_data->sibling);
    
        //update fix data
        f_data->sibling = f_data->parent->right;
    }

    return;
}



/*
 *  Sibling is black, sibling's far child is red. Set sibling's colour to 
 *  parent's colour. Set sibling's far child's colour to black. Set parent's 
 *  colour to black. Then rotate parent to make sibling tne new grandparent 
 *  and advance node up to root.
 */

//remove case 4
DBG_STATIC DBG_INLINE 
void _rbt_rem_case_4(cm_rbt * tree, 
                     cm_rbt_node ** node, struct _rbt_fix_data * f_data) {

    //recolour nodes
    f_data->sibling->colour        = f_data->parent->colour;
    f_data->parent->colour         = BLACK;

    //colour far nephew black, rotate
    if (f_data->sibling->parent_side == LESS) {
        
        f_data->sibling->left->colour = BLACK;
        _rbt_right_rotate(tree, f_data->parent);
    
    } else {
        
        f_data->sibling->right->colour = BLACK;
        _rbt_left_rotate(tree, f_data->parent);
    }

    *node = tree->root;

    return;
}



//determines which case applies for inserting nodes
DBG_STATIC DBG_INLINE 
int _rbt_determine_ins_case(const cm_rbt_node * node,
                            const struct _rbt_fix_data * f_data) {

    bool uncle_black, parent_black;


    //if node is root and is red, no fix necessary
    if (node->parent_side == ROOT) return 0;

    //determine if parent is black
    parent_black = _rbt_get_colour(f_data->parent) == BLACK ? true : false;

    //if parent is black, no fix necessary
    if (parent_black) return 0;

    //determine if uncle is black
    uncle_black = _rbt_get_colour(f_data->uncle) == BLACK ? true : false;

    //if uncle is red, case 1
    if (!uncle_black)
        if (f_data->uncle->colour == RED) return 1;

    //if parent is root, case 2
    if (f_data->parent->parent_side == ROOT) return 2;

    //determine 'triangle' or 'line' case
    if (uncle_black) {

        //if red nodes form a 'triange', case 3
        if (node->parent_side != f_data->parent->parent_side) return 3;

        //if red nodes form a 'line', case 4
        if (node->parent_side == f_data->parent->parent_side) return 4;

    }

    //otherwise invalid state, error out
    cm_errno = CM_ERR_RB_INVALID_STATE;
    return -1;
}



//determines which case applies for removing nodes
DBG_STATIC DBG_INLINE 
int _rbt_determine_rem_case(const cm_rbt * tree, const cm_rbt_node * node,
                            const struct _rbt_fix_data * f_data) {
    
    enum cm_rbt_colour left_colour;
    enum cm_rbt_colour right_colour;

    enum cm_rbt_colour close_colour;
    enum cm_rbt_colour distant_colour;


    //if node is root, no fix necessary
    if (tree->root == node) return 0;

    //if sibling is red, case 1
    if (_rbt_get_colour(f_data->sibling) == RED) return 1;

    //get sibling's child colours
    left_colour = _rbt_get_colour(f_data->sibling->left);
    right_colour = _rbt_get_colour(f_data->sibling->right);

    //if both sibling's children are black, case 2
    if ((left_colour == BLACK) && (right_colour == BLACK)) return 2;

    //case 3 and 4 work with 'close' and 'distant' nephews
    close_colour = f_data->sibling->parent_side == LESS 
                   ? right_colour : left_colour;
    distant_colour = f_data->sibling->parent_side == LESS 
                     ? left_colour : right_colour;

    //if sibling's left child is red and right child is black, case 3
    if ((close_colour == RED) && (distant_colour == BLACK)) return 3;

    //if sibling's right child's colour is red, case 4
    if (distant_colour == RED) return 4;

    //otherwise invalid state, error out
    cm_errno = CM_ERR_RB_INVALID_STATE;
    return -1;
}



DBG_STATIC 
int _rbt_fix_ins(cm_rbt * tree, cm_rbt_node * node) {

    /*
     *  TODO: Refactor to not call _rbt_determine_ins_case() each 
     *        iteration. Also make case 4 follow case 3 immediately.
     */

    int fix_case;
    struct _rbt_fix_data f_data;

    //move up tree and correct violations until fixed or root is reached
    while (node != tree->root) {

        _rbt_populate_fix_data(node, &f_data);
        
        //if fix case is 0, no further corrections necessary
        fix_case = _rbt_determine_ins_case(node, &f_data);
        if (fix_case <= 0) return fix_case; //-1 or 0

        //dispatch fix cases
        switch (fix_case) {

            case 1:
                _rbt_ins_case_1(tree, &node, &f_data);
                break;
            
            case 2:
                _rbt_ins_case_2(tree, &node, &f_data);
                return 0;

            case 3:
                _rbt_ins_case_3(tree, &node, &f_data);
                break;

            case 4:
                _rbt_ins_case_4(tree, &node, &f_data);
                return 0;
        
        } //end switch 

    } //end while

    return 0;
}



DBG_STATIC 
int _rbt_fix_rem(cm_rbt * tree, cm_rbt_node * node, 
                 enum cm_rbt_colour node_colour,
                 struct _rbt_fix_data * f_data) {

    int fix_case;

    //move up tree and correct violations until fixed or root is reached
    while (node != tree->root && node_colour == BLACK) {

        //determine remove case
        fix_case = _rbt_determine_rem_case(tree, node, f_data);
        if (fix_case <= 0) return fix_case; //-1 or 0

        //dispatch remove case
        switch(fix_case) {

            case 1:
                _rbt_rem_case_1(tree, &node, f_data);
                break;

            case 2:
                _rbt_rem_case_2(tree, &node, f_data);
                break;

            case 3:
                _rbt_rem_case_3(tree, &node, f_data);
                break;
            
            case 4:
                _rbt_rem_case_4(tree, &node, f_data);
                return 0;
        }

    } //end while

    return 0;
}



DBG_STATIC DBG_INLINE 
cm_rbt_node * _rbt_add_node(cm_rbt * tree, const void * key, 
                            const void * data, cm_rbt_node * parent, 
                            const enum cm_rbt_side side) {
    int ret;

    //create new node
    cm_rbt_node * node = _rbt_new_node(tree, key, data);

    //if tree is empty, set root
    if (tree->size == 0) {
        _rbt_set_root(tree, node);

    //else connect node
    } else {
        node->parent = parent;
        if (side == LESS) {
            node->parent_side = LESS;
            parent->left = node;
        } else {
            node->parent_side = MORE;
            parent->right = node;
        }
    }

    //increment tree size
    tree->size += 1;

    //fix violations
    ret = _rbt_fix_ins(tree, node);
    if (ret == -1) return NULL;

    return node;

}



DBG_STATIC 
cm_rbt_node * _rbt_uln_node(cm_rbt * tree, const void * key) {

    int ret;

    struct _rbt_fix_data f_data;
    cm_rbt_node * node, * max_node, * fix_node;
    
    enum cm_rbt_side side;
    enum cm_rbt_colour unlink_colour;
    

    //do not apply fixes by default
    unlink_colour = RED;
    
    //get relevant node
    node = _rbt_traverse(tree, key, &side);

    //if a node doesn't exist for this key, error out
    if (side != EQUAL || node == NULL) {
        cm_errno = CM_ERR_USER_KEY;
        return NULL;
    }


    //both children present
    if (node->left != NULL && node->right != NULL) {

        //get maximum node in left subtree
        max_node = _rbt_left_max(node);
        fix_node = max_node->left;

        
        if (max_node->colour == BLACK && 
            _rbt_get_colour(fix_node) == RED) {

            //can replace min node with its child and 
            //colour it black, no fix necessary
            fix_node->colour = BLACK;

        } else {

            //save state prior to removal for use during fixing
            unlink_colour = max_node->colour;
            if (unlink_colour == BLACK) 
                _rbt_populate_fix_data(max_node, &f_data);
        }

        //cut maximum node from left subtree
        _rbt_transplant(tree, max_node, max_node->left);
        
        //re-attach maximum node as root of left subtree
        max_node->left = node->left;
        if (max_node->left != NULL) max_node->left->parent = max_node;

        //set minimum node as new root of whole tree
        _rbt_transplant(tree, node, max_node);

        //re-attach maxumum node as root of right subtree
        max_node->right = node->right;
        if (max_node->right != NULL) max_node->right->parent = max_node;  

        //update fix data if transplant caused parent to change
        if (f_data.parent == node) f_data.parent = max_node;

    //only a left child
    } else if (node->right == NULL && node->left != NULL) {

        //replace node with child
        _rbt_transplant(tree, node, node->left);
        
        //convert node to BLACK, this is guaranteed to maintain balance
        node->left->colour = BLACK;

    //only a right child
    } else if (node->left == NULL && node->right != NULL) {

        //replace node with child
        _rbt_transplant(tree, node, node->right);
        
        //convert node to BLACK, this is guaranteed to maintain balance
        node->right->colour = BLACK;
    
    //no children present
    } else {

        fix_node = NULL;

        //save state prior to removal for use during fixing
        unlink_colour = node->colour;
        if (unlink_colour == BLACK) 
            _rbt_populate_fix_data(node, &f_data);

        if (node->parent_side == ROOT) {

            //set tree root to NULL
            tree->root = NULL;
            
        } else {

            //remove node from parent
            if (node->parent_side == LESS) {
                node->parent->left = NULL;
            } else {
                node->parent->right = NULL;
            }
        }
    }

    tree->size -= 1;

    //if a black node was removed, must correct tree
    if (unlink_colour == BLACK) {
        ret = _rbt_fix_rem(tree, fix_node, unlink_colour, &f_data);
        if (ret == -1) return NULL;
    }
    
    return node;
}



DBG_STATIC 
void _rbt_emp_recurse(cm_rbt_node * node) {

    if (node == NULL) return;
    if (node->left != NULL) _rbt_emp_recurse(node->left);
    if (node->right != NULL) _rbt_emp_recurse(node->right);
    _rbt_del_node(node);

    return;
}



/*
 *  --- [EXTERNAL] ---
 */

int cm_rbt_get(const cm_rbt * tree, const void * key, void * buf) {

    enum cm_rbt_side side;

    //get the node
    cm_rbt_node * node = _rbt_traverse(tree, key, &side);
    if (side != EQUAL) {
        
        cm_errno = CM_ERR_USER_KEY;
        return -1;
    }

    memcpy(buf, node->data, tree->data_sz);

    return 0;
}



void * cm_rbt_get_p(const cm_rbt * tree, const void * key) {

    enum cm_rbt_side side;

    //get the node
    cm_rbt_node * node = _rbt_traverse(tree, key, &side);
    if (side != EQUAL) {
    
        cm_errno = CM_ERR_USER_KEY;
        return NULL;
    }

    return node->data;
}



cm_rbt_node * cm_rbt_get_n(const cm_rbt * tree, const void * key) {

    enum cm_rbt_side side;

    //get the node
    cm_rbt_node * node = _rbt_traverse(tree, key, &side);
    if (side != EQUAL) {
        
        cm_errno = CM_ERR_USER_KEY;
        return NULL;
    }

    return node;
}



cm_rbt_node * cm_rbt_set(cm_rbt * tree,
                         const void * key, const void * data) {

    enum cm_rbt_side side;

    //get relevant node
    cm_rbt_node * node = _rbt_traverse(tree, key, &side);

    //if a node already exists for this key, update its value
    if (side == EQUAL) {
        memcpy(node->data, data, tree->data_sz);
        return node;

    //else create a new node
    } else { 
        return _rbt_add_node(tree, key, data, node, side);
    }
}



int cm_rbt_rem(cm_rbt * tree, const void * key) {

    //get relevant node
    cm_rbt_node * node = _rbt_uln_node(tree, key);
    if (node == NULL) return -1;

    _rbt_del_node(node);

    return 0;
}



cm_rbt_node * cm_rbt_uln(cm_rbt * tree, const void * key) {

    cm_rbt_node * node;

    //get relevant node
    node = _rbt_uln_node(tree, key);

    //null out pointers
    node->parent = node->left = node->right = NULL;

    return node;
}



void cm_rbt_emp(cm_rbt * tree) {

    _rbt_emp_recurse(tree->root);
    tree->root = NULL;
    tree->size = 0;

    return;
}



void cm_new_rbt(cm_rbt * tree, const size_t key_sz, const size_t data_sz, 
                enum cm_rbt_side (*compare) (const void *, const void *)) {

    tree->size      = 0;
    tree->key_sz  = key_sz;
    tree->data_sz = data_sz;
    tree->root      = NULL;
    tree->compare   = compare;

    return;
}



void cm_del_rbt(cm_rbt * tree) {

    _rbt_emp_recurse(tree->root);
    tree->root = NULL;
    tree->size = 0;

    return;
}



void cm_del_rbt_node(cm_rbt_node * node) {

    _rbt_del_node(node);

    return;
}
