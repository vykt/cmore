//standard library
#include <stdio.h>
#include <stdlib.h>

//system headers
#include <time.h>
#include <unistd.h>
#include <limits.h>

//external libraries
#include <check.h>

//local headers
#include "test_data.h"
#include "suites.h"

//test target headers
#include "../lib/cmore.h"
#include "../lib/rbt.h"



/*
 *  [ADVANCED TEST]
 *
 *      Red-black trees are complicated; internal functions will be tested 
 *      directly. For simplicity, key and data values are kept the same.
 */



//globals
static cm_rbt t;
static data d;



/*
 *  --- [HELPERS] ---
 */

#define DATA_NULL    INT_MAX
#define DATA_NOCHECK INT_MAX - 1
#define GET_NODE_DATA(node) ((data *) (node->data))



//assert the node, its data, and its position in the tree
static void _assert_node(cm_rbt_node * n, int n_data,
                         int left_data, int right_data, int parent_data) {

    /*
     *  If called on adjascent nodes, there are substantial redundant checks.
     */

    data * temp_data;

    //check node
    ck_assert_ptr_nonnull(n);
    temp_data = GET_NODE_DATA(n);
    ck_assert_int_eq(temp_data->x, n_data);

    //check left
    if (left_data != DATA_NOCHECK) {
        
        if (left_data != DATA_NULL) {
            ck_assert_ptr_nonnull(n->left);
            ck_assert(n->left->parent_side == CM_RBT_LESS);
            temp_data = GET_NODE_DATA(n->left);
            ck_assert_int_eq(temp_data->x, left_data);
    
        } else {
            ck_assert_ptr_null(n->left);
        }
    }

    //check right
    if (right_data != DATA_NOCHECK) {
    
        if (right_data != DATA_NULL) {
            ck_assert_ptr_nonnull(n->right);
            ck_assert(n->right->parent_side == CM_RBT_MORE);
            temp_data = GET_NODE_DATA(n->right);
            ck_assert_int_eq(temp_data->x, right_data);
    
        } else {
            ck_assert_ptr_null(n->right);
        }
    }

    //check parent
    if (parent_data != DATA_NOCHECK) {
    
        if (parent_data != DATA_NULL) {
            ck_assert_ptr_nonnull(n->parent);
        
            if (n->parent_side == CM_RBT_LESS) {
                ck_assert_ptr_eq(n->parent->left, n);
            }

            if (n->parent_side == CM_RBT_MORE) {
                ck_assert_ptr_eq(n->parent->right, n);
            }

            temp_data = GET_NODE_DATA(n->parent);
            ck_assert_int_eq(temp_data->x, parent_data);
    
        } else {
            ck_assert_ptr_null(n->parent);
            ck_assert_ptr_eq(t.root, n);
        }
    }

    return;
}



//assert only the individual node and its data, not its position
static void _assert_node_fast(cm_rbt_node * n, int n_data) {

    //check node
    ck_assert_ptr_nonnull(n);
    ck_assert_int_eq(GET_NODE_DATA(n)->x, n_data);
   
    return;
}



//assert nodes in fix data correctly relate to each other
static void _assert_fix_data(struct _rbt_fix_data * fix_data, 
                             cm_rbt_node * n, int n_data, 
                             int grandparent_data, int parent_data, 
                             int uncle_data, int sibling_data) {

    /*
     *  Likely some redundancy in the checks; rather be safe.
     */

    data * temp_data;


    //node
    ck_assert_ptr_nonnull(n);
    temp_data = GET_NODE_DATA(n);
    ck_assert_int_eq(temp_data->x, n_data); 
    if (n->parent_side == CM_RBT_ROOT) ck_assert_ptr_eq(t.root, n);

    
    //parent
    if (parent_data != DATA_NULL) {
        
        ck_assert_ptr_nonnull(fix_data->parent);

        if (n->parent_side == CM_RBT_LESS) {
            ck_assert_ptr_eq(fix_data->parent->left, n);
        }

        if (n->parent_side == CM_RBT_MORE) {
            ck_assert_ptr_eq(fix_data->parent->right, n);
        }

        if (fix_data->parent->parent_side == CM_RBT_ROOT) {
            ck_assert_ptr_eq(t.root, fix_data->parent);
            ck_assert_ptr_null(fix_data->grandparent);
            ck_assert_ptr_null(fix_data->uncle);
        } 

        temp_data = GET_NODE_DATA(fix_data->parent);
        ck_assert_int_eq(temp_data->x, parent_data);
   
    } else {
        
        ck_assert_ptr_null(fix_data->parent);
        ck_assert_ptr_null(fix_data->grandparent);
        ck_assert_ptr_null(fix_data->uncle);
        ck_assert_ptr_null(fix_data->sibling);
    }


    //grandparent
    if (grandparent_data != DATA_NULL) {
        
        ck_assert_ptr_nonnull(fix_data->parent);
        ck_assert_ptr_nonnull(fix_data->grandparent);

        if (fix_data->parent->parent_side == CM_RBT_LESS) {
            ck_assert_ptr_eq(fix_data->grandparent->left, fix_data->parent);
        }

        if (fix_data->parent->parent_side == CM_RBT_MORE) {
            ck_assert_ptr_eq(fix_data->grandparent->right, fix_data->parent);
        }

        if (fix_data->grandparent->parent_side == CM_RBT_ROOT) {
            ck_assert_ptr_eq(t.root, fix_data->grandparent);
        } 

        ck_assert(fix_data->parent->parent_side != CM_RBT_ROOT);
        temp_data = GET_NODE_DATA(fix_data->grandparent);
        ck_assert_int_eq(temp_data->x, grandparent_data);
    
    } else {
    
        ck_assert_ptr_null(fix_data->grandparent);
        ck_assert_ptr_null(fix_data->uncle);
        ck_assert_ptr_eq(t.root, fix_data->parent);
    }

    
    //uncle
    if (uncle_data != DATA_NULL) {
        
        ck_assert_ptr_nonnull(fix_data->uncle);
        ck_assert_ptr_nonnull(fix_data->parent);
        ck_assert_ptr_nonnull(fix_data->grandparent);

        if (fix_data->uncle->parent_side == CM_RBT_LESS) {
            ck_assert_ptr_eq(fix_data->grandparent->left, fix_data->uncle);
        }

        if (fix_data->uncle->parent_side == CM_RBT_MORE) {
            ck_assert_ptr_eq(fix_data->grandparent->right, fix_data->uncle);
        }

        ck_assert(fix_data->uncle->parent_side != CM_RBT_ROOT);
        temp_data = GET_NODE_DATA(fix_data->uncle);
        ck_assert_int_eq(temp_data->x, uncle_data);

    } else {
    
        ck_assert_ptr_null(fix_data->uncle);
    }


    //subling
    if (sibling_data != DATA_NULL) {
        
        ck_assert_ptr_nonnull(fix_data->sibling);
        ck_assert_ptr_nonnull(fix_data->parent);
    
        if (fix_data->sibling->parent_side == CM_RBT_LESS) {
            ck_assert_ptr_eq(fix_data->parent->left, fix_data->sibling);
        }

        if (fix_data->sibling->parent_side == CM_RBT_MORE) {
            ck_assert_ptr_eq(fix_data->parent->right, fix_data->sibling);
        }

        ck_assert(fix_data->sibling->parent_side != CM_RBT_ROOT);
        temp_data = GET_NODE_DATA(fix_data->sibling);
        ck_assert_int_eq(temp_data->x, sibling_data);
    
    } else {

        ck_assert_ptr_null(fix_data->sibling);
    }

    return;
}



