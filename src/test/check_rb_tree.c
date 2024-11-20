//standard library
#include <stdio.h>
#include <stdlib.h>

//system headers
#include <unistd.h>
#include <limits.h>

//external libraries
#include <check.h>

//local headers
#include "test_data.h"
#include "suites.h"

//test target headers
#include "../lib/rb_tree.h"


#define DATA_NULL INT_MAX
#define GET_NODE_DATA(node) ((data *) (node->data))

/*
 *  [ADVANCED TEST]
 *
 *      Red-black trees are complicated; internal functions will be tested directly.
 *      For simplicity, key and data values are kept the same.
 */


/*
 *  --- [FIXTURES] ---
 */

//globals
static cm_rb_tree t;
static data d;



enum cm_rb_tree_eval compare(const cm_byte * k_1_ptr, const cm_byte * k_2_ptr) {

    int k_1 = *((int *) k_1_ptr);
    int k_2 = *((int *) k_2_ptr);

    if (k_1 > k_2) return MORE;
    if (k_1 < k_2) return LESS;

    return EQUAL;
}




//empty red-black tree setup
static void _setup_empty() {

    cm_new_rb_tree(&t, sizeof(d), sizeof(d), compare);
    d.x = 0;

    return;
}



//initialiser of false node
static inline void _setup_facade_node(cm_rb_tree_node * node, cm_rb_tree_node * left, 
                                      cm_rb_tree_node * right, 
                                      cm_rb_tree_node * parent, 
                                      enum cm_rb_tree_eval parent_eval, 
                                      enum cm_rb_tree_colour colour) {

    //set relevant fields
    node->left = left;
    node->right = right;
    node->parent = parent;
    node->parent_eval = parent_eval;
    node->colour = colour;

    return;
}



//false populated red-black tree setup
static void _setup_facade() {

    t.size = 5;

    cm_rb_tree_node * n[7];

    //allocate each node
    for (int i = 0; i < 7; ++i) {
        n[i] = malloc(sizeof(cm_rb_tree_node));
        n[i]->key = malloc(sizeof(d));
        n[i]->data = malloc(sizeof(d));
    }

    //link n together 
    t.root = n[0];

    //link n together
    _setup_facade_node(n[0], n[1], n[2], NULL, ROOT, BLACK);
    _setup_facade_node(n[1], n[3], NULL, n[0], LESS, BLACK);
    _setup_facade_node(n[2], n[4], n[5], n[0], MORE, BLACK);
    _setup_facade_node(n[3], NULL, NULL, n[1], LESS, BLACK);
    _setup_facade_node(n[4], NULL, n[6], n[2], LESS, RED);
    _setup_facade_node(n[5], NULL, NULL, n[2], MORE, BLACK);
    _setup_facade_node(n[6], NULL, NULL, n[4], MORE, BLACK);

    /*
     *  Facade tree (not sorted by value):
     *
     *       0
     *     /   \
     *    1     2
     *   /     / \
     *  3     4   5
     *         \
     *          6
     */

    return;
}



//populated red-black tree setup
#define TEST_LEN_FULL 10
static void _setup_full() {

    cm_new_rb_tree(&t, sizeof(d), sizeof(d), compare);
    d.x = 0;

    for (int i = 0; i < TEST_LEN_FULL; ++i) {
        cm_rb_tree_set(&t, (cm_byte *) &d, (cm_byte *) &d);
        d.x++;
    }

    return;
}



static void teardown() {

    cm_del_rb_tree(&t); 
    d.x = -1;

    return;
}



/*
 *  --- [HELPERS] ---
 */

