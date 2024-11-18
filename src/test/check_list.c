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
#include "../lib/cmore.h"
#include "../lib/list.h"


#define GET_NODE_DATA(node) ((data *) (node->data))


/*
 *  [BASIC TEST]
 *
 *      Lists are simple; internal functions are tested through exported functions.
 */


/*
 *  --- [FIXTURES] ---
 */

//globals
static cm_list l;
static data d;



//empty list setup
static void _setup_empty() {

    cm_new_list(&l, sizeof(d));
    d.x = 0;

    return;
}



//false populated list setup
static void _setup_facade() {

    l.len = 3;
    l.data_size = sizeof(d);
    
    cm_list_node * n[3];
    
    //allocate each node
    for (int i = 0; i < 3; ++i) {
        n[i]       = malloc(sizeof(cm_list_node));
        n[i]->data = malloc(l.data_size);
    }

    //link n together
    l.head = n[0];

    n[0]->prev = n[2];
    n[0]->next = n[1];
    
    n[1]->prev = n[0];
    n[1]->next = n[2];

    n[2]->prev = n[1];
    n[2]->next = n[0];

    return;
}



///populated list setup
#define TEST_LEN_FULL 10
static void _setup_full() {

    cm_new_list(&l, sizeof(d));
    d.x = 0;

    for (int i = 0; i < TEST_LEN_FULL; ++i) {
        cm_list_append(&l, (cm_byte *) &d);
        d.x++;
    }

    return;
}



static void teardown() {

    cm_del_list(&l);
    d.x = -1;

    return;
}



/*
 *  --- [HELPERS] ---
 */

static void _print_list() {

    data e;

    //for each entry
    for (int i = 0; i < l.len; ++i) {

        cm_list_get_val(&l, i, (cm_byte *) &e);
        printf("%d ", e.x);
        
    } //end for

    putchar('\n');

    return;
}



static void _assert_state(const int len, const int index, const int value) {

    ck_assert_int_eq(l.len, len);
    cm_list_node * iter = l.head;

    //traverse list
    for (int i = 0; i < index; ++i) {
        iter = iter->next;
    }

    ck_assert_int_eq(GET_NODE_DATA(iter)->x, value);

    return;
}



/*
 *  --- [UNIT TESTS] ---
 */

//cm_new_list()
START_TEST(test_new_list) {

    //run test
    cm_new_list(&l, sizeof(data));
    
    //assert result
    ck_assert_int_eq(l.len, 0);
    ck_assert_int_eq(l.data_size, sizeof(data));

    //cleanup
    cm_del_list(&l);

    return;

} END_TEST



START_TEST(test_del_list) {

    /*
     *  Using ASAN to check for leaks here.
     */

    //run test
    cm_del_list(&l);

    return;

} END_TEST



//cm_list_append()
START_TEST(test_list_append) {

    cm_list_node * n;

    //append to empty list
    n = cm_list_append(&l, (cm_byte *) &d);
    ck_assert_ptr_nonnull(n);
    _assert_state(1, 0, 0);
    
    d.x++;

    //append to non-empty list
    n = cm_list_append(&l, (cm_byte *) &d);
    ck_assert_ptr_nonnull(n);
    _assert_state(2, 1, 1); 

} END_TEST



//cm_list_get_val()
START_TEST(test_list_get_val) {

    int ret;
    
    data d;

    //get every list entry by value (positive index)
    for (int i = 0; i < TEST_LEN_FULL; ++i) {

        ret = cm_list_get_val(&l, i, (cm_byte *) &d);
        ck_assert_int_eq(ret, 0);
        ck_assert_int_eq(d.x, i);

    } //end for

    //get every list entry by value (negative index)
    for (int i = -1; i > TEST_LEN_FULL * -1; --i) {

        ret = cm_list_get_val(&l, i, (cm_byte *) &d);
        ck_assert_int_eq(ret, 0);
        ck_assert_int_eq(d.x, TEST_LEN_FULL + i);

    } //end for

    //get invalid index (+ve index)
    cm_errno = 0;
    ret = cm_list_get_val(&l, TEST_LEN_FULL, (cm_byte *) &d);
    ck_assert_int_eq(ret, -1);
    ck_assert_int_eq(cm_errno, CM_ERR_USER_INDEX);

    //get invalid index (-ve index)
    cm_errno = 0;
    ret = cm_list_get_val(&l, -TEST_LEN_FULL, (cm_byte *) &d);
    ck_assert_int_eq(ret, -1);
    ck_assert_int_eq(cm_errno, CM_ERR_USER_INDEX);

    return;
    
} END_TEST