static void _set_fix_data(struct _rbt_fix_data * f_data,
                          cm_rbt_node * parent, 
                          cm_rbt_node * grandparent, 
                          cm_rbt_node * uncle, cm_rbt_node * sibling) {

    f_data->parent      = parent;
    f_data->grandparent = grandparent;
    f_data->uncle       = uncle;
    f_data->sibling     = sibling;

    return;
}



static void _recurse_compare_trees(cm_rbt_node * node_0,
                                   cm_rbt_node * node_1) {

    int * data_0, * data_1;
    int * key_0, * key_1;


    //assert both nodes exist or dont exist
    if ((node_0 == NULL && node_1 != NULL)
        || (node_0 != NULL && node_1 == NULL)) {
            ck_assert_ptr_nonnull(node_0);
            ck_assert_ptr_nonnull(node_1);
    } //end if

    //return immediately if either node is undefined
    if (node_0 == NULL) return;

    //perform key & data assertions
    key_0 = node_0->key;
    key_1 = node_1->key;
    bool hmm = *key_0;
    hmm = *key_1;
    //ck_assert_int_eq(key_0->x, key_1->x);

    data_0 = node_0->data;
    data_1 = node_1->data;
    //ck_assert_int_eq(data_0->x, data_1->x);

    //perform node assertions
    ck_assert_int_eq(node_0->colour, node_1->colour);
    ck_assert_int_eq(node_0->parent_side, node_1->parent_side);

    //recurse down
    _recurse_compare_trees(node_0->left, node_1->left);
    _recurse_compare_trees(node_0->right, node_1->right);

    return;
}



/*
 *  --- [FIXTURES] ---
 */

//compare function used by the fixture red-black trees
enum cm_rbt_side compare(const void * b_1, const void * b_2) {

    data * d_1, * d_2;

    d_1 = (data *) b_1;
    d_2 = (data *) b_2;

    if (d_1->x > d_2->x) return CM_RBT_MORE;
    if (d_1->x < d_2->x) return CM_RBT_LESS;

    return CM_RBT_EQUAL;
}




//empty red-black tree setup
static void _setup_emp() {

    cm_new_rbt(&t, sizeof(d), sizeof(d), compare);
    d.x = 0;

    return;
}



//initialiser of a stub node
static void _setup_stub_node(cm_rbt_node * node, cm_rbt_node * left, 
                             cm_rbt_node * right, cm_rbt_node * parent, 
                             enum cm_rbt_side parent_side, 
                             enum cm_rbt_colour colour) {

    //set relevant fields
    node->left = left;
    node->right = right;
    node->parent = parent;
    node->parent_side = parent_side;
    node->colour = colour;

    return;
}



//stub red-black tree setup
static void _setup_stub() {

    /*
     *  Stub tree (not sorted by value):
     *
     *       0b
     *     /   \
     *    1r    2r
     *   /     / \
     *  3b    4b  5b
     *         \
     *          6r
     */

    t.size = 7;

    cm_rbt_node * n[7];

    //allocate each node
    for (int i = 0; i < 7; ++i) {
        
        n[i] = malloc(sizeof(cm_rbt_node));
        n[i]->key = malloc(sizeof(d));
        n[i]->data = malloc(sizeof(d));
    
        *((int *) n[i]->key)  = i;
        *((int *) n[i]->data) = i;
    }

    //link n together 
    t.root = n[0];

    //link n together
    _setup_stub_node(n[0], n[1], n[2], NULL, CM_RBT_ROOT, CM_RBT_BLACK);
    _setup_stub_node(n[1], n[3], NULL, n[0], CM_RBT_LESS, CM_RBT_RED);
    _setup_stub_node(n[2], n[4], n[5], n[0], CM_RBT_MORE, CM_RBT_RED);
    _setup_stub_node(n[3], NULL, NULL, n[1], CM_RBT_LESS, CM_RBT_BLACK);
    _setup_stub_node(n[4], NULL, n[6], n[2], CM_RBT_LESS, CM_RBT_BLACK);
    _setup_stub_node(n[5], NULL, NULL, n[2], CM_RBT_MORE, CM_RBT_BLACK);
    _setup_stub_node(n[6], NULL, NULL, n[4], CM_RBT_MORE, CM_RBT_RED);

    return;
}



//sorted stub red-black tree
static void _setup_sorted_stub() {

    /*
     *  Sorted stub tree (sorted by value):
     *
     *        _ 20b _
     *       /       \
     *    10b         40r
     *   /   \       /  \
     *  5r   15r   30b  50b
     *            /    /  \
     *          25r  45r   55r
     */

    t.size = 10;
    t.compare = compare;
    t.key_sz  = sizeof(d.x);
    t.data_sz = sizeof(d);
    t.is_init = true;

    cm_rbt_node * n[10];
    int values[10] = {20, 10, 40, 5, 15, 30, 50, 25, 45, 55};

    //allocate each node
    for (int i = 0; i < 10; ++i) {
        
        n[i] = malloc(sizeof(cm_rbt_node));
        n[i]->key = malloc(sizeof(d));
        n[i]->data = malloc(sizeof(d));
    
        *((int *) n[i]->key)  = values[i];
        *((int *) n[i]->data) = values[i];
    }

    //link n together 
    t.root = n[0];

    //link n together
    _setup_stub_node(n[0], n[1], n[2], NULL, CM_RBT_ROOT, CM_RBT_BLACK);
    _setup_stub_node(n[1], n[3], n[4], n[0], CM_RBT_LESS, CM_RBT_BLACK);
    _setup_stub_node(n[2], n[5], n[6], n[0], CM_RBT_MORE, CM_RBT_RED);
    _setup_stub_node(n[3], NULL, NULL, n[1], CM_RBT_LESS, CM_RBT_RED);
    _setup_stub_node(n[4], NULL, NULL, n[1], CM_RBT_MORE, CM_RBT_RED);
    _setup_stub_node(n[5], n[7], NULL, n[2], CM_RBT_LESS, CM_RBT_BLACK);
    _setup_stub_node(n[6], n[8], n[9], n[2], CM_RBT_MORE, CM_RBT_BLACK);
    _setup_stub_node(n[7], NULL, NULL, n[5], CM_RBT_LESS, CM_RBT_RED);
    _setup_stub_node(n[8], NULL, NULL, n[6], CM_RBT_LESS, CM_RBT_RED);
    _setup_stub_node(n[9], NULL, NULL, n[6], CM_RBT_MORE, CM_RBT_RED);

    return;
}



static void _teardown() {

    cm_del_rbt(&t); 
    d.x = -1;

    return;
}



/*
 *  --- [UNIT TESTS] ---
 */

//cm_new_rbt() & cm_del_rbt() [no fixture]
START_TEST(test_new_del_rbt) {

    //only test: create a new red-black tree & destroy it
    cm_new_rbt(&t, sizeof(d), sizeof(d), compare);

    ck_assert_int_eq(t.size, 0);
    ck_assert_int_eq(t.data_sz, sizeof(d));
    ck_assert_int_eq(t.data_sz, sizeof(d));
    ck_assert_ptr_null(t.root);
    ck_assert_ptr_eq(t.compare, compare);
    ck_assert_int_eq(t.is_init, true);

    cm_del_rbt(&t);
    ck_assert_int_eq(t.is_init, false);

    return;

} END_TEST



#ifdef DEBUG
//_rbt_new_node() & _rbt_del_node() [no fixture]
START_TEST(test__new_del_node) {
    
    //setup test
    cm_rbt_node * n;

    d.x = 0;

    //only test: create a new node & destroy it
    n = _rbt_new_node(&t, &d, &d);

    ck_assert_ptr_nonnull(n);
    ck_assert_ptr_nonnull(n->key);
    ck_assert_ptr_nonnull(n->data);
    ck_assert(n->colour == CM_RBT_RED);

    _rbt_del_node(n);

} END_TEST