//assert node's position in the tree is correct
static void _assert_node(cm_rb_tree_node * n, int n_data,
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
    if (left_data != DATA_NULL) {
    ck_assert_ptr_nonnull(n->left);
        ck_assert(n->left->parent_eval == LESS);
        temp_data = GET_NODE_DATA(n->left);
        ck_assert_int_eq(temp_data->x, left_data);
    } else {
        ck_assert_ptr_null(n->left);
    }

    //check right
    if (right_data != DATA_NULL) {
        ck_assert_ptr_nonnull(n->right);
        ck_assert(n->right->parent_eval == MORE);
        temp_data = GET_NODE_DATA(n->right);
        ck_assert_int_eq(temp_data->x, right_data);
    } else {
        ck_assert_ptr_null(n->right);
    }

    //check parent
    if (parent_data != DATA_NULL) {
        ck_assert_ptr_nonnull(n->parent);
        
        if (n->parent_eval == LESS) {
            ck_assert_ptr_eq(n->parent->left, n);
        }

        if (n->parent_eval == MORE) {
            ck_assert_ptr_eq(n->parent->right, n);
        }

        if (n->parent_eval == ROOT) {
            ck_assert_ptr_eq(t.root, n);
        }

        temp_data = GET_NODE_DATA(n->right);
        ck_assert_int_eq(temp_data->x, right_data);
    } else {
        ck_assert_ptr_null(n->parent);
        ck_assert_ptr_eq(t.root, n);
    }

    return;
}



static void _assert_node_fast(cm_rb_tree_node * n, int n_data) {

    //check node
    ck_assert_ptr_nonnull(n);
    ck_assert_int_eq(GET_NODE_DATA(n)->x, n_data);
   
    return;
}



/*
 *  --- [UNIT TESTS] ---
 */

//cm_new_rb_tree() [no fixture]
START_TEST(test_new_cm_rb_tree) {

    //run test
    cm_new_rb_tree(&t, sizeof(d), sizeof(d), compare);

    //assert result
    ck_assert_int_eq(t.size, 0);
    ck_assert_int_eq(t.data_size, sizeof(d));
    ck_assert_int_eq(t.data_size, sizeof(d));
    ck_assert_ptr_null(t.root);
    ck_assert_ptr_eq(t.compare, compare);

    //cleanup
    cm_del_rb_tree(&t);

    return;

} END_TEST



//cm_del_rb_tree() [facade fixture]
START_TEST(test_del_cm_rb_tree) {

    /*
     *  Using ASAN to check for leaks here
     */

    //run test
    cm_del_rb_tree(&t);

    return;

} END_TEST



//_rb_tree_new_cm_rb_tree_node() [no fixture]
START_TEST(test__rb_tree_new_cm_rb_tree_node) {
    
    //setup test
    cm_rb_tree_node * n;

    d.x = 0;

    //run test
    n = _rb_tree_new_cm_rb_tree_node(&t, (cm_byte *) &d, (cm_byte *) &d);

    //assert results
    ck_assert_ptr_nonnull(n);
    ck_assert_ptr_nonnull(n->key);
    ck_assert_ptr_nonnull(n->data);
    ck_assert(n->colour == RED);

    _rb_tree_del_cm_rb_tree_node(n);

} END_TEST



//_rb_tree_del_cm_rb_tree_node() [no fixture]
START_TEST(test__rb_tree_del_cm_rb_tree_node) {

    /*
     *  Using ASAN to check for leaks here.
     */
        
    //setup test
    cm_rb_tree_node * n = malloc(sizeof(cm_rb_tree_node));
    n->key = malloc(sizeof(d));
    n->data = malloc(sizeof(d));
    
    //run test
    _rb_tree_del_cm_rb_tree_node(n);

} END_TEST



//_rb_tree_left_rotate() [facade fixture]
START_TEST(test__rb_tree_left_rotate) {

    //rotate node 2 left
    _rb_tree_left_rotate(&t, t.root->right);

    //assert result
    _assert_node(t.root->right, 5, 2, DATA_NULL, 0);
    _assert_node(t.root->right->left, 2, 4, DATA_NULL, 5);
    _assert_node(t.root->right->left->left, 4, DATA_NULL, 6, 2);
    _assert_node(t.root, 0, 1, 5, DATA_NULL);

    //rotate node 0 left
    _rb_tree_left_rotate(&t, t.root);

    //assert result
    _assert_node(t.root, 5, 1, DATA_NULL, DATA_NULL);
    _assert_node(t.root->left, 1, 3, 2, 5);
    ck_assert_ptr_null(t.root->right);

    return;

} END_TEST