//cm_list_get_ref
START_TEST(test_list_get_ref) {

    data * p;

    //get every list entry by address (positive index)
    for (int i = 0; i < TEST_LEN_FULL; ++i) {

        p = (data *) cm_list_get_ref(&l, i);
        ck_assert_ptr_nonnull(p);
        ck_assert_int_eq(p->x, i);
    
    } //end for

    //get every list entry by address (negative index)
    for (int i = -1; i > TEST_LEN_FULL * -1; --i) {

        p = (data *) cm_list_get_ref(&l, i);
        ck_assert_ptr_nonnull(p);
        ck_assert_int_eq(p->x, TEST_LEN_FULL + i);

    } //end for

    //get ref to invalid index (+ve index)
    cm_errno = 0;
    p = (data *) cm_list_get_ref(&l, TEST_LEN_FULL);
    ck_assert_ptr_null(p);
    ck_assert_int_eq(cm_errno, CM_ERR_USER_INDEX);

    //get ref to invalid index (-ve index)
    cm_errno = 0;
    p = (data *) cm_list_get_ref(&l, -TEST_LEN_FULL);
    ck_assert_ptr_null(p);
    ck_assert_int_eq(cm_errno, CM_ERR_USER_INDEX);

    return;

} END_TEST



//cm_list_get_ref
START_TEST(test_list_get_node) {

    cm_list_node * n;

    //get every list entry by address (positive index)
    for (int i = 0; i < TEST_LEN_FULL; ++i) {

        n = cm_list_get_node(&l, i);
        ck_assert_ptr_nonnull(n);
        ck_assert_int_eq(GET_NODE_DATA(n)->x, i);
    
    } //end for

    //get every list entry by address (negative index)
    for (int i = -1; i > TEST_LEN_FULL * -1; --i) {

        n = cm_list_get_node(&l, i);
        ck_assert_ptr_nonnull(n);
        ck_assert_int_eq(GET_NODE_DATA(n)->x, TEST_LEN_FULL + i);

    } //end for

    //get ref to invalid index (+ve index)
    cm_errno = 0;
    n = cm_list_get_node(&l, TEST_LEN_FULL);
    ck_assert_ptr_null(n);
    ck_assert_int_eq(cm_errno, CM_ERR_USER_INDEX);

    //get ref to invalid index (-ve index)
    cm_errno = 0;
    n = cm_list_get_node(&l, -TEST_LEN_FULL);
    ck_assert_ptr_null(n);
    ck_assert_int_eq(cm_errno, CM_ERR_USER_INDEX);

    return;

} END_TEST



//cm_list_set
START_TEST(test_list_set) {

    int ret;

    data e;
    cm_list_node * n;


    //starting state of the list
    printf("[test_list_set] starting values:             ");
    _print_list();

    //set every list entry (positive index)
    for (int i = 0; i < TEST_LEN_FULL; ++i) {

        e.x = i * -1;
        n = cm_list_set(&l, i, (cm_byte *) &e);
        ck_assert_ptr_nonnull(n);
        
        ck_assert_int_eq(GET_NODE_DATA(n)->x, i * -1);
        ret = cm_list_get_val(&l, i, (cm_byte *) &d);
        ck_assert_int_eq(ret, 0);
        ck_assert_int_eq(d.x, i * -1);

    } //end for


    //state of the list after setting all values with a +ve index
    printf("[test_list_set] final values (+ve index):    ");
    _print_list();
    printf("[test_list_set] expected values (+ve index): \
0 -1 -2 -3 -4 -5 -6 -7 -8 -9\n");


    //set every list entry (negative index)
    for (int i = (TEST_LEN_FULL * -1) + 1; i < 0; ++i) {

        //value of e.x will range from -11 to -19 (skip index 0)
        e.x = -TEST_LEN_FULL - (TEST_LEN_FULL + i);
        n = cm_list_set(&l, i, (cm_byte *) &e);
        ck_assert_ptr_nonnull(n);

        ck_assert_int_eq(GET_NODE_DATA(n)->x, -TEST_LEN_FULL - (TEST_LEN_FULL + i));
        ret = cm_list_get_val(&l, i, (cm_byte *) &d);
        ck_assert_int_eq(ret, 0);
        ck_assert_int_eq(d.x, -TEST_LEN_FULL - (TEST_LEN_FULL + i));

    } //end for


    //state of the list after setting all values with a -ve index
    printf("[test_list_set] final values (-ve index):    ");
    _print_list();
    printf("[test_list_set] expected values (-ve index): \
0 -11 -12 -13 -14 -15 -16 -17 -18 -19\n");


    //set invalid index (+ve index)
    cm_errno = 0;
    n = cm_list_set(&l, TEST_LEN_FULL, (cm_byte *) &d);
    ck_assert_ptr_null(n);
    ck_assert_int_eq(cm_errno, 1100);

    //set invalid index (-ve index)
    cm_errno = 0;
    n = cm_list_set(&l, -TEST_LEN_FULL, (cm_byte *) &d);
    ck_assert_ptr_null(n);
    ck_assert_int_eq(cm_errno, 1100);

    return;

} END_TEST



