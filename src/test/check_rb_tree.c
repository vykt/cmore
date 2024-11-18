//standard library
#include <stdio.h>
#include <stdlib.h>

//system headers
#include <unistd.h>

//external libraries
#include <check.h>

//local headers
#include "test_data.h"
#include "suites.h"

//test target headers
#include "../lib/rb_tree.h"


#define GET_NODE_DATA(node) ((data *) (node->data))

/*
 *  [ADVANCED TEST]
 *
 *      Red-black trees are complicated; internal functions will be tested directly.
 */


/*
 *  --- [FIXTURES] ---
 */

//globals
static cm_rb_tree t;
static data d;
static key k;



enum cm_rb_tree_eval compare(const cm_byte * k_1_ptr, const cm_byte * k_2_ptr) {

    int k_1 = *((int *) k_1_ptr);
    int k_2 = *((int *) k_2_ptr);

    if (k_1 > k_2) return MORE;
    if (k_1 < k_2) return LESS;

    return EQUAL;
}




//empty red-black tree setup
static void _setup_empty() {

    cm_new_rb_tree(&t, sizeof(k), sizeof(d), compare);
    k.x = 0;
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
    t.key_size = sizeof(k);
    t.data_size = sizeof(d);

    cm_rb_tree_node * n[5];

    //allocate each node
    for (int i = 0; i < 5; ++i) {
        n[i] = malloc(sizeof(cm_rb_tree_node));
        n[i]->key = malloc(t.key_size);
        n[i]->data = malloc(t.data_size);
    }

    //link n together 
    t.root = n[0];

    //link n together
    _setup_facade_node(n[0], n[1], n[2], NULL, ROOT, BLACK);
    _setup_facade_node(n[1], NULL, NULL, n[0], LESS, BLACK);
    _setup_facade_node(n[2], n[3], n[4], n[0], MORE, RED);
    _setup_facade_node(n[3], NULL, NULL, n[2], LESS, BLACK);
    _setup_facade_node(n[4], NULL, NULL, n[2], MORE, BLACK);

    return;
}



//populated red-black tree setup
#define TEST_LEN_FULL 10
static void _setup_full() {

    cm_new_rb_tree(&t, sizeof(k), sizeof(d), compare);
    k.x = 0;
    d.x = 0;

    for (int i = 0; i < TEST_LEN_FULL; ++i) {
        cm_rb_tree_set(&t, (cm_byte *) &k, (cm_byte *) &d);
        k.x++;
        d.x++;
    }

    return;
}



static void teardown() {

    cm_del_rb_tree(&t); 
    k.x = -1;
    d.x = -1;

    return;
}



/*
 *  --- [HELPERS] ---
 */



/*
 *  --- [UNIT TESTS] ---
 */

//cm_new_rb_tree()
START_TEST(test_new_cm_rb_tree) {

    //run test
    cm_new_rb_tree(&t, sizeof(k), sizeof(d), compare);

    //assert result
    ck_assert_int_eq(t.size, 0);
    ck_assert_int_eq(t.key_size, sizeof(k));
    ck_assert_int_eq(t.data_size, sizeof(d));
    ck_assert_ptr_null(t.root);
    ck_assert_ptr_eq(t.compare, compare);

    //cleanup
    cm_del_rb_tree(&t);

    return;

} END_TEST



//cm_del_rb_tree()
START_TEST(test_del_cm_rb_tree) {

    /*
     *  Using ASAN to check for leaks here
     */

    //run test
    cm_del_rb_tree(&t);

    return;

} END_TEST



//_rb_tree_new_cm_rb_tree_node()
START_TEST(test_rb_tree_new_cm_rb_tree_node) {
    
    //setup test
    cm_rb_tree_node * n;

    k.x = 0;
    d.x = 0;

    //run test
    n = _rb_tree_new_cm_rb_tree_node(&t, (const cm_byte *) &k, (const cm_byte *) &d);

    //assert results
    ck_assert_ptr_nonnull(n);
    ck_assert_ptr_nonnull(n->key);
    ck_assert_ptr_nonnull(n->data);
    ck_assert(n->colour == RED);

    _rb_tree_del_cm_rb_tree_node(n);

} END_TEST



//_rb_tree_del_cm_rb_tree_node()
START_TEST(test_rb_tree_del_cm_rb_tree_node) {

    /*
     *  Using ASAN to check for leaks here.
     */
        
    //setup test
    cm_rb_tree_node * n = malloc(sizeof(cm_rb_tree_node));
    n->key = malloc(sizeof(k));
    n->data = malloc(sizeof(d));
    
    //run test
    _rb_tree_del_cm_rb_tree_node(n);

} END_TEST



/*
 *  TODO
 *
 *  - Test left & right rotations (use facade fixture & make it bigger if needed)
 *
 *  - Test transplant (on facade fixture)
 *
 *  - Test right min (on facade fixture)
 *
 *  - Test get child colours (on facade fixture)
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