//_rb_tree_right_rotate() [facade fixture]
START_TEST(test__rb_tree_right_rotate) {

    //rotate node 2 right
    _rb_tree_right_rotate(&t, t.root->right);

    //assert result
    _assert_node(t.root->right, 4, DATA_NULL, 2, 0);
    ck_assert_ptr_null(t.root->right->left);
    _assert_node(t.root->right->right, 2, 6, 5, 4);
    _assert_node(t.root, 0, 1, 4, DATA_NULL);

    //rotate node 0 right
    _rb_tree_right_rotate(&t, t.root);

    //assert result
    _assert_node(t.root, 1, 3, 0, DATA_NULL);
    _assert_node(t.root->left, 3, DATA_NULL, DATA_NULL, 1);
    _assert_node(t.root->right, 0, DATA_NULL, 4, 0);

    return;

} END_TEST



//_rb_tree_transplant() [facade fixture]
START_TEST(test__rb_tree_transplant) {

    /*
     *  These are very specific test cases; transplant() can't be called 
     *  on arbitrary nodes (per red-black tree theory).
     */

    cm_rb_tree_node * n_1, * n_2, *n_3;

    //transplant 5 into 2
    n_1 = t.root->right;
    _rb_tree_transplant(&t, t.root->right, t.root->right->right);
    _assert_node(t.root->right, 5, 4, DATA_NULL, 0);
    _rb_tree_del_cm_rb_tree_node(n_1);

    //transplant 6 to 4
    n_2 = t.root->right->left;
    _rb_tree_transplant(&t, t.root->right->left, t.root->right->left->right);
    _assert_node(t.root->right->left, 6, DATA_NULL, DATA_NULL, 5);
    _rb_tree_del_cm_rb_tree_node(n_2);

    //transplant 6 to 0
    n_3 = t.root;
    _rb_tree_transplant(&t, t.root, t.root->right->left);
    _assert_node(t.root, 6, DATA_NULL, 5, DATA_NULL);
    _rb_tree_del_cm_rb_tree_node(n_3->left->left);
    _rb_tree_del_cm_rb_tree_node(n_3->left);
    _rb_tree_del_cm_rb_tree_node(n_3);

    return;

} END_TEST



//_rb_tree_right_min() [facade fixture]
START_TEST(test__rb_tree_right_min) {

    cm_rb_tree_node * n;

    //typical case
    n = _rb_tree_right_min(t.root->right);
    _assert_node_fast(n, 5);

    //root
    n = _rb_tree_right_min(t.root);
    _assert_node_fast(n, 4);

    //no right node
    n = _rb_tree_right_min(t.root->left);
    _assert_node_fast(n, 1);

    return;

} END_TEST



//_rb_tree_get_child_colours
START_TEST(test__rb_tree_get_child_colours) {

    enum cm_rb_tree_colour left, right;

    //set opposite values
    left = BLACK;
    right = RED;

    //2 children
    _rb_tree_get_child_colours(t.root->right, &left, &right);
    ck_assert(left == RED);
    ck_assert(right == BLACK);

    //set opposite values
    left = RED;
    right = RED;

    //1 child
    _rb_tree_get_child_colours(t.root->right->left, &left, &right);
    ck_assert(left == BLACK);
    ck_assert(right == BLACK);

    //set opposite values
    left = RED;
    right = RED;

    //no children
    _rb_tree_get_child_colours(t.root->right->left->right, &left, &right);
    ck_assert(left == BLACK);
    ck_assert(right == BLACK);

    return;

} END_TEST


/*
 *  TODO
 *
 *  - Test _rb_tree_populate_fix_data() (on facade fixture)
 *
 *  - Test _rb_tree_determine_ins_case() (on custom tree, mutated for each case)
 *
 *  - Test _rb_tree_determine_rem_case() (on custom tree, mutated for each case)
 *
 *  - Test each insert case (on custom fixtures)
 *
 *  - Test each remove case (on custom fixtures)
 *
 *  - Test fix insert
 *
 *  - Test fix remove
 *
 *  - Test add node
 *
 *  - Test remove node
 *
 *  - Test empty tree
 *
 *  - Test unlink node
 */