//cm_list_insert
START_TEST(test_list_insert) {

    int ret;
    int len = TEST_LEN_FULL;
    
    data e;
    cm_list_node * n;


    //starting state of the list
    printf("[test_list_insert] starting values: ");
    _print_list();

    //insert in the third index (positive index)
    e.x = -1;
    n = cm_list_insert(&l, 3, (cm_byte *) &e);
    ck_assert_ptr_nonnull(n);

    ck_assert_int_eq(GET_NODE_DATA(n)->x, -1);
    ret = cm_list_get_val(&l, 3, (cm_byte *) &d);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(d.x, -1);
    len++;

    //insert in the third from last index (negative index)
    e.x = -2;
    n = cm_list_insert(&l, -3, (cm_byte *) &e);
    ck_assert_ptr_nonnull(n);

    ck_assert_int_eq(GET_NODE_DATA(n)->x, -2);
    ret = cm_list_get_val(&l, -3, (cm_byte *) &d);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(d.x, -2);
    len++;

    //insert at the end (positive index)
    e.x = -3;
    n = cm_list_insert(&l, len, (cm_byte *) &e);
    ck_assert_ptr_nonnull(n);

    ck_assert_int_eq(GET_NODE_DATA(n)->x, -3);
    ret = cm_list_get_val(&l, len, (cm_byte *) &d);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(d.x, -3);
    len++;

    //insert at the end (negative index)
    e.x = -4;
    n = cm_list_insert(&l, -1, (cm_byte *) &e);
    ck_assert_ptr_nonnull(n);

    ck_assert_int_eq(GET_NODE_DATA(n)->x, -4);
    ret = cm_list_get_val(&l, len, (cm_byte *) &d);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(d.x, -4);
    len++;

    //insert at the beginning (zero index)
    e.x = -5;
    n = cm_list_insert(&l, 0, (cm_byte *) &e);
    ck_assert_ptr_nonnull(n);

    ck_assert_int_eq(GET_NODE_DATA(n)->x, -5);
    ret = cm_list_get_val(&l, 0, (cm_byte *) &d);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(d.x, -5);
    len++;

    //insert at max negative index
    e.x = -6;
    n = cm_list_insert(&l, len * -1, (cm_byte *) &e);
    ck_assert_ptr_nonnull(n);

    ck_assert_int_eq(GET_NODE_DATA(n)->x, -6);
    ret = cm_list_get_val(&l, 1, (cm_byte *) &d);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(d.x, -6);
    len++;
    
    //state of the list after every type of insertion
    printf("[test_list_insert] final values:    ");
    _print_list();
    printf("[test_list_insert] expected values: \
-5 -6 0 1 2 -1 3 4 5 6 7 -2 8 9 -3 -4\n");


    //insert invalid index (+ve index)
    cm_errno = 0;
    n = cm_list_insert(&l, len+1, (cm_byte *) &e);
    ck_assert_ptr_null(n);
    ck_assert_int_eq(cm_errno, 1100);

    //insert invalid index (-ve index)
    cm_errno = 0;
    n = cm_list_insert(&l, (len+1) * -1, (cm_byte *) &e);
    ck_assert_ptr_null(n);
    ck_assert_int_eq(cm_errno, 1100);

    return;

} END_TEST