//_rbt_left_rotate() [stub fixture]
START_TEST(test__left_rotate) {

    //first test: rotate node 2 left
    _rbt_left_rotate(&t, t.root->right);

    _assert_node(t.root->right, 5, 2, DATA_NULL, 0);
    _assert_node(t.root->right->left, 2, 4, DATA_NULL, 5);
    _assert_node(t.root->right->left->left, 4, DATA_NULL, 6, 2);
    _assert_node(t.root, 0, 1, 5, DATA_NULL);


    //second test: rotate node 0 left
    _rbt_left_rotate(&t, t.root);

    _assert_node(t.root, 5, 0, DATA_NULL, DATA_NULL);
    _assert_node(t.root->left, 0, 1, 2, 5);
    ck_assert_ptr_null(t.root->right);

    return;

} END_TEST



//_rbt_right_rotate() [stub fixture]
START_TEST(test__right_rotate) {

    //first test: rotate node 2 right
    _rbt_right_rotate(&t, t.root->right);

    _assert_node(t.root->right, 4, DATA_NULL, 2, 0);
    ck_assert_ptr_null(t.root->right->left);
    _assert_node(t.root->right->right, 2, 6, 5, 4);
    _assert_node(t.root, 0, 1, 4, DATA_NULL);


    //second test: rotate node 0 right
    _rbt_right_rotate(&t, t.root);

    _assert_node(t.root, 1, 3, 0, DATA_NULL);
    _assert_node(t.root->left, 3, DATA_NULL, DATA_NULL, 1);
    _assert_node(t.root->right, 0, DATA_NULL, 4, 1);

    return;

} END_TEST



//_rbt_transplant() [stub fixture]
START_TEST(test__transplant) {

    /*
     *  These are very specific test cases; transplant() can't be called 
     *  on arbitrary nodes (per red-black tree theory).
     */

    cm_rbt_node * del_node;


    //first test: transplant 3 into 1
    del_node = t.root->left;
    _rbt_transplant(&t, t.root->left, t.root->left->left);
    _assert_node(t.root->left, 3, DATA_NULL, DATA_NULL, 0);
    _rbt_del_node(del_node);

    //second test: transplant 6 into 4
    del_node = t.root->right->left;
    _rbt_transplant(&t, t.root->right->left, t.root->right->left->right);
    _assert_node(t.root->right->left, 6, DATA_NULL, DATA_NULL, 2);
    _rbt_del_node(del_node);

    return;

} END_TEST



//_rbt_left_max() [stub fixture]
START_TEST(test__left_max) {

    cm_rbt_node * n;


    //first test: typical case
    n = _rbt_left_max(t.root->right);
    _assert_node_fast(n, 6);


    //second test: root
    n = _rbt_left_max(t.root);
    _assert_node_fast(n, 1);


    //third test: no right node
    n = _rbt_left_max(t.root->right->left);
    _assert_node_fast(n, 4);

    return;

} END_TEST



//_rbt_get_colour [stub fixture]
START_TEST(test__get_colour) {

    enum cm_rbt_colour colour;


    //first test: red node
    colour = _rbt_get_colour(t.root->right);
    ck_assert(colour == CM_RBT_RED);


    //second test: black node
    colour = _rbt_get_colour(t.root->right->left);
    ck_assert(colour == CM_RBT_BLACK);


    //third test: NULL node
    colour = _rbt_get_colour(NULL);
    ck_assert(colour = CM_RBT_BLACK);

    return;

} END_TEST



//_rbt_populate_fix_data() [stub fixture]
START_TEST(test__populate_fix_data) {

    struct _rbt_fix_data f_data;


    //first test: parent, grandparent, uncle, sibling present
    memset(&f_data, 0, sizeof(f_data));
    _rbt_populate_fix_data(t.root->right->left, &f_data);
    _assert_fix_data(&f_data, t.root->right->left, 4, 0, 2, 1, 5);


    //second test: parent, grandparent, uncle present
    memset(&f_data, 0, sizeof(f_data));
    _rbt_populate_fix_data(t.root->right->left->right, &f_data);
    _assert_fix_data(&f_data, 
                     t.root->right->left->right, 6, 2, 4, 5, DATA_NULL);


    //third test: parent, sibling present
    memset(&f_data, 0, sizeof(f_data));
    _rbt_populate_fix_data(t.root->left, &f_data);
    _assert_fix_data(&f_data, t.root->left, 1, DATA_NULL, 0, DATA_NULL, 2);

    return;

} END_TEST



//_rbt_determine_ins_case() [no fixture]
START_TEST(test__determine_ins_case) {
    
    int ret;
    
    struct _rbt_fix_data f_data;
    cm_rbt_node node, parent, grandparent, uncle;


    //first test: case 0
    _setup_stub_node(&node, NULL, NULL, &parent, CM_RBT_LESS, CM_RBT_RED);
    _setup_stub_node(&parent, &node, NULL, NULL, CM_RBT_LESS, CM_RBT_BLACK);
    
    _set_fix_data(&f_data, &parent, NULL, NULL, NULL);
    ret = _rbt_determine_ins_case(&node, &f_data);
    ck_assert_int_eq(ret, 0);

    ret = _rbt_determine_ins_case(&node, &f_data);
    ck_assert_int_eq(ret, 0);
    

    //second test: case 1
    _setup_stub_node(&node, NULL, NULL, &parent, CM_RBT_LESS, CM_RBT_RED);
    _setup_stub_node(&parent, &node, NULL, &grandparent, CM_RBT_LESS, CM_RBT_RED);
    _setup_stub_node(&grandparent, &parent, &uncle, NULL, CM_RBT_LESS, CM_RBT_BLACK);
    _setup_stub_node(&uncle, NULL, NULL, &grandparent, CM_RBT_MORE, CM_RBT_RED);

    _set_fix_data(&f_data, &parent, &grandparent, &uncle, NULL);
    ret = _rbt_determine_ins_case(&node, &f_data);
    ck_assert_int_eq(ret, 1);


    //third test: case 2
    _setup_stub_node(&node, NULL, NULL, &parent, CM_RBT_LESS, CM_RBT_RED);
    _setup_stub_node(&parent, &node, NULL, NULL, CM_RBT_ROOT, CM_RBT_RED);
    
    _set_fix_data(&f_data, &parent, NULL, NULL, NULL);
    ret = _rbt_determine_ins_case(&node, &f_data);
    ck_assert_int_eq(ret, 2);


    //fourth test: case 3
    _setup_stub_node(&node, NULL, NULL, &parent, CM_RBT_LESS, CM_RBT_RED);
    _setup_stub_node(&parent, &node, NULL, &grandparent, CM_RBT_MORE, CM_RBT_RED);
    _setup_stub_node(&grandparent, &uncle, &parent, NULL, CM_RBT_MORE, CM_RBT_RED);
    _setup_stub_node(&uncle, NULL, NULL, &grandparent, CM_RBT_LESS, CM_RBT_BLACK);
    
    _set_fix_data(&f_data, &parent, &grandparent, &uncle, NULL);
    ret = _rbt_determine_ins_case(&node, &f_data);
    ck_assert_int_eq(ret, 3);
    

    //fifth test: case 4
    _setup_stub_node(&node, NULL, NULL, &parent, CM_RBT_LESS, CM_RBT_RED);
    _setup_stub_node(&parent, &node, NULL, &grandparent, CM_RBT_LESS, CM_RBT_RED);
    _setup_stub_node(&grandparent, &parent, &uncle, NULL, CM_RBT_MORE, CM_RBT_RED);
    _setup_stub_node(&uncle, NULL, NULL, &grandparent, CM_RBT_MORE, CM_RBT_BLACK);
    
    _set_fix_data(&f_data, &parent, &grandparent, &uncle, NULL);
    ret = _rbt_determine_ins_case(&node, &f_data);
    ck_assert_int_eq(ret, 4);

    return;

} END_TEST



