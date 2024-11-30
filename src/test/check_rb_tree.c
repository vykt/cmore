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
#include "../lib/cmore.h"
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



//initialiser of stub node
static void _setup_stub_node(cm_rb_tree_node * node, cm_rb_tree_node * left, 
                             cm_rb_tree_node * right, cm_rb_tree_node * parent, 
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



//stub red-black tree setup
static void _setup_stub() {

    t.size = 7;

    cm_rb_tree_node * n[7];

    //allocate each node
    for (int i = 0; i < 7; ++i) {
        
        n[i] = malloc(sizeof(cm_rb_tree_node));
        n[i]->key = malloc(sizeof(d));
        n[i]->data = malloc(sizeof(d));
    
        *((int *) n[i]->key)  = i;
        *((int *) n[i]->data) = i;
    }

    //link n together 
    t.root = n[0];

    //link n together
    _setup_stub_node(n[0], n[1], n[2], NULL, ROOT, BLACK);
    _setup_stub_node(n[1], n[3], NULL, n[0], LESS, RED);
    _setup_stub_node(n[2], n[4], n[5], n[0], MORE, RED);
    _setup_stub_node(n[3], NULL, NULL, n[1], LESS, BLACK);
    _setup_stub_node(n[4], NULL, n[6], n[2], LESS, BLACK);
    _setup_stub_node(n[5], NULL, NULL, n[2], MORE, BLACK);
    _setup_stub_node(n[6], NULL, NULL, n[4], MORE, RED);

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

        temp_data = GET_NODE_DATA(n->parent);
        ck_assert_int_eq(temp_data->x, parent_data);
    
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



static void _assert_fix_data(struct _rb_tree_fix_data * fix_data, 
                             cm_rb_tree_node * n, int n_data, int grandparent_data, 
                             int parent_data, int uncle_data, int sibling_data) {

    /*
     *  Likely some redundancy in the checks; rather be safe.
     */

    data * temp_data;


    //node
    ck_assert_ptr_nonnull(n);
    temp_data = GET_NODE_DATA(n);
    ck_assert_int_eq(temp_data->x, n_data); 
    if (n->parent_eval == ROOT) ck_assert_ptr_eq(t.root, n);

    
    //parent
    if (parent_data != DATA_NULL) {
        
        ck_assert_ptr_nonnull(fix_data->parent);

        if (n->parent_eval == LESS) {
            ck_assert_ptr_eq(fix_data->parent->left, n);
        }

        if (n->parent_eval == MORE) {
            ck_assert_ptr_eq(fix_data->parent->right, n);
        }

        if (fix_data->parent->parent_eval == ROOT) {
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

        if (fix_data->parent->parent_eval == LESS) {
            ck_assert_ptr_eq(fix_data->grandparent->left, fix_data->parent);
        }

        if (fix_data->parent->parent_eval == MORE) {
            ck_assert_ptr_eq(fix_data->grandparent->right, fix_data->parent);
        }

        if (fix_data->grandparent->parent_eval == ROOT) {
            ck_assert_ptr_eq(t.root, fix_data->grandparent);
        } 

        ck_assert(fix_data->parent->parent_eval != ROOT);
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

        if (fix_data->uncle->parent_eval == LESS) {
            ck_assert_ptr_eq(fix_data->grandparent->left, fix_data->uncle);
        }

        if (fix_data->uncle->parent_eval == MORE) {
            ck_assert_ptr_eq(fix_data->grandparent->right, fix_data->uncle);
        }

        ck_assert(fix_data->uncle->parent_eval != ROOT);
        temp_data = GET_NODE_DATA(fix_data->uncle);
        ck_assert_int_eq(temp_data->x, uncle_data);

    } else {
    
        ck_assert_ptr_null(fix_data->uncle);
    }


    //subling
    if (sibling_data != DATA_NULL) {
        
        ck_assert_ptr_nonnull(fix_data->sibling);
        ck_assert_ptr_nonnull(fix_data->parent);
    
        if (fix_data->sibling->parent_eval == LESS) {
            ck_assert_ptr_eq(fix_data->parent->left, fix_data->sibling);
        }

        if (fix_data->sibling->parent_eval == MORE) {
            ck_assert_ptr_eq(fix_data->parent->right, fix_data->sibling);
        }

        ck_assert(fix_data->sibling->parent_eval != ROOT);
        temp_data = GET_NODE_DATA(fix_data->sibling);
        ck_assert_int_eq(temp_data->x, sibling_data);
    
    } else {

        ck_assert_ptr_null(fix_data->sibling);
    }

    return;
}



static void _set_fix_data(struct _rb_tree_fix_data * f_data,
                          cm_rb_tree_node * parent, cm_rb_tree_node * grandparent,
                          cm_rb_tree_node * uncle, cm_rb_tree_node * sibling) {

    f_data->parent      = parent;
    f_data->grandparent = grandparent;
    f_data->uncle       = uncle;
    f_data->sibling     = sibling;

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



//cm_del_rb_tree() [stub fixture]
START_TEST(test_del_cm_rb_tree) {

    /*
     *  Using ASAN to check for leaks here
     */

    //run test
    cm_del_rb_tree(&t);

    return;

} END_TEST


#ifdef DEBUG
//_rb_tree_new_node() [no fixture]
START_TEST(test__new_node) {
    
    //setup test
    cm_rb_tree_node * n;

    d.x = 0;

    //run test
    n = _rb_tree_new_node(&t, (cm_byte *) &d, (cm_byte *) &d);

    //assert results
    ck_assert_ptr_nonnull(n);
    ck_assert_ptr_nonnull(n->key);
    ck_assert_ptr_nonnull(n->data);
    ck_assert(n->colour == RED);

    _rb_tree_del_node(n);

} END_TEST



//_rb_tree_del_node() [no fixture]
START_TEST(test__del_node) {

    /*
     *  Using ASAN to check for leaks here.
     */
        
    //setup test
    cm_rb_tree_node * n = malloc(sizeof(cm_rb_tree_node));
    n->key = malloc(sizeof(d));
    n->data = malloc(sizeof(d));
    
    //run test
    _rb_tree_del_node(n);

} END_TEST



//_rb_tree_left_rotate() [stub fixture]
START_TEST(test__left_rotate) {

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
    _assert_node(t.root, 5, 0, DATA_NULL, DATA_NULL);
    _assert_node(t.root->left, 0, 1, 2, 5);
    ck_assert_ptr_null(t.root->right);

    return;

} END_TEST



//_rb_tree_right_rotate() [stub fixture]
START_TEST(test__right_rotate) {

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
    _assert_node(t.root->right, 0, DATA_NULL, 4, 1);

    return;

} END_TEST



//_rb_tree_transplant() [stub fixture]
START_TEST(test__transplant) {

    /*
     *  These are very specific test cases; transplant() can't be called 
     *  on arbitrary nodes (per red-black tree theory).
     */

    cm_rb_tree_node * del_node;

    //transplant 3 into 1
    del_node = t.root->left;
    _rb_tree_transplant(&t, t.root->left, t.root->left->left);
    _assert_node(t.root->left, 3, DATA_NULL, DATA_NULL, 0);
    _rb_tree_del_node(del_node);

    //transplant 6 into 4
    del_node = t.root->right->left;
    _rb_tree_transplant(&t, t.root->right->left, t.root->right->left->right);
    _assert_node(t.root->right->left, 6, DATA_NULL, DATA_NULL, 2);
    _rb_tree_del_node(del_node);

    //transplant 6 into 0
    del_node = t.root;
    _rb_tree_transplant(&t, t.root, t.root->right->left);
    _assert_node(t.root, 6, DATA_NULL, 2, DATA_NULL);
    _rb_tree_del_node(del_node->left);
    _rb_tree_del_node(del_node);

    return;

} END_TEST



//_rb_tree_right_min() [stub fixture]
START_TEST(test__right_min) {

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



//_rb_tree_get_colour [stub fixture]
START_TEST(test__get_colour) {

    enum cm_rb_tree_colour colour;

    //red node
    colour = _rb_tree_get_colour(t.root->right);
    ck_assert(colour == RED);

    //black node
    colour = _rb_tree_get_colour(t.root->right->left);
    ck_assert(colour == BLACK);

    //NULL node
    colour = _rb_tree_get_colour(NULL);
    ck_assert(colour = BLACK);

    return;

} END_TEST



//_rb_tree_populate_fix_data() [stub fixture]
START_TEST(test__populate_fix_data) {

    struct _rb_tree_fix_data f_data;

    //parent, grandparent, uncle, sibling present
    memset(&f_data, 0, sizeof(f_data));
    _rb_tree_populate_fix_data(t.root->right->left, &f_data);
    _assert_fix_data(&f_data, t.root->right->left, 4, 0, 2, 1, 5);

    //parent, grandparent, uncle present
    memset(&f_data, 0, sizeof(f_data));
    _rb_tree_populate_fix_data(t.root->right->left->right, &f_data);
    _assert_fix_data(&f_data, t.root->right->left->right, 6, 2, 4, 5, DATA_NULL);

    //parent, sibling present
    memset(&f_data, 0, sizeof(f_data));
    _rb_tree_populate_fix_data(t.root->left, &f_data);
    _assert_fix_data(&f_data, t.root->left, 1, DATA_NULL, 0, DATA_NULL, 2);

    return;

} END_TEST



//_rb_tree_determine_ins_case() [no fixture]
START_TEST(test__determine_ins_case) {
    
    int ret;
    
    struct _rb_tree_fix_data f_data;
    cm_rb_tree_node node, parent, grandparent, uncle;


    //case 0:
    _setup_stub_node(&node, NULL, NULL, &parent, LESS, RED);
    _setup_stub_node(&parent, &node, NULL, NULL, LESS, BLACK);
    t.root = NULL;
    
    _set_fix_data(&f_data, &parent, NULL, NULL, NULL);
    ret = _rb_tree_determine_ins_case(&node, &f_data);
    ck_assert_int_eq(ret, 0);

    t.root = &parent;
    ret = _rb_tree_determine_ins_case(&node, &f_data);
    ck_assert_int_eq(ret, 0);
    

    //case 1:
    _setup_stub_node(&node, NULL, NULL, &parent, LESS, RED);
    _setup_stub_node(&parent, &node, NULL, &grandparent, LESS, RED);
    _setup_stub_node(&grandparent, &parent, &uncle, NULL, LESS, BLACK);
    _setup_stub_node(&uncle, NULL, NULL, &grandparent, MORE, RED);
    t.root = NULL;

    _set_fix_data(&f_data, &parent, &grandparent, &uncle, NULL);
    ret = _rb_tree_determine_ins_case(&node, &f_data);
    ck_assert_int_eq(ret, 1);


    //case 2:
    _setup_stub_node(&node, NULL, NULL, &parent, LESS, RED);
    _setup_stub_node(&parent, &node, NULL, &grandparent, LESS, RED);
    _setup_stub_node(&grandparent, &parent, NULL, NULL, ROOT, RED);
    t.root = &grandparent;
    
    _set_fix_data(&f_data, &parent, &grandparent, NULL, NULL);
    ret = _rb_tree_determine_ins_case(&node, &f_data);
    ck_assert_int_eq(ret, 2);


    //case 3
    _setup_stub_node(&node, NULL, NULL, &parent, LESS, RED);
    _setup_stub_node(&parent, &node, NULL, &grandparent, MORE, RED);
    _setup_stub_node(&grandparent, &uncle, &parent, NULL, MORE, RED);
    _setup_stub_node(&uncle, NULL, NULL, &grandparent, LESS, BLACK);
    t.root = NULL;
    
    _set_fix_data(&f_data, &parent, &grandparent, &uncle, NULL);
    ret = _rb_tree_determine_ins_case(&node, &f_data);
    ck_assert_int_eq(ret, 3);
    

    //case 4
    _setup_stub_node(&node, NULL, NULL, &parent, LESS, RED);
    _setup_stub_node(&parent, &node, NULL, &grandparent, LESS, RED);
    _setup_stub_node(&grandparent, &parent, &uncle, NULL, MORE, RED);
    _setup_stub_node(&uncle, NULL, NULL, &grandparent, MORE, BLACK);
    t.root = NULL;
    
    _set_fix_data(&f_data, &parent, &grandparent, &uncle, NULL);
    ret = _rb_tree_determine_ins_case(&node, &f_data);
    ck_assert_int_eq(ret, 4);

    t.root = NULL;
    return;

} END_TEST



//_rb_tree_determine_rem_case() [no fixture]
START_TEST(test__determine_rem_case) {

    //int ret;
    //struct _rb_tree_fix_data f_data;

    int ret;
    
    struct _rb_tree_fix_data f_data;
    cm_rb_tree_node node, parent, sibling, c_nephew, f_nephew;


    //case 0:
    _setup_stub_node(&node, NULL, NULL, &parent, LESS, RED);
    _setup_stub_node(&parent, &node, NULL, NULL, ROOT, BLACK);
    
    _set_fix_data(&f_data, &parent, NULL, NULL, NULL);
    ret = _rb_tree_determine_rem_case(&f_data);
    ck_assert_int_eq(ret, 0);


    //case 1.0 - left side
    _setup_stub_node(&node, NULL, NULL, &parent, LESS, BLACK);
    _setup_stub_node(&parent, &node, &sibling, NULL, LESS, BLACK);
    _setup_stub_node(&sibling, &c_nephew, &f_nephew, &parent, MORE, RED);
    _setup_stub_node(&c_nephew, NULL, NULL, &sibling, LESS, BLACK);
    _setup_stub_node(&f_nephew, NULL, NULL, &sibling, MORE, BLACK);

    _set_fix_data(&f_data, &parent, NULL, NULL, &sibling);
    ret = _rb_tree_determine_rem_case(&f_data);
    ck_assert_int_eq(ret, 1);

    //case 1.1 - right side
    _setup_stub_node(&node, NULL, NULL, &parent, MORE, BLACK);
    _setup_stub_node(&parent, &sibling, &node, NULL, LESS, BLACK);
    _setup_stub_node(&sibling, &f_nephew, &c_nephew, &parent, LESS, RED);
    _setup_stub_node(&c_nephew, NULL, NULL, &sibling, MORE, BLACK);
    _setup_stub_node(&f_nephew, NULL, NULL, &sibling, LESS, BLACK);

    _set_fix_data(&f_data, &parent, NULL, NULL, &sibling);
    ret = _rb_tree_determine_rem_case(&f_data);
    ck_assert_int_eq(ret, 1);


    //case 2.0 - left side
    _setup_stub_node(&node, NULL, NULL, &parent, LESS, BLACK);
    _setup_stub_node(&parent, &node, &sibling, NULL, LESS, BLACK);
    _setup_stub_node(&sibling, &c_nephew, &f_nephew, &parent, MORE, BLACK);
    _setup_stub_node(&c_nephew, NULL, NULL, &sibling, LESS, BLACK);
    _setup_stub_node(&f_nephew, NULL, NULL, &sibling, MORE, BLACK);

    _set_fix_data(&f_data, &parent, NULL, NULL, &sibling);
    ret = _rb_tree_determine_rem_case(&f_data);
    ck_assert_int_eq(ret, 2);


    //case 2.1 - right side    
    _setup_stub_node(&node, NULL, NULL, &parent, MORE, BLACK);
    _setup_stub_node(&parent, &sibling, &node, NULL, LESS, BLACK);
    _setup_stub_node(&sibling, &f_nephew, &c_nephew, &parent, LESS, BLACK);
    _setup_stub_node(&c_nephew, NULL, NULL, &sibling, MORE, BLACK);
    _setup_stub_node(&f_nephew, NULL, NULL, &sibling, LESS, BLACK);

    _set_fix_data(&f_data, &parent, NULL, NULL, &sibling);
    ret = _rb_tree_determine_rem_case(&f_data);
    ck_assert_int_eq(ret, 2);


    //case 3.0 - left side    
    _setup_stub_node(&node, NULL, NULL, &parent, LESS, BLACK);
    _setup_stub_node(&parent, &node, &sibling, NULL, LESS, BLACK);
    _setup_stub_node(&sibling, &c_nephew, &f_nephew, &parent, MORE, BLACK);
    _setup_stub_node(&c_nephew, NULL, NULL, &sibling, LESS, RED);
    _setup_stub_node(&f_nephew, NULL, NULL, &sibling, MORE, BLACK);

    _set_fix_data(&f_data, &parent, NULL, NULL, &sibling);
    ret = _rb_tree_determine_rem_case(&f_data);
    ck_assert_int_eq(ret, 3);


    //case 3.1 - right side
    _setup_stub_node(&node, NULL, NULL, &parent, MORE, BLACK);
    _setup_stub_node(&parent, &sibling, &node, NULL, LESS, RED);
    _setup_stub_node(&sibling, &f_nephew, &c_nephew, &parent, LESS, BLACK);
    _setup_stub_node(&c_nephew, NULL, NULL, &sibling, MORE, RED);
    _setup_stub_node(&f_nephew, NULL, NULL, &sibling, LESS, BLACK);

    _set_fix_data(&f_data, &parent, NULL, NULL, &sibling);
    ret = _rb_tree_determine_rem_case(&f_data);
    ck_assert_int_eq(ret, 3);
    
    
    //case 4.0 - left side    
    _setup_stub_node(&node, NULL, NULL, &parent, LESS, BLACK);
    _setup_stub_node(&parent, &node, &sibling, NULL, LESS, BLACK);
    _setup_stub_node(&sibling, NULL, &f_nephew, &parent, MORE, BLACK);
    _setup_stub_node(&f_nephew, NULL, NULL, &sibling, MORE, RED);

    _set_fix_data(&f_data, &parent, NULL, NULL, &sibling);
    ret = _rb_tree_determine_rem_case(&f_data);
    ck_assert_int_eq(ret, 4);


    //case 4.1 - right side
    _setup_stub_node(&node, NULL, NULL, &parent, MORE, BLACK);
    _setup_stub_node(&parent, &sibling, &node, NULL, LESS, RED);
    _setup_stub_node(&sibling, &f_nephew, NULL, &parent, LESS, BLACK);
    _setup_stub_node(&f_nephew, NULL, NULL, &sibling, LESS, RED);

    _set_fix_data(&f_data, &parent, NULL, NULL, &sibling);
    ret = _rb_tree_determine_rem_case(&f_data);
    ck_assert_int_eq(ret, 4);
    
    return;

} END_TEST



//_rb_tree_ins_case_1 [stub fixture]
START_TEST(test__ins_case_1) {

    //int ret;
    //struct _rb_tree_fix_data f_data;
    
    //_set_fix_data(&f_data, NULL, t.root, NULL, t.root->right);
    
    


    return;

} END_TEST



#endif

/*
 *  TODO
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


Suite * rb_tree_suite() {

    //test cases
    TCase * tc_new_cm_rb_tree;
    TCase * tc_del_cm_rb_tree;

    //internal test cases for debug builds
    #ifdef DEBUG
    TCase * tc__new_node;
    TCase * tc__del_node;
    TCase * tc__left_rotate;
    TCase * tc__right_rotate;
    TCase * tc__transplant;
    TCase * tc__right_min;
    TCase * tc__get_colour;
    TCase * tc__populate_fix_data;
    TCase * tc__determine_ins_case;
    TCase * tc__determine_rem_case;
    #endif

    Suite * s = suite_create("rb_tree");


    //tc_new_cm_rb_tree()
    tc_new_cm_rb_tree = tcase_create("new_cm_rb_tree");
    tcase_add_test(tc_new_cm_rb_tree, test_new_cm_rb_tree);

    //tc_del_cm_rb_tree()
    tc_del_cm_rb_tree = tcase_create("del_cm_rb_tree");
    tcase_add_checked_fixture(tc_del_cm_rb_tree, _setup_stub, NULL);
    tcase_add_test(tc_del_cm_rb_tree, test_del_cm_rb_tree);

    #ifdef DEBUG
    //tc__new_node()
    tc__new_node = tcase_create("_new_node");
    tcase_add_test(tc__new_node, test__new_node);
    
    //tc__del_node()
    tc__del_node = tcase_create("_del_node");
    tcase_add_test(tc__del_node, test__del_node);

    //tc__left_rotate()
    tc__left_rotate = tcase_create("_left_rotate");
    tcase_add_checked_fixture(tc__left_rotate, _setup_stub, teardown);
    tcase_add_test(tc__left_rotate, test__left_rotate);

    //tc__right_rotate()
    tc__right_rotate = tcase_create("_right_rotate");
    tcase_add_checked_fixture(tc__right_rotate, _setup_stub, teardown);
    tcase_add_test(tc__right_rotate, test__right_rotate);
    
    //tc__transplant()
    tc__transplant = tcase_create("_transplant");
    tcase_add_checked_fixture(tc__transplant, _setup_stub, teardown);
    tcase_add_test(tc__transplant, test__transplant);
    
    //tc__right_min()
    tc__right_min = tcase_create("_right_min");
    tcase_add_checked_fixture(tc__right_min, _setup_stub, teardown);
    tcase_add_test(tc__right_min, test__right_min);

    //tc__get_colour()
    tc__get_colour = tcase_create("_get_colour");
    tcase_add_checked_fixture(tc__get_colour, _setup_stub, teardown);
    tcase_add_test(tc__get_colour, test__get_colour);

    //tc__populate_fix_data()
    tc__populate_fix_data = tcase_create("_populate_fix_data");
    tcase_add_checked_fixture(tc__populate_fix_data, _setup_stub, teardown);
    tcase_add_test(tc__populate_fix_data, test__populate_fix_data);

    //tc__determine_ins_case()
    tc__determine_ins_case = tcase_create("_determine_ins_case");
    tcase_add_test(tc__determine_ins_case, test__determine_ins_case);

    //tc__determine_rem_case()
    tc__determine_rem_case = tcase_create("_determine_rem_case");
    tcase_add_test(tc__determine_rem_case, test__determine_rem_case);
    #endif


    //add test cases to red-black tree suite
    suite_add_tcase(s, tc_new_cm_rb_tree);
    suite_add_tcase(s, tc_del_cm_rb_tree);
    
    #ifdef DEBUG
    suite_add_tcase(s, tc__new_node);
    suite_add_tcase(s, tc__del_node);
    suite_add_tcase(s, tc__left_rotate);
    suite_add_tcase(s, tc__right_rotate);
    suite_add_tcase(s, tc__transplant);
    suite_add_tcase(s, tc__right_min);
    suite_add_tcase(s, tc__get_colour);
    suite_add_tcase(s, tc__populate_fix_data);
    suite_add_tcase(s, tc__determine_ins_case);
    suite_add_tcase(s, tc__determine_rem_case);
    #endif

    return s;
}