//cm_list_unlink()
START_TEST(test_list_unlink) {

    /*
     *  TODO This test depends on the _setup_full() initialisation. If it 
     *       is changed, the values this test compares against must be 
     *       adjusted.
     */

    int ret;
    int len = TEST_LEN_FULL;

    data e;
    cm_list_node * n;


    //starting state of the list
    printf("[test_list_unlink] starting values: ");
    _print_list();

    //remove the third index (positive index)
    n = cm_list_unlink(&l, 3);
    ck_assert_ptr_nonnull(n);
    ck_assert_int_eq(l.len, len - 1);
    len--;

    ck_assert_int_eq(GET_NODE_DATA(n)->x, 3);
    cm_del_list_node(n);

    ret = cm_list_get_val(&l, 3, (cm_byte *) &e);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(e.x, 4);

    //remove the third from last index (negative index)
    n = cm_list_unlink(&l, -3);
    ck_assert_ptr_nonnull(n);
    ck_assert_int_eq(l.len, len - 1);
    len--;

    ck_assert_int_eq(GET_NODE_DATA(n)->x, 7);
    cm_del_list_node(n);

    ret = cm_list_get_val(&l, -3, (cm_byte *) &e);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(e.x, 6);

    //remove at end (positive index)
    n = cm_list_unlink(&l, len - 1);
    ck_assert_ptr_nonnull(n);
    ck_assert_int_eq(l.len, len - 1);
    len--;

    ck_assert_int_eq(GET_NODE_DATA(n)->x, 9);
    cm_del_list_node(n);

    ret = cm_list_get_val(&l, len - 1, (cm_byte *) &e);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(e.x, 8);

    //remove at end (negative index)
    n = cm_list_unlink(&l, -1);
    ck_assert_ptr_nonnull(n);
    ck_assert_int_eq(l.len, len - 1);
    len--;

    ck_assert_int_eq(GET_NODE_DATA(n)->x, 8);
    cm_del_list_node(n);

    ret = cm_list_get_val(&l, len - 1, (cm_byte *) &e);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(e.x, 6);

    //remove at start (zero index)
    n = cm_list_unlink(&l, 0);
    ck_assert_ptr_nonnull(n);
    ck_assert_int_eq(l.len, len - 1);
    len--;

    ck_assert_int_eq(GET_NODE_DATA(n)->x, 0);
    cm_del_list_node(n);

    ret = cm_list_get_val(&l, 0, (cm_byte *) &e);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(e.x, 1);

    //remove at max negative index
    n = cm_list_unlink(&l, (len - 1) * -1);
    ck_assert_ptr_nonnull(n);
    ck_assert_int_eq(l.len, len - 1);
    len--;

    ck_assert_int_eq(GET_NODE_DATA(n)->x, 2);
    cm_del_list_node(n);

    ret = cm_list_get_val(&l, 1, (cm_byte *) &e);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(e.x, 4);


    //remove invalid index (+ve index)
    cm_errno = 0;
    n = cm_list_unlink(&l, 99);
    ck_assert_ptr_null(n);
    ck_assert_int_eq(cm_errno, 1100);

    //remove invalid index (-ve index)
    cm_errno = 0;
    n = cm_list_unlink(&l, -99);
    ck_assert_ptr_null(n);
    ck_assert_int_eq(cm_errno, 1100);


    //state of the list after every type of removal
    printf("[test_list_unlink] final values:    ");
    _print_list();
    printf("[test_list_unlink] expected values: \
1 4 5 6\n");

    return;

} END_TEST



//cm_list_remove()
START_TEST(test_list_remove) {

    /*
     *  TODO This test depends on the _setup_full() initialisation. If it 
     *       is changed, the values this test compares against must be 
     *       adjusted.
     */

    int ret;
    int len = TEST_LEN_FULL;

    data e;


    //starting state of the list
    printf("[test_list_remove] starting values: ");
    _print_list();

    //remove the third index (positive index)
    ret = cm_list_remove(&l, 3);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(l.len, len - 1);
    len--;

    ret = cm_list_get_val(&l, 3, (cm_byte *) &e);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(e.x, 4);

    //remove the third from last index (negative index)
    ret = cm_list_remove(&l, -3);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(l.len, len - 1);
    len--;

    ret = cm_list_get_val(&l, -3, (cm_byte *) &e);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(e.x, 6);

    //remove at end (positive index)
    ret = cm_list_remove(&l, len - 1);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(l.len, len - 1);
    len--;

    ret = cm_list_get_val(&l, len - 1, (cm_byte *) &e);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(e.x, 8);

    //remove at end (negative index)
    ret = cm_list_remove(&l, -1);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(l.len, len - 1);
    len--;

    ret = cm_list_get_val(&l, len - 1, (cm_byte *) &e);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(e.x, 6);

    //remove at start (zero index)
    ret = cm_list_remove(&l, 0);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(l.len, len - 1);
    len--;

    ret = cm_list_get_val(&l, 0, (cm_byte *) &e);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(e.x, 1);

    //remove at max negative index
    ret = cm_list_remove(&l, (len - 1) * -1);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(l.len, len - 1);
    len--;

    ret = cm_list_get_val(&l, 1, (cm_byte *) &e);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(e.x, 4);


    //remove invalid index (+ve index)
    cm_errno = 0;
    ret = cm_list_remove(&l, 99);
    ck_assert_int_eq(ret, -1);
    ck_assert_int_eq(cm_errno, 1100);

    //remove invalid index (-ve index)
    cm_errno = 0;
    ret = cm_list_remove(&l, -99);
    ck_assert_int_eq(ret, -1);
    ck_assert_int_eq(cm_errno, 1100);


    //state of the list after every type of removal
    printf("[test_list_remove] final values:    ");
    _print_list();
    printf("[test_list_remove] expected values: \
1 4 5 6\n");

    return;

} END_TEST;