//_rbt_determine_rmv_case() [no fixture]
START_TEST(test__determine_rmv_case) {

    int ret;
    
    struct _rbt_fix_data f_data;
    cm_rbt_node node, parent, sibling, c_nephew, f_nephew;


    //first test: case 0
    _setup_stub_node(&node, NULL, NULL, NULL, CM_RBT_ROOT, CM_RBT_BLACK);
    t.root = &node;
    
    _set_fix_data(&f_data, NULL, NULL, NULL, NULL);
    ret = _rbt_determine_rmv_case(&t, &node, &f_data);
    ck_assert_int_eq(ret, 0);


    //second test: case 1 - left side
    _setup_stub_node(&node, NULL, NULL, &parent, CM_RBT_LESS, CM_RBT_BLACK);
    _setup_stub_node(&parent, &node, &sibling, NULL, CM_RBT_LESS, CM_RBT_BLACK);
    _setup_stub_node(&sibling, &c_nephew, &f_nephew, &parent, CM_RBT_MORE, CM_RBT_RED);
    _setup_stub_node(&c_nephew, NULL, NULL, &sibling, CM_RBT_LESS, CM_RBT_BLACK);
    _setup_stub_node(&f_nephew, NULL, NULL, &sibling, CM_RBT_MORE, CM_RBT_BLACK);
    t.root = NULL;

    _set_fix_data(&f_data, &parent, NULL, NULL, &sibling);
    ret = _rbt_determine_rmv_case(&t, &node, &f_data);
    ck_assert_int_eq(ret, 1);

    //third test: case 1 - right side
    _setup_stub_node(&node, NULL, NULL, &parent, CM_RBT_MORE, CM_RBT_BLACK);
    _setup_stub_node(&parent, &sibling, &node, NULL, CM_RBT_LESS, CM_RBT_BLACK);
    _setup_stub_node(&sibling, &f_nephew, &c_nephew, &parent, CM_RBT_LESS, CM_RBT_RED);
    _setup_stub_node(&c_nephew, NULL, NULL, &sibling, CM_RBT_MORE, CM_RBT_BLACK);
    _setup_stub_node(&f_nephew, NULL, NULL, &sibling, CM_RBT_LESS, CM_RBT_BLACK);

    _set_fix_data(&f_data, &parent, NULL, NULL, &sibling);
    ret = _rbt_determine_rmv_case(&t, &node, &f_data);
    ck_assert_int_eq(ret, 1);


    //fourth test: case 2 - left side
    _setup_stub_node(&node, NULL, NULL, &parent, CM_RBT_LESS, CM_RBT_BLACK);
    _setup_stub_node(&parent, &node, &sibling, NULL, CM_RBT_LESS, CM_RBT_BLACK);
    _setup_stub_node(&sibling, &c_nephew, &f_nephew, &parent, CM_RBT_MORE, CM_RBT_BLACK);
    _setup_stub_node(&c_nephew, NULL, NULL, &sibling, CM_RBT_LESS, CM_RBT_BLACK);
    _setup_stub_node(&f_nephew, NULL, NULL, &sibling, CM_RBT_MORE, CM_RBT_BLACK);

    _set_fix_data(&f_data, &parent, NULL, NULL, &sibling);
    ret = _rbt_determine_rmv_case(&t, &node, &f_data);
    ck_assert_int_eq(ret, 2);


    //fifth test: case 2 - right side    
    _setup_stub_node(&node, NULL, NULL, &parent, CM_RBT_MORE, CM_RBT_BLACK);
    _setup_stub_node(&parent, &sibling, &node, NULL, CM_RBT_LESS, CM_RBT_BLACK);
    _setup_stub_node(&sibling, &f_nephew, &c_nephew, &parent, CM_RBT_LESS, CM_RBT_BLACK);
    _setup_stub_node(&c_nephew, NULL, NULL, &sibling, CM_RBT_MORE, CM_RBT_BLACK);
    _setup_stub_node(&f_nephew, NULL, NULL, &sibling, CM_RBT_LESS, CM_RBT_BLACK);

    _set_fix_data(&f_data, &parent, NULL, NULL, &sibling);
    ret = _rbt_determine_rmv_case(&t, &node, &f_data);
    ck_assert_int_eq(ret, 2);


    //sixth test: case 3 - left side    
    _setup_stub_node(&node, NULL, NULL, &parent, CM_RBT_LESS, CM_RBT_BLACK);
    _setup_stub_node(&parent, &node, &sibling, NULL, CM_RBT_LESS, CM_RBT_BLACK);
    _setup_stub_node(&sibling, &c_nephew, &f_nephew, &parent, CM_RBT_MORE, CM_RBT_BLACK);
    _setup_stub_node(&c_nephew, NULL, NULL, &sibling, CM_RBT_LESS, CM_RBT_RED);
    _setup_stub_node(&f_nephew, NULL, NULL, &sibling, CM_RBT_MORE, CM_RBT_BLACK);

    _set_fix_data(&f_data, &parent, NULL, NULL, &sibling);
    ret = _rbt_determine_rmv_case(&t, &node, &f_data);
    ck_assert_int_eq(ret, 3);


    //seventh test: case 3 - right side
    _setup_stub_node(&node, NULL, NULL, &parent, CM_RBT_MORE, CM_RBT_BLACK);
    _setup_stub_node(&parent, &sibling, &node, NULL, CM_RBT_LESS, CM_RBT_RED);
    _setup_stub_node(&sibling, &f_nephew, &c_nephew, &parent, CM_RBT_LESS, CM_RBT_BLACK);
    _setup_stub_node(&c_nephew, NULL, NULL, &sibling, CM_RBT_MORE, CM_RBT_RED);
    _setup_stub_node(&f_nephew, NULL, NULL, &sibling, CM_RBT_LESS, CM_RBT_BLACK);

    _set_fix_data(&f_data, &parent, NULL, NULL, &sibling);
    ret = _rbt_determine_rmv_case(&t, &node, &f_data);
    ck_assert_int_eq(ret, 3);
    
    
    //eighth test: case 4 - left side    
    _setup_stub_node(&node, NULL, NULL, &parent, CM_RBT_LESS, CM_RBT_BLACK);
    _setup_stub_node(&parent, &node, &sibling, NULL, CM_RBT_LESS, CM_RBT_BLACK);
    _setup_stub_node(&sibling, NULL, &f_nephew, &parent, CM_RBT_MORE, CM_RBT_BLACK);
    _setup_stub_node(&f_nephew, NULL, NULL, &sibling, CM_RBT_MORE, CM_RBT_RED);

    _set_fix_data(&f_data, &parent, NULL, NULL, &sibling);
    ret = _rbt_determine_rmv_case(&t, &node, &f_data);
    ck_assert_int_eq(ret, 4);


    //ninth test: case 4 - right side
    _setup_stub_node(&node, NULL, NULL, &parent, CM_RBT_MORE, CM_RBT_BLACK);
    _setup_stub_node(&parent, &sibling, &node, NULL, CM_RBT_LESS, CM_RBT_RED);
    _setup_stub_node(&sibling, &f_nephew, NULL, &parent, CM_RBT_LESS, CM_RBT_BLACK);
    _setup_stub_node(&f_nephew, NULL, NULL, &sibling, CM_RBT_LESS, CM_RBT_RED);

    _set_fix_data(&f_data, &parent, NULL, NULL, &sibling);
    ret = _rbt_determine_rmv_case(&t, &node, &f_data);
    ck_assert_int_eq(ret, 4);
    
    return;

} END_TEST



//_rbt_ins_case_1 [stub fixture]
START_TEST(test__ins_case_1) {

    struct _rbt_fix_data f_data;
    cm_rbt_node * node;

    //setup
    node = t.root->right->left->right;
    t.root->right->colour = CM_RBT_BLACK;
    t.root->right->left->colour = CM_RBT_RED;
    t.root->right->right->colour = CM_RBT_RED;
    _set_fix_data(&f_data, t.root->right->left, t.root->right, 
                  t.root->right->right, NULL);

    
    //only test:
    _rbt_ins_case_1(&t, &node, &f_data);
    
    ck_assert(t.root->right->colour == CM_RBT_RED);
    ck_assert(t.root->right->left->colour == CM_RBT_BLACK);
    ck_assert(t.root->right->right->colour == CM_RBT_BLACK);
    
    ck_assert_ptr_eq(node, t.root->right);

    return;

} END_TEST



//_rbt_ins_case_2 [stub fixture]
START_TEST(test__ins_case_2) {

    struct _rbt_fix_data f_data;
    cm_rbt_node * node;

    //setup
    node = t.root->left;
    t.root->colour = CM_RBT_RED;
    _set_fix_data(&f_data, t.root, NULL, NULL, NULL);

    //only test:
    _rbt_ins_case_2(&t, &node, &f_data);
    ck_assert(t.root->colour == CM_RBT_BLACK);

    return;

} END_TEST



//_rbt_ins_case_3 [stub fixture]
START_TEST(test__ins_case_3) {

    struct _rbt_fix_data f_data;
    cm_rbt_node * node;

    //setup
    node = t.root->right->left->right;
    t.root->right->left->colour = CM_RBT_RED;
    t.root->right->colour = CM_RBT_BLACK;
    t.root->right->right->colour = CM_RBT_BLACK;
    _set_fix_data(&f_data, t.root->right->left,
                  t.root->right, t.root->right->right, NULL);

    //only test:
    _rbt_ins_case_3(&t, &node, &f_data);

    _assert_node(t.root->right, 2, 6, 5, 0);
    _assert_node(t.root->right->right, 5, DATA_NULL, DATA_NULL, 2);
    _assert_node(t.root->right->left, 6, 4, DATA_NULL, 2);
    _assert_node(t.root->right->left->left, 4, DATA_NULL, DATA_NULL, 6);
    
    ck_assert_ptr_eq(node, t.root->right->left->left);

    return;

} END_TEST



//_rbt_ins_case_4 [stub fixture]
START_TEST(test__ins_case_4) {

    struct _rbt_fix_data f_data;
    cm_rbt_node * node;

    //setup
    t.root->right->left->left = t.root->right->left->right;
    t.root->right->left->right = NULL;
    t.root->right->left->left->parent_side = CM_RBT_LESS;
    node = t.root->right->left->left;
    t.root->right->colour = CM_RBT_BLACK;
    t.root->right->left->colour = CM_RBT_BLACK;
    _set_fix_data(&f_data, t.root->right->left, t.root->right,
                  t.root->right->right, NULL);

    //only test:
    _rbt_ins_case_4(&t, &node, &f_data);

    _assert_node(t.root, 0, 1, 4, DATA_NULL);
    _assert_node(t.root->right, 4, 6, 2, 0);
    _assert_node(t.root->right->left, 6, DATA_NULL, DATA_NULL, 4);
    _assert_node(t.root->right->right, 2, DATA_NULL, 5, 4);
    
    ck_assert_ptr_eq(node, t.root->right->left);
    
    ck_assert(t.root->right->colour == CM_RBT_BLACK);
    ck_assert(t.root->right->right->colour == CM_RBT_RED);

    return;

} END_TEST



//_rbt_rmv_case_1 [stub fixture]
START_TEST(test__rmv_case_1) {

    struct _rbt_fix_data f_data;
    cm_rbt_node * node;
    
    //setup
    node = t.root->right->right;
    t.root->right->left->right->colour = CM_RBT_BLACK;
    t.root->right->left->colour = CM_RBT_RED;
    t.root->right->colour = CM_RBT_BLACK;
    _set_fix_data(&f_data, t.root->right, t.root, 
                  t.root->left, t.root->right->left);
    
    //only test:
    _rbt_rmv_case_1(&t, &node, &f_data);

    _assert_node(t.root, 0, 1, 4, DATA_NULL);
    _assert_node(t.root->right, 4, DATA_NULL, 2, 0);
    _assert_node(t.root->right->right, 2, 6, 5, 4);
    _assert_node(t.root->right->right->left, 6, DATA_NULL, DATA_NULL, 2);
    _assert_node(t.root->right->right->right, 5, DATA_NULL, DATA_NULL, 2);
    
    ck_assert(t.root->right->colour == CM_RBT_BLACK);
    ck_assert(t.root->right->right->colour == CM_RBT_RED);

    return;
} END_TEST



//_rbt_rmv_case_2 [stub fixture]
START_TEST(test__rmv_case_2) {

    struct _rbt_fix_data f_data;
    cm_rbt_node * node;

    //setup
    node = t.root->right->right;
    t.root->right->left->right->colour = CM_RBT_BLACK;
    _set_fix_data(&f_data, t.root->right, t.root,
                  t.root->left, t.root->right->left);    

    //only test:
    _rbt_rmv_case_2(&t, &node, &f_data);
    ck_assert(t.root->right->left->colour == CM_RBT_RED);

    return;

} END_TEST



//_rbt_rmv_case_3 [stub fixture]
START_TEST(test__rmv_case_3) {

    struct _rbt_fix_data f_data;
    cm_rbt_node * node;

    //setup
    node = t.root->right->right;
    _set_fix_data(&f_data, t.root->right, t.root, 
                  t.root->left, t.root->right->left);

    //only test:
    _rbt_rmv_case_3(&t, &node, &f_data);

    _assert_node(t.root->right, 2, 6, 5, 0);
    _assert_node(t.root->right->right, 5, DATA_NULL, DATA_NULL, 2);
    _assert_node(t.root->right->left, 6, 4, DATA_NULL, 2);
    _assert_node(t.root->right->left->left, 4, DATA_NULL, DATA_NULL, 6);

    ck_assert_ptr_eq(node, t.root->right->right);    
    ck_assert(t.root->right->left->colour == CM_RBT_BLACK);
    ck_assert(t.root->right->left->left->colour == CM_RBT_RED);

    return;

} END_TEST



//_rbt_rmv_case_4 [stub fixture]
START_TEST(test__rmv_case_4) {

    struct _rbt_fix_data f_data;
    cm_rbt_node * node;

    //setup
    node = t.root->right->right;
    t.root->right->left->left = t.root->right->left->right;
    t.root->right->left->right = NULL;
    t.root->right->left->left->parent_side = CM_RBT_LESS;
    _set_fix_data(&f_data, t.root->right, t.root,
                  t.root->left, t.root->right->left);

    //only test:
    _rbt_rmv_case_4(&t, &node, &f_data);

    //assert result
    _assert_node(t.root, 0, 1, 4, DATA_NULL);
    _assert_node(t.root->right, 4, 6, 2, 0);
    _assert_node(t.root->right->left, 6, DATA_NULL, DATA_NULL, 4);
    _assert_node(t.root->right->right, 2, DATA_NULL, 5, 4);
    _assert_node(t.root->right->right->right, 5, DATA_NULL, DATA_NULL, 2);

    ck_assert_ptr_eq(node, t.root);

    ck_assert(t.root->right->colour == CM_RBT_RED);
    ck_assert(t.root->right->left->colour == CM_RBT_BLACK);
    ck_assert(t.root->right->right->colour = CM_RBT_BLACK);

    return;

} END_TEST