//cm_list_empty()
START_TEST(test_list_empty) {

    /*
     *  Debug builds of cmore build with -fsanitize=address, which should
     *  catch memory leaks during emptying.
     */

    cm_list_empty(&l);
    ck_assert_int_eq(l.len, 0);

    return;

} END_TEST;



/*
 *  --- [SUITE] ---
 */

Suite * list_suite() {

    /*
     *  No test case for cm_del_list_node; it is very simple.
     */

    //test cases
    TCase * tc_new_list;
    TCase * tc_del_list;
    TCase * tc_list_append;
    TCase * tc_list_get_val;
    TCase * tc_list_get_ref;
    TCase * tc_list_get_node;
    TCase * tc_list_set;
    TCase * tc_list_insert;
    TCase * tc_list_unlink;
    TCase * tc_list_remove;
    TCase * tc_list_empty;

    Suite * s = suite_create("list");
    

    //cm_new_list()
    tc_new_list = tcase_create("new_list");
    tcase_add_test(tc_new_list, test_new_list);
    
    //cm_del_list()
    tc_del_list = tcase_create("del_list");
    tcase_add_checked_fixture(tc_del_list, _setup_facade, NULL);
    tcase_add_test(tc_del_list, test_del_list);

    //cm_list_append()
    tc_list_append = tcase_create("list_append");
    tcase_add_checked_fixture(tc_list_append, _setup_empty, teardown);   
    tcase_add_test(tc_list_append, test_list_append);

    //cm_list_get_val()
    tc_list_get_val = tcase_create("list_get_val");
    tcase_add_checked_fixture(tc_list_get_val, _setup_full, teardown);
    tcase_add_test(tc_list_get_val, test_list_get_val);

    //cm_list_get_ref()
    tc_list_get_ref = tcase_create("list_get_ref");
    tcase_add_checked_fixture(tc_list_get_ref, _setup_full, teardown);
    tcase_add_test(tc_list_get_ref, test_list_get_ref);

    //cm_list_get_node()
    tc_list_get_node = tcase_create("list_get_node");
    tcase_add_checked_fixture(tc_list_get_node, _setup_full, teardown);
    tcase_add_test(tc_list_get_node, test_list_get_node);

    //cm_list_set()
    tc_list_set = tcase_create("list_set");
    tcase_add_checked_fixture(tc_list_set, _setup_full, teardown);
    tcase_add_test(tc_list_set, test_list_set);

    //cm_list_insert()
    tc_list_insert = tcase_create("list_insert");
    tcase_add_checked_fixture(tc_list_insert, _setup_full, teardown);
    tcase_add_test(tc_list_insert, test_list_insert);

    //cm_list_unlink()
    tc_list_unlink = tcase_create("list_unlink");
    tcase_add_checked_fixture(tc_list_unlink, _setup_full, teardown);
    tcase_add_test(tc_list_unlink, test_list_unlink);

    //cm_list_remove()
    tc_list_remove = tcase_create("list_remove");
    tcase_add_checked_fixture(tc_list_remove, _setup_full, teardown);
    tcase_add_test(tc_list_remove, test_list_remove);

    //cm_list_empty()
    tc_list_empty = tcase_create("list_empty");
    tcase_add_checked_fixture(tc_list_empty, _setup_full, teardown);
    tcase_add_test(tc_list_empty, test_list_empty);


    //add test cases to list suite
    suite_add_tcase(s, tc_new_list);
    suite_add_tcase(s, tc_del_list);
    suite_add_tcase(s, tc_list_append);
    suite_add_tcase(s, tc_list_get_val);
    suite_add_tcase(s, tc_list_get_ref);
    suite_add_tcase(s, tc_list_get_node);
    suite_add_tcase(s, tc_list_set);
    suite_add_tcase(s, tc_list_insert);
    suite_add_tcase(s, tc_list_unlink);
    suite_add_tcase(s, tc_list_remove);
    suite_add_tcase(s, tc_list_empty);

    return s;
}