/*
 * The remaining internal functions are tested indirectly through the 
 * exported cm_rbt_set() and cm_rbt_rmv() functions.
 *
 */

#endif



//cm_rbt_get [sorted stub fixture]
START_TEST(test_rbt_get) {

    int ret;
    data ret_data;
    data * ret_data_ptr;
    cm_rbt_node * ret_node;


    //first test: get the root node's value
    d.x = 20;
    ret = cm_rbt_get(&t, &d.x, &ret_data);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(ret_data.x, d.x);

    //second test: get a value pointer to a leaf node
    d.x = 15;
    ret_data_ptr = cm_rbt_get_p(&t, &d.x);
    ck_assert_ptr_ne(ret_data_ptr, NULL);
    ck_assert_int_eq(ret_data_ptr->x, d.x);

    //third test: get a node pointer to a non-leaf node
    d.x = 30;
    ret_node = cm_rbt_get_n(&t, &d.x);
    ck_assert_ptr_ne(ret_node, NULL);
    ck_assert_int_eq(((data *) ret_node->data)->x, d.x);


    //fourth test: fetch index 0
    ret = cm_rbt_idx_get(&t, 0, &ret_data);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(ret_data.x, 5);

    //fifth test: fetch a non-leaf node by index
    ret_data_ptr = cm_rbt_idx_get_p(&t, 6);
    ck_assert_ptr_ne(ret_data_ptr, NULL);
    ck_assert_int_eq(ret_data_ptr->x, 40);

    //sixth test: fetch the final index node
    ret_node = cm_rbt_idx_get_n(&t, 9);
    ck_assert_ptr_ne(ret_node, NULL);
    ck_assert_int_eq(((data *) ret_node->data)->x, 55);

    //seventh test: fetch the final index node via negative index
    ret_node = cm_rbt_idx_get_n(&t, -1);
    ck_assert_ptr_ne(ret_node, NULL);
    ck_assert_int_eq(((data *) ret_node->data)->x, 55);

    //eighth test: fetch an out of bounds index
    ret = cm_rbt_idx_get(&t, 10, &ret_data);
    ck_assert_int_eq(ret, -1);

} END_TEST



//cm_rbt_set [empty fixture]
START_TEST(test_rbt_set) {

    cm_rbt_node * ret;

    //first test: root insert, case 0
    d.x = 20;
    ret = cm_rbt_set(&t, &d.x, &d);
    _assert_node_fast(ret, 20);
    
    _assert_node(t.root, 20, DATA_NULL, DATA_NULL, DATA_NULL);
    ck_assert(t.root->colour == CM_RBT_BLACK);


    //second test: case 2
    d.x = 25;
    ret = cm_rbt_set(&t, &d.x, &d);
    _assert_node_fast(ret, 25);
    
    _assert_node(t.root, 20, DATA_NULL, 25, DATA_NULL);
    ck_assert(t.root->colour == CM_RBT_BLACK);
    _assert_node(t.root->right, 25, DATA_NULL, DATA_NULL, 20);
    ck_assert(t.root->right->colour == CM_RBT_RED);

    
    //third test: case 4
    d.x = 30;
    ret = cm_rbt_set(&t, &d.x, &d);
    _assert_node_fast(ret, 30);
    
    _assert_node(t.root, 25, 20, 30, DATA_NULL);
    ck_assert(t.root->colour == CM_RBT_BLACK);
    _assert_node(t.root->left, 20, DATA_NULL, DATA_NULL, 25);
    ck_assert(t.root->left->colour == CM_RBT_RED);
    _assert_node(t.root->right, 30, DATA_NULL, DATA_NULL, 25);
    ck_assert(t.root->right->colour == CM_RBT_RED);


    //fourth test: case 1
    d.x = 22;
    ret = cm_rbt_set(&t, &d.x, &d);
    _assert_node_fast(ret, 22);

    _assert_node(t.root->left, 20, DATA_NULL, 22, 25);
    ck_assert(t.root->left->colour == CM_RBT_BLACK);
    ck_assert(t.root->right->colour == CM_RBT_BLACK);
    _assert_node(t.root->left->right, 22, DATA_NULL, DATA_NULL, 20);
    ck_assert(t.root->left->right->colour == CM_RBT_RED);


    //fifth test: case 3, case 4
    d.x = 21;
    ret = cm_rbt_set(&t, &d.x, &d);
    _assert_node_fast(ret, 21);
    
    _assert_node(t.root->left, 21, 20, 22, 25);
    ck_assert(t.root->left->colour == CM_RBT_BLACK);
    _assert_node(t.root->left->left, 20, DATA_NULL, DATA_NULL, 21);
    ck_assert(t.root->left->left->colour == CM_RBT_RED);
    _assert_node(t.root->left->right, 22, DATA_NULL, DATA_NULL, 21);
    ck_assert(t.root->left->right->colour == CM_RBT_RED);

    return;
    
} END_TEST



//cm_rbt_rmv [sorted stub fixture]
START_TEST(test_rbt_rmv) {

    int ret;

    //first test: case 0
    d.x = 25;
    ret = cm_rbt_rmv(&t, &d.x);
    ck_assert_int_eq(ret, 0);

    _assert_node(t.root->right->left, 30, DATA_NULL, DATA_NULL, 40);


    //second test: case 4
    d.x = 30;
    ret = cm_rbt_rmv(&t, &d.x);
    ck_assert_int_eq(ret, 0);

    _assert_node(t.root->right, 50, 40, 55, 20);
    ck_assert(t.root->right->colour == CM_RBT_RED);
    _assert_node(t.root->right->left, 40, DATA_NULL, 45, 50);
    ck_assert(t.root->right->left->colour == CM_RBT_BLACK);
    _assert_node(t.root->right->right, 55, DATA_NULL, DATA_NULL, 50);
    ck_assert(t.root->right->right->colour == CM_RBT_BLACK);
    _assert_node(t.root->right->left->right, 45, DATA_NULL, DATA_NULL, 40);


    //third test: case 3
    d.x = 55;
    ret = cm_rbt_rmv(&t, &d.x);
    ck_assert_int_eq(ret, 0);

    _assert_node(t.root, 20, 10, 45, DATA_NULL);
    ck_assert(t.root->colour == CM_RBT_BLACK);
    _assert_node(t.root->right, 45, 40, 50, 20);
    ck_assert(t.root->right->colour == CM_RBT_RED);
    _assert_node(t.root->right->left, 40, DATA_NULL, DATA_NULL, 45);
    ck_assert(t.root->right->left->colour == CM_RBT_BLACK);
    _assert_node(t.root->right->right, 50, DATA_NULL, DATA_NULL, 45);
    ck_assert(t.root->right->right->colour == CM_RBT_BLACK);


    //fourth test: case 1 & 2 (red parent)
    d.x = 5;
    ret = cm_rbt_rmv(&t, &d.x);
    ck_assert_int_eq(ret, 0);

    d.x = 15;
    ret = cm_rbt_rmv(&t, &d.x);
    ck_assert_int_eq(ret, 0);
    
    d.x = 10;
    ret = cm_rbt_rmv(&t, &d.x);
    ck_assert_int_eq(ret, 0);

    _assert_node(t.root, 45, 20, 50, DATA_NULL);
    ck_assert(t.root->colour == CM_RBT_BLACK);
    _assert_node(t.root->left, 20, DATA_NULL, 40, 45);
    ck_assert(t.root->left->colour == CM_RBT_BLACK);
    _assert_node(t.root->right, 50, DATA_NULL, DATA_NULL, 45);
    ck_assert(t.root->right->colour == CM_RBT_BLACK);
    _assert_node(t.root->left->right, 40, DATA_NULL, DATA_NULL, 20);
    ck_assert(t.root->left->right->colour == CM_RBT_RED);


    //fifth test: 2 children (root, no fixes)
    d.x = 15;
    cm_rbt_set(&t, &d.x, &d);

    d.x = 30;
    cm_rbt_set(&t, &d.x, &d);

    d.x = 45;
    ret = cm_rbt_rmv(&t, &d.x);
    ck_assert_int_eq(ret, 0);

    _assert_node(t.root, 40, 20, 50, DATA_NULL);
    ck_assert(t.root->colour == CM_RBT_BLACK);
    _assert_node(t.root->left, 20, 15, 30, 40);
    ck_assert(t.root->left->colour == CM_RBT_RED);
    _assert_node(t.root->right, 50, DATA_NULL, DATA_NULL, 40);
    ck_assert(t.root->right->colour == CM_RBT_BLACK);
    _assert_node(t.root->left->left, 15, DATA_NULL, DATA_NULL, 20);
    ck_assert(t.root->left->left->colour == CM_RBT_BLACK);
    _assert_node(t.root->left->right, 30, DATA_NULL, DATA_NULL, 20);
    ck_assert(t.root->left->right->colour == CM_RBT_BLACK);

    //sixth test: 2 children (non-root, fixes)
    d.x = 20;
    ret = cm_rbt_rmv(&t, &d.x);
    ck_assert_int_eq(ret, 0);

    _assert_node(t.root, 40, 15, 50, DATA_NULL);
    _assert_node(t.root->left, 15, DATA_NULL, 30, 40);
    ck_assert(t.root->left->colour == CM_RBT_BLACK);
    _assert_node(t.root->left->right, 30, DATA_NULL, DATA_NULL, 15);
    ck_assert(t.root->left->right->colour == CM_RBT_RED);

    //seventh test: 1 child
    d.x = 15;
    ret = cm_rbt_rmv(&t, &d.x);
    ck_assert_int_eq(ret, 0);

    _assert_node(t.root, 40, 30, 50, DATA_NULL);
    _assert_node(t.root->left, 30, DATA_NULL, DATA_NULL, 40);
    ck_assert(t.root->left->colour == CM_RBT_BLACK);
    
    return;
    
} END_TEST



/*
 *  Both the remove() and unlink() operations call the same internal function.
 *  As such, the unlink() test depends on the remove() test, and the tests
 *  test implementation rather than behaviour. While this is not ideal, my
 *  time is also finite.
 */

//cm_rbt_uln [sorted stub fixture]
START_TEST(test_rbt_uln) {

    cm_rbt_node * ret;


    //only test:
    d.x = 5;
    ret = cm_rbt_uln(&t, &d.x);
    ck_assert_ptr_nonnull(ret);

    ck_assert_ptr_null(ret->parent);
    ck_assert_ptr_null(ret->left);
    ck_assert_ptr_null(ret->right);

    free(ret->key);
    free(ret->data);
    free(ret);

    return;
    
} END_TEST



//cm_rbt_emp() [stub fixture]
START_TEST(test_rbt_emp) {

    /*
     *  Using ASAN to check for leaks here.
     */

    cm_rbt_emp(&t);
    ck_assert_ptr_null(t.root);
    ck_assert_int_eq(t.size, 0);

    return;
    
} END_TEST



//cm_rbt_cpy() [sorted stub fixture]
START_TEST(test_rbt_cpy) {

    int ret;
    cm_rbt u;

    cm_rbt_node * dst_node, * src_node;
    data * dst_data, * src_data;


    //only test: copy tree t into u
    ret = cm_rbt_cpy(&u, &t);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(u.size, t.size);

    //recursively compare nodes
    _recurse_compare_trees(t.root, u.root);

    cm_del_rbt(&u);
    return;

} END_TEST



//cm_rbt_mov() [sorted stub fixture]
START_TEST(test_rbt_mov) {

    cm_rbt u;
    
    //only test: move vector v into w and back
    cm_rbt_mov(&u, &t);
    ck_assert_int_eq(t.is_init, false);
    ck_assert_int_eq(u.is_init, true);

    cm_rbt_mov(&t, &u);
    ck_assert_int_eq(t.is_init, true);
    ck_assert_int_eq(u.is_init, false);

    
} END_TEST



//cm_del_rbt_node [no fixture]
START_TEST(test_del_rbt_node) {

    /*
     *  Using ASAN to check for leaks here.
     */
        
    //setup test
    cm_rbt_node * n = malloc(sizeof(cm_rbt_node));
    n->key = malloc(sizeof(d));
    n->data = malloc(sizeof(d));
    
    //only test:
    cm_del_rbt_node(n);
    
} END_TEST



Suite * rbt_suite() {

    //test cases
    TCase * tc_new_del_rbt;

    //internal test cases for debug builds
    #ifdef DEBUG
    TCase * tc__new_del_node;
    TCase * tc__left_rotate;
    TCase * tc__right_rotate;
    TCase * tc__transplant;
    TCase * tc__left_max;
    TCase * tc__get_colour;
    TCase * tc__populate_fix_data;
    TCase * tc__determine_ins_case;
    TCase * tc__determine_rmv_case;
    TCase * tc__ins_case_1;
    TCase * tc__ins_case_2;
    TCase * tc__ins_case_3;
    TCase * tc__ins_case_4;
    TCase * tc__rmv_case_1;
    TCase * tc__rmv_case_2;
    TCase * tc__rmv_case_3;
    TCase * tc__rmv_case_4;
    #endif

    //test cases (cont.)
    TCase * tc_rbt_get;
    TCase * tc_rbt_set;
    TCase * tc_rbt_rmv;
    TCase * tc_rbt_uln;
    TCase * tc_rbt_emp;
    TCase * tc_rbt_cpy;
    TCase * tc_rbt_mov;
    TCase * tc_del_rbt_node;

    Suite * s = suite_create("rb_tree");


    //tc_new_cm_rbt()
    tc_new_del_rbt = tcase_create("new_del_cm_rbt");
    tcase_add_test(tc_new_del_rbt, test_new_del_rbt);

    #ifdef DEBUG
    //tc__new_del_node()
    tc__new_del_node = tcase_create("_new_del_node");
    tcase_add_test(tc__new_del_node, test__new_del_node);
    
    //tc__left_rotate()
    tc__left_rotate = tcase_create("_left_rotate");
    tcase_add_checked_fixture(tc__left_rotate, _setup_stub, _teardown);
    tcase_add_test(tc__left_rotate, test__left_rotate);

    //tc__right_rotate()
    tc__right_rotate = tcase_create("_right_rotate");
    tcase_add_checked_fixture(tc__right_rotate, _setup_stub, _teardown);
    tcase_add_test(tc__right_rotate, test__right_rotate);
    
    //tc__transplant()
    tc__transplant = tcase_create("_transplant");
    tcase_add_checked_fixture(tc__transplant, _setup_stub, _teardown);
    tcase_add_test(tc__transplant, test__transplant);
    
    //tc__left_max()
    tc__left_max = tcase_create("_left_max");
    tcase_add_checked_fixture(tc__left_max, _setup_stub, _teardown);
    tcase_add_test(tc__left_max, test__left_max);

    //tc__get_colour()
    tc__get_colour = tcase_create("_get_colour");
    tcase_add_checked_fixture(tc__get_colour, _setup_stub, _teardown);
    tcase_add_test(tc__get_colour, test__get_colour);

    //tc__populate_fix_data()
    tc__populate_fix_data = tcase_create("_populate_fix_data");
    tcase_add_checked_fixture(tc__populate_fix_data, _setup_stub, _teardown);
    tcase_add_test(tc__populate_fix_data, test__populate_fix_data);

    //tc__determine_ins_case()
    tc__determine_ins_case = tcase_create("_determine_ins_case");
    tcase_add_test(tc__determine_ins_case, test__determine_ins_case);

    //tc__determine_rmv_case()
    tc__determine_rmv_case = tcase_create("_determine_rmv_case");
    tcase_add_test(tc__determine_rmv_case, test__determine_rmv_case);

    //tc__ins_case_1
    tc__ins_case_1 = tcase_create("_ins_case_1");
    tcase_add_checked_fixture(tc__ins_case_1, _setup_stub, _teardown);
    tcase_add_test(tc__ins_case_1, test__ins_case_1);

    //tc__ins_case_2
    tc__ins_case_2 = tcase_create("_ins_case_2");
    tcase_add_checked_fixture(tc__ins_case_2, _setup_stub, _teardown);
    tcase_add_test(tc__ins_case_2, test__ins_case_2);
    
    //tc__ins_case_3
    tc__ins_case_3 = tcase_create("_ins_case_3");
    tcase_add_checked_fixture(tc__ins_case_3, _setup_stub, _teardown);
    tcase_add_test(tc__ins_case_3, test__ins_case_3);
    
    //tc__ins_case_4
    tc__ins_case_4 = tcase_create("_ins_case_4");
    tcase_add_checked_fixture(tc__ins_case_4, _setup_stub, _teardown);
    tcase_add_test(tc__ins_case_4, test__ins_case_4);

    //tc__rmv_case_1
    tc__rmv_case_1 = tcase_create("_rmv_case_1");
    tcase_add_checked_fixture(tc__rmv_case_1, _setup_stub, _teardown);
    tcase_add_test(tc__rmv_case_1, test__rmv_case_1);
    
    //tc__rmv_case_2
    tc__rmv_case_2 = tcase_create("_rmv_case_2");
    tcase_add_checked_fixture(tc__rmv_case_2, _setup_stub, _teardown);
    tcase_add_test(tc__rmv_case_2, test__rmv_case_2);
    
    //tc__rmv_case_3
    tc__rmv_case_3 = tcase_create("_rmv_case_3");
    tcase_add_checked_fixture(tc__rmv_case_3, _setup_stub, _teardown);
    tcase_add_test(tc__rmv_case_3, test__rmv_case_3);
    
    //tc__rmv_case_4
    tc__rmv_case_4 = tcase_create("_rmv_case_4");
    tcase_add_checked_fixture(tc__rmv_case_4, _setup_stub, _teardown);
    tcase_add_test(tc__rmv_case_4, test__rmv_case_4);
    #endif

    //tc_rbt_get
    tc_rbt_get = tcase_create("rb_tree_get");
    tcase_add_checked_fixture(tc_rbt_get, _setup_sorted_stub, _teardown);
    tcase_add_test(tc_rbt_get, test_rbt_get);

    //tc_rbt_set
    tc_rbt_set = tcase_create("rb_tree_set");
    tcase_add_checked_fixture(tc_rbt_set, _setup_emp, _teardown);
    tcase_add_test(tc_rbt_set, test_rbt_set);

    //tc_rbt_rmv
    tc_rbt_rmv = tcase_create("rb_tree_rmv");
    tcase_add_checked_fixture(tc_rbt_rmv, _setup_sorted_stub, _teardown);
    tcase_add_test(tc_rbt_rmv, test_rbt_rmv);

    //tc_rbt_uln
    tc_rbt_uln = tcase_create("rb_tree_uln");
    tcase_add_checked_fixture(tc_rbt_uln, _setup_sorted_stub, _teardown);
    tcase_add_test(tc_rbt_uln, test_rbt_uln);

    //tc_rbt_emp
    tc_rbt_emp = tcase_create("rb_tree_emp");
    tcase_add_checked_fixture(tc_rbt_emp, _setup_stub, _teardown);
    tcase_add_test(tc_rbt_emp, test_rbt_emp);

    //tc_rbt_cpy
    tc_rbt_cpy = tcase_create("rb_tree_cpy");
    tcase_add_checked_fixture(tc_rbt_cpy, _setup_sorted_stub, _teardown);
    tcase_add_test(tc_rbt_cpy, test_rbt_cpy);

    //tc_rbt_mov
    tc_rbt_mov = tcase_create("rb_tree_mov");
    tcase_add_checked_fixture(tc_rbt_mov, _setup_sorted_stub, _teardown);
    tcase_add_test(tc_rbt_mov, test_rbt_mov);

    //tc_del_rbt_node
    tc_del_rbt_node = tcase_create("del_rbt_node");
    tcase_add_test(tc_del_rbt_node, test_del_rbt_node);


    //add test cases to red-black tree suite
    suite_add_tcase(s, tc_new_del_rbt);
    
    #ifdef DEBUG
    suite_add_tcase(s, tc__new_del_node);
    suite_add_tcase(s, tc__left_rotate);
    suite_add_tcase(s, tc__right_rotate);
    suite_add_tcase(s, tc__transplant);
    suite_add_tcase(s, tc__left_max);
    suite_add_tcase(s, tc__get_colour);
    suite_add_tcase(s, tc__populate_fix_data);
    suite_add_tcase(s, tc__determine_ins_case);
    suite_add_tcase(s, tc__determine_rmv_case);
    suite_add_tcase(s, tc__ins_case_1);
    suite_add_tcase(s, tc__ins_case_2);
    suite_add_tcase(s, tc__ins_case_3);
    suite_add_tcase(s, tc__ins_case_4);
    suite_add_tcase(s, tc__rmv_case_1);
    suite_add_tcase(s, tc__rmv_case_2);
    suite_add_tcase(s, tc__rmv_case_3);
    suite_add_tcase(s, tc__rmv_case_4);
    #endif

    //add test cases to red-black tree suite (cont.)
    suite_add_tcase(s, tc_rbt_get);
    suite_add_tcase(s, tc_rbt_set);
    suite_add_tcase(s, tc_rbt_rmv);
    suite_add_tcase(s, tc_rbt_uln);
    suite_add_tcase(s, tc_rbt_emp);
    suite_add_tcase(s, tc_rbt_cpy);
    suite_add_tcase(s, tc_rbt_mov);
    suite_add_tcase(s, tc_del_rbt_node);

    return s;
}


/*
 *  --- [EXPERIMENTAL TESTS] ---
 */

/*
 *  To perform experimental tests, run the following loop inside a debugger.
 *
 *  Manipulate the run, op, and d.x values to modify the behaviour of the loop.
 *
 *  Inspect the state of the tree with `pnode`, `pcolour`, and `pall` macros.
 */

#define SET    0
#define REMOVE 1
void rbt_explore() {

    bool run = true;
    char op = SET;
    d.x = 64;

    int ret;
    cm_rbt_node * node;

    //create tree
    cm_new_rbt(&t, sizeof(d.x), sizeof(d), compare);

    //perform operations
    while (run) {

        if (op == SET) {
            node = cm_rbt_set(&t, &d.x, &d);
            
        } else {
            ret = cm_rbt_rmv(&t, &d.x);
        }
    }

    //destroy tree
    cm_del_rbt(&t);

    return;
}
