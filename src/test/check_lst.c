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
#include "../lib/lst.h"


#define GET_NODE_DATA(node) ((data *) (node->data))


/*
 *  [BASIC TEST]
 *
 *      Lists are simple; internal functions 
 *      are tested through exported functions.
 */


/*
 *  --- [FIXTURES] ---
 */

//globals
static cm_lst l;
static data d;



//empty list setup
static void _setup_emp() {

    cm_new_lst(&l, sizeof(d));
    d.x = 0;

    return;
}



//false populated list setup
static void _setup_stub() {

    l.len = 3;
    l.data_sz = sizeof(d);
    
    cm_lst_node * n[3];
    
    //allocate each node
    for (int i = 0; i < 3; ++i) {
        n[i]       = malloc(sizeof(cm_lst_node));
        n[i]->data = malloc(l.data_sz);
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

    cm_new_lst(&l, sizeof(d));
    d.x = 0;

    for (int i = 0; i < TEST_LEN_FULL; ++i) {
        cm_lst_apd(&l, &d);
        d.x++;
    }

    return;
}



static void teardown() {

    cm_del_lst(&l);
    d.x = -1;

    return;
}



/*
 *  --- [HELPERS] ---
 */

static void _print_lst() {

    data e;

    //for each entry
    for (int i = 0; i < l.len; ++i) {

        cm_lst_get(&l, i, &e);
        printf("%d ", e.x);
        
    } //end for

    putchar('\n');

    return;
}



static void _assert_state(const int len, const int index, const int value) {

    ck_assert_int_eq(l.len, len);
    cm_lst_node * iter = l.head;

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

//cm_new_lst() [no fixture]
START_TEST(test_new_lst) {

    //run test
    cm_new_lst(&l, sizeof(data));
    
    //assert result
    ck_assert_int_eq(l.len, 0);
    ck_assert_int_eq(l.data_sz, sizeof(data));

    //cleanup
    cm_del_lst(&l);

    return;

} END_TEST



//cm_del_lst() [stub fixture]
START_TEST(test_del_lst) {

    /*
     *  Using ASAN to check for leaks here.
     */

    //run test
    cm_del_lst(&l);

    return;

} END_TEST



//cm_lst_apd() [empty fixture]
START_TEST(test_lst_apd) {

    cm_lst_node * n;

    //append to empty list
    n = cm_lst_apd(&l, &d);
    ck_assert_ptr_nonnull(n);
    _assert_state(1, 0, 0);
    
    d.x++;

    //append to non-empty list
    n = cm_lst_apd(&l, &d);
    ck_assert_ptr_nonnull(n);
    _assert_state(2, 1, 1); 

} END_TEST



//cm_lst_get() [full fixture]
START_TEST(test_lst_get) {

    int ret;
    
    data d;

    //get every list entry by value (positive index)
    for (int i = 0; i < TEST_LEN_FULL; ++i) {

        ret = cm_lst_get(&l, i, &d);
        ck_assert_int_eq(ret, 0);
        ck_assert_int_eq(d.x, i);

    } //end for

    //get every list entry by value (negative index)
    for (int i = -1; i > TEST_LEN_FULL * -1; --i) {

        ret = cm_lst_get(&l, i, &d);
        ck_assert_int_eq(ret, 0);
        ck_assert_int_eq(d.x, TEST_LEN_FULL + i);

    } //end for

    //get invalid index (+ve index)
    cm_errno = 0;
    ret = cm_lst_get(&l, TEST_LEN_FULL, &d);
    ck_assert_int_eq(ret, -1);
    ck_assert_int_eq(cm_errno, CM_ERR_USER_INDEX);

    //get invalid index (-ve index)
    cm_errno = 0;
    ret = cm_lst_get(&l, -TEST_LEN_FULL, &d);
    ck_assert_int_eq(ret, -1);
    ck_assert_int_eq(cm_errno, CM_ERR_USER_INDEX);

    return;
    
} END_TEST



//cm_lst_get_p() [full fixture]
START_TEST(test_lst_get_p) {

    data * p;

    //get every list entry by address (positive index)
    for (int i = 0; i < TEST_LEN_FULL; ++i) {

        p = (data *) cm_lst_get_p(&l, i);
        ck_assert_ptr_nonnull(p);
        ck_assert_int_eq(p->x, i);
    
    } //end for

    //get every list entry by address (negative index)
    for (int i = -1; i > TEST_LEN_FULL * -1; --i) {

        p = (data *) cm_lst_get_p(&l, i);
        ck_assert_ptr_nonnull(p);
        ck_assert_int_eq(p->x, TEST_LEN_FULL + i);

    } //end for

    //get ref to invalid index (+ve index)
    cm_errno = 0;
    p = (data *) cm_lst_get_p(&l, TEST_LEN_FULL);
    ck_assert_ptr_null(p);
    ck_assert_int_eq(cm_errno, CM_ERR_USER_INDEX);

    //get ref to invalid index (-ve index)
    cm_errno = 0;
    p = (data *) cm_lst_get_p(&l, -TEST_LEN_FULL);
    ck_assert_ptr_null(p);
    ck_assert_int_eq(cm_errno, CM_ERR_USER_INDEX);

    return;

} END_TEST



//cm_lst_get_p() [full fixture]
START_TEST(test_lst_get_n) {

    cm_lst_node * n;

    //get every list entry by address (positive index)
    for (int i = 0; i < TEST_LEN_FULL; ++i) {

        n = cm_lst_get_n(&l, i);
        ck_assert_ptr_nonnull(n);
        ck_assert_int_eq(GET_NODE_DATA(n)->x, i);
    
    } //end for

    //get every list entry by address (negative index)
    for (int i = -1; i > TEST_LEN_FULL * -1; --i) {

        n = cm_lst_get_n(&l, i);
        ck_assert_ptr_nonnull(n);
        ck_assert_int_eq(GET_NODE_DATA(n)->x, TEST_LEN_FULL + i);

    } //end for

    //get ref to invalid index (+ve index)
    cm_errno = 0;
    n = cm_lst_get_n(&l, TEST_LEN_FULL);
    ck_assert_ptr_null(n);
    ck_assert_int_eq(cm_errno, CM_ERR_USER_INDEX);

    //get ref to invalid index (-ve index)
    cm_errno = 0;
    n = cm_lst_get_n(&l, -TEST_LEN_FULL);
    ck_assert_ptr_null(n);
    ck_assert_int_eq(cm_errno, CM_ERR_USER_INDEX);

    return;

} END_TEST



//cm_lst_set [full fixture]
START_TEST(test_lst_set) {

    int ret;

    data e;
    cm_lst_node * n;


    //starting state of the list
    printf("[test_lst_set] starting values:             ");
    _print_lst();

    //set every list entry (positive index)
    for (int i = 0; i < TEST_LEN_FULL; ++i) {

        e.x = i * -1;
        n = cm_lst_set(&l, i, &e);
        ck_assert_ptr_nonnull(n);
        
        ck_assert_int_eq(GET_NODE_DATA(n)->x, i * -1);
        ret = cm_lst_get(&l, i, &d);
        ck_assert_int_eq(ret, 0);
        ck_assert_int_eq(d.x, i * -1);

    } //end for


    //state of the list after setting all values with a +ve index
    printf("[test_lst_set] final values (+ve index):    ");
    _print_lst();
    printf("[test_lst_set] expected values (+ve index): \
0 -1 -2 -3 -4 -5 -6 -7 -8 -9\n");


    //set every list entry (negative index)
    for (int i = (TEST_LEN_FULL * -1) + 1; i < 0; ++i) {

        //value of e.x will range from -11 to -19 (skip index 0)
        e.x = -TEST_LEN_FULL - (TEST_LEN_FULL + i);
        n = cm_lst_set(&l, i, &e);
        ck_assert_ptr_nonnull(n);

        ck_assert_int_eq(GET_NODE_DATA(n)->x, 
                         -TEST_LEN_FULL - (TEST_LEN_FULL + i));
        ret = cm_lst_get(&l, i, &d);
        ck_assert_int_eq(ret, 0);
        ck_assert_int_eq(d.x, -TEST_LEN_FULL - (TEST_LEN_FULL + i));

    } //end for


    //state of the list after setting all values with a -ve index
    printf("[test_lst_set] final values (-ve index):    ");
    _print_lst();
    printf("[test_lst_set] expected values (-ve index): \
0 -11 -12 -13 -14 -15 -16 -17 -18 -19\n");


    //set invalid index (+ve index)
    cm_errno = 0;
    n = cm_lst_set(&l, TEST_LEN_FULL, &d);
    ck_assert_ptr_null(n);
    ck_assert_int_eq(cm_errno, 1100);

    //set invalid index (-ve index)
    cm_errno = 0;
    n = cm_lst_set(&l, -TEST_LEN_FULL, &d);
    ck_assert_ptr_null(n);
    ck_assert_int_eq(cm_errno, 1100);

    return;

} END_TEST



//cm_lst_set_n [full fixture]
START_TEST(test_lst_set_n) {

    int ret;

    data e;
    cm_lst_node * n;


    //starting state of the list
    printf("[test_lst_set_n] starting values: ");
    _print_lst();

    //setup iteration
    n = l.head;

    //set every list entry
    for (int i = 0; i < TEST_LEN_FULL; ++i) {

        e.x = i * -1;
        n = cm_lst_set_n(&l, n, &e);
        ck_assert_ptr_eq(n, cm_lst_get_n(&l, i));
        
        ck_assert_int_eq(GET_NODE_DATA(n)->x, i * -1);
        ret = cm_lst_get(&l, i, &d);
        ck_assert_int_eq(ret, 0);
        ck_assert_int_eq(d.x, i * -1);

        n = n->next;

    } //end for


    //state of the list after setting all values with a +ve index
    printf("[test_lst_set_n] final values:    ");
    _print_lst();
    printf("[test_lst_set_n] expected values: \
0 -1 -2 -3 -4 -5 -6 -7 -8 -9\n");

    return;

} END_TEST



//cm_lst_ins [full fixture]
START_TEST(test_lst_ins) {

    int ret;
    int len = TEST_LEN_FULL;
    
    data e;
    cm_lst_node * n;


    //starting state of the list
    printf("[test_lst_ins] starting values: ");
    _print_lst();

    //insert in the third index (positive index)
    e.x = -1;
    n = cm_lst_ins(&l, 3, &e);
    ck_assert_ptr_nonnull(n);

    ck_assert_int_eq(GET_NODE_DATA(n)->x, -1);
    ret = cm_lst_get(&l, 3, &d);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(d.x, -1);
    len++;

    //insert in the third from last index (negative index)
    e.x = -2;
    n = cm_lst_ins(&l, -3, &e);
    ck_assert_ptr_nonnull(n);

    ck_assert_int_eq(GET_NODE_DATA(n)->x, -2);
    ret = cm_lst_get(&l, -3, &d);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(d.x, -2);
    len++;

    //insert at the end (positive index)
    e.x = -3;
    n = cm_lst_ins(&l, len, &e);
    ck_assert_ptr_nonnull(n);

    ck_assert_int_eq(GET_NODE_DATA(n)->x, -3);
    ret = cm_lst_get(&l, len, &d);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(d.x, -3);
    len++;

    //insert at the end (negative index)
    e.x = -4;
    n = cm_lst_ins(&l, -1, &e);
    ck_assert_ptr_nonnull(n);

    ck_assert_int_eq(GET_NODE_DATA(n)->x, -4);
    ret = cm_lst_get(&l, len, &d);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(d.x, -4);
    len++;

    //insert at the beginning (zero index)
    e.x = -5;
    n = cm_lst_ins(&l, 0, &e);
    ck_assert_ptr_nonnull(n);

    ck_assert_int_eq(GET_NODE_DATA(n)->x, -5);
    ret = cm_lst_get(&l, 0, &d);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(d.x, -5);
    len++;

    //insert at max negative index
    e.x = -6;
    n = cm_lst_ins(&l, len * -1, &e);
    ck_assert_ptr_nonnull(n);

    ck_assert_int_eq(GET_NODE_DATA(n)->x, -6);
    ret = cm_lst_get(&l, 1, &d);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(d.x, -6);
    len++;
    
    //state of the list after every type of insertion
    printf("[test_lst_ins] final values:    ");
    _print_lst();
    printf("[test_lst_ins] expected values: \
-5 -6 0 1 2 -1 3 4 5 6 7 -2 8 9 -3 -4\n");


    //insert invalid index (+ve index)
    cm_errno = 0;
    n = cm_lst_ins(&l, len+1, &e);
    ck_assert_ptr_null(n);
    ck_assert_int_eq(cm_errno, 1100);

    //insert invalid index (-ve index)
    cm_errno = 0;
    n = cm_lst_ins(&l, (len+1) * -1, &e);
    ck_assert_ptr_null(n);
    ck_assert_int_eq(cm_errno, 1100);

    return;

} END_TEST



//cm_lst_ins_nb [full fixture]
START_TEST(test_lst_ins_nb) {

    int ret;
    int len = TEST_LEN_FULL;
    
    data e;
    cm_lst_node * n;


    //starting state of the list
    printf("[test_lst_ins] starting values: ");
    _print_lst();

    //insert in the third index (positive index)
    e.x = -1;
    n = cm_lst_ins_nb(&l, l.head->next->next->next, &e);
    ck_assert_ptr_nonnull(n);

    ck_assert_int_eq(GET_NODE_DATA(n)->x, -1);
    ret = cm_lst_get(&l, 3, &d);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(d.x, -1);
    len++;

    //insert at the end (positive index)
    e.x = -2;
    n = cm_lst_ins_nb(&l, l.head->prev, &e);
    ck_assert_ptr_nonnull(n);

    ck_assert_int_eq(GET_NODE_DATA(n)->x, -2);
    ret = cm_lst_get(&l, len - 1, &d);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(d.x, -2);
    len++;

    //insert at the beginning (zero index)
    e.x = -3;
    n = cm_lst_ins_nb(&l, l.head, &e);
    ck_assert_ptr_nonnull(n);

    ck_assert_int_eq(GET_NODE_DATA(n)->x, -3);
    ret = cm_lst_get(&l, 0, &d);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(d.x, -3);
    len++;
    
    //state of the list after every type of insertion
    printf("[test_lst_ins] final values:    ");
    _print_lst();
    printf("[test_lst_ins] expected values: \
-3 0 1 2 -1 3 4 5 6 7 8 -2 9\n");

    return;

} END_TEST



//cm_lst_ins_na [full fixture]
START_TEST(test_lst_ins_na) {

    int ret;
    int len = TEST_LEN_FULL;
    
    data e;
    cm_lst_node * n;


    //starting state of the list
    printf("[test_lst_ins] starting values: ");
    _print_lst();

    //insert in the third index (positive index)
    e.x = -1;
    n = cm_lst_ins_na(&l, l.head->next->next, &e);
    ck_assert_ptr_nonnull(n);

    ck_assert_int_eq(GET_NODE_DATA(n)->x, -1);
    ret = cm_lst_get(&l, 3, &d);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(d.x, -1);
    len++;

    //insert at the end (positive index)
    e.x = -2;
    n = cm_lst_ins_na(&l, l.head->prev, &e);
    ck_assert_ptr_nonnull(n);

    ck_assert_int_eq(GET_NODE_DATA(n)->x, -2);
    ret = cm_lst_get(&l, len, &d);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(d.x, -2);
    len++;

    //insert at the beginning (zero index)
    e.x = -3;
    n = cm_lst_ins_na(&l, l.head, &e);
    ck_assert_ptr_nonnull(n);

    ck_assert_int_eq(GET_NODE_DATA(n)->x, -3);
    ret = cm_lst_get(&l, 1, &d);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(d.x, -3);
    len++;
    
    //state of the list after every type of insertion
    printf("[test_lst_ins] final values:    ");
    _print_lst();
    printf("[test_lst_ins] expected values: \
0 -3 1 2 -1 3 4 5 6 7 8 9 -2\n");

    return;

} END_TEST



//cm_lst_uln() [full fixture]
START_TEST(test_lst_uln) {

    int ret;
    int len = TEST_LEN_FULL;

    data e;
    cm_lst_node * n;


    //starting state of the list
    printf("[test_lst_uln] starting values: ");
    _print_lst();

    //unlink the third index (positive index)
    n = cm_lst_uln(&l, 3);
    ck_assert_ptr_nonnull(n);
    ck_assert_int_eq(l.len, len - 1);
    len--;

    ck_assert_int_eq(GET_NODE_DATA(n)->x, 3);
    cm_del_lst_node(n);

    ret = cm_lst_get(&l, 3, &e);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(e.x, 4);

    //unlink the third from last index (negative index)
    n = cm_lst_uln(&l, -3);
    ck_assert_ptr_nonnull(n);
    ck_assert_int_eq(l.len, len - 1);
    len--;

    ck_assert_int_eq(GET_NODE_DATA(n)->x, 7);
    cm_del_lst_node(n);

    ret = cm_lst_get(&l, -3, &e);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(e.x, 6);

    //unlink at end (positive index)
    n = cm_lst_uln(&l, len - 1);
    ck_assert_ptr_nonnull(n);
    ck_assert_int_eq(l.len, len - 1);
    len--;

    ck_assert_int_eq(GET_NODE_DATA(n)->x, 9);
    cm_del_lst_node(n);

    ret = cm_lst_get(&l, len - 1, &e);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(e.x, 8);

    //unlink at end (negative index)
    n = cm_lst_uln(&l, -1);
    ck_assert_ptr_nonnull(n);
    ck_assert_int_eq(l.len, len - 1);
    len--;

    ck_assert_int_eq(GET_NODE_DATA(n)->x, 8);
    cm_del_lst_node(n);

    ret = cm_lst_get(&l, len - 1, &e);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(e.x, 6);

    //unlink at start (zero index)
    n = cm_lst_uln(&l, 0);
    ck_assert_ptr_nonnull(n);
    ck_assert_int_eq(l.len, len - 1);
    len--;

    ck_assert_int_eq(GET_NODE_DATA(n)->x, 0);
    cm_del_lst_node(n);

    ret = cm_lst_get(&l, 0, &e);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(e.x, 1);

    //unlink at max negative index
    n = cm_lst_uln(&l, (len - 1) * -1);
    ck_assert_ptr_nonnull(n);
    ck_assert_int_eq(l.len, len - 1);
    len--;

    ck_assert_int_eq(GET_NODE_DATA(n)->x, 2);
    cm_del_lst_node(n);

    ret = cm_lst_get(&l, 1, &e);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(e.x, 4);


    //unlink invalid index (+ve index)
    cm_errno = 0;
    n = cm_lst_uln(&l, 99);
    ck_assert_ptr_null(n);
    ck_assert_int_eq(cm_errno, 1100);

    //unlink invalid index (-ve index)
    cm_errno = 0;
    n = cm_lst_uln(&l, -99);
    ck_assert_ptr_null(n);
    ck_assert_int_eq(cm_errno, 1100);


    //state of the list after every type of removal
    printf("[test_lst_uln] final values:    ");
    _print_lst();
    printf("[test_lst_uln] expected values: \
1 4 5 6\n");

    return;

} END_TEST



//cm_lst_uln_n() [full fixture]
START_TEST(test_lst_uln_n) {

    int ret;
    int len = TEST_LEN_FULL;

    data e;
    cm_lst_node * n;


    //starting state of the list
    printf("[test_lst_uln_n] starting values: ");
    _print_lst();

    //unlink the third index
    n = cm_lst_uln_n(&l, l.head->next->next->next);
    ck_assert_ptr_nonnull(n);
    ck_assert_int_eq(l.len, len - 1);
    len--;

    ck_assert_int_eq(GET_NODE_DATA(n)->x, 3);
    cm_del_lst_node(n);

    ret = cm_lst_get(&l, 3, &e);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(e.x, 4);

    //unlink at end
    n = cm_lst_uln_n(&l, l.head->prev);
    ck_assert_ptr_nonnull(n);
    ck_assert_int_eq(l.len, len - 1);
    len--;

    ck_assert_int_eq(GET_NODE_DATA(n)->x, 9);
    cm_del_lst_node(n);

    ret = cm_lst_get(&l, len - 1, &e);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(e.x, 8);

    //unlink at start
    n = cm_lst_uln_n(&l, l.head);
    ck_assert_ptr_nonnull(n);
    ck_assert_int_eq(l.len, len - 1);
    len--;

    ck_assert_int_eq(GET_NODE_DATA(n)->x, 0);
    cm_del_lst_node(n);

    ret = cm_lst_get(&l, 0, &e);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(e.x, 1);


    //state of the list after every type of removal
    printf("[test_lst_uln_n] final values:    ");
    _print_lst();
    printf("[test_lst_uln_n] expected values: \
1 2 4 5 6 7 8\n");

    return;

} END_TEST



//cm_lst_rem() [full fixture]
START_TEST(test_lst_rem) {

    int ret;
    int len = TEST_LEN_FULL;

    data e;


    //starting state of the list
    printf("[test_lst_rem] starting values: ");
    _print_lst();

    //remove the third index (positive index)
    ret = cm_lst_rem(&l, 3);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(l.len, len - 1);
    len--;

    ret = cm_lst_get(&l, 3, &e);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(e.x, 4);

    //remove the third from last index (negative index)
    ret = cm_lst_rem(&l, -3);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(l.len, len - 1);
    len--;

    ret = cm_lst_get(&l, -3, &e);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(e.x, 6);

    //remove at end (positive index)
    ret = cm_lst_rem(&l, len - 1);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(l.len, len - 1);
    len--;

    ret = cm_lst_get(&l, len - 1, &e);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(e.x, 8);

    //remove at end (negative index)
    ret = cm_lst_rem(&l, -1);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(l.len, len - 1);
    len--;

    ret = cm_lst_get(&l, len - 1, &e);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(e.x, 6);

    //remove at start (zero index)
    ret = cm_lst_rem(&l, 0);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(l.len, len - 1);
    len--;

    ret = cm_lst_get(&l, 0, &e);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(e.x, 1);

    //remove at max negative index
    ret = cm_lst_rem(&l, (len - 1) * -1);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(l.len, len - 1);
    len--;

    ret = cm_lst_get(&l, 1, &e);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(e.x, 4);


    //remove invalid index (+ve index)
    cm_errno = 0;
    ret = cm_lst_rem(&l, 99);
    ck_assert_int_eq(ret, -1);
    ck_assert_int_eq(cm_errno, 1100);

    //remove invalid index (-ve index)
    cm_errno = 0;
    ret = cm_lst_rem(&l, -99);
    ck_assert_int_eq(ret, -1);
    ck_assert_int_eq(cm_errno, 1100);


    //state of the list after every type of removal
    printf("[test_lst_rem] final values:    ");
    _print_lst();
    printf("[test_lst_rem] expected values: \
1 4 5 6\n");

    return;

} END_TEST;



//cm_lst_rem_n() [full fixture]
START_TEST(test_lst_rem_n) {

    int ret;
    int len = TEST_LEN_FULL;

    data e;


    //starting state of the list
    printf("[test_lst_rem_n] starting values: ");
    _print_lst();

    //remove the third index
    ret = cm_lst_rem_n(&l, l.head->next->next->next);
    ck_assert_int_eq(l.len, len - 1);
    len--;

    ret = cm_lst_get(&l, 3, &e);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(e.x, 4);

    //remove at end
    ret = cm_lst_rem_n(&l, l.head->prev);
    ck_assert_int_eq(l.len, len - 1);
    len--;

    ret = cm_lst_get(&l, len - 1, &e);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(e.x, 8);

    //remove at start
    ret = cm_lst_rem_n(&l, l.head);
    ck_assert_int_eq(l.len, len - 1);
    len--;

    ret = cm_lst_get(&l, 0, &e);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(e.x, 1);


    //state of the list after every type of removal
    printf("[test_lst_rem_n] final values:    ");
    _print_lst();
    printf("[test_lst_rem_n] expected values: \
1 2 4 5 6 7 8\n");

    return;

} END_TEST



//cm_lst_emp() [full fixture]
START_TEST(test_lst_emp) {

    /*
     *  Debug builds of cmore build with -fsanitize=address, which should
     *  catch memory leaks during emptying.
     */

    cm_lst_emp(&l);
    ck_assert_int_eq(l.len, 0);

    return;

} END_TEST;



/*
 *  --- [SUITE] ---
 */

Suite * lst_suite() {

    /*
     *  No test case for cm_del_lst_node; it is very simple.
     */

    //test cases
    TCase * tc_new_lst;
    TCase * tc_del_lst;
    TCase * tc_lst_apd;
    TCase * tc_lst_get;
    TCase * tc_lst_get_p;
    TCase * tc_lst_get_n;
    TCase * tc_lst_set;
    TCase * tc_lst_set_n;
    TCase * tc_lst_ins;
    TCase * tc_lst_ins_nb;
    TCase * tc_lst_ins_na;
    TCase * tc_lst_uln;
    TCase * tc_lst_uln_n;
    TCase * tc_lst_rem;
    TCase * tc_lst_rem_n;
    TCase * tc_lst_emp;

    Suite * s = suite_create("list");
    

    //cm_new_lst()
    tc_new_lst = tcase_create("new_lst");
    tcase_add_test(tc_new_lst, test_new_lst);
    
    //cm_del_lst()
    tc_del_lst = tcase_create("del_lst");
    tcase_add_checked_fixture(tc_del_lst, _setup_stub, NULL);
    tcase_add_test(tc_del_lst, test_del_lst);

    //cm_lst_apd()
    tc_lst_apd = tcase_create("list_apd");
    tcase_add_checked_fixture(tc_lst_apd, _setup_emp, teardown);   
    tcase_add_test(tc_lst_apd, test_lst_apd);

    //cm_lst_get()
    tc_lst_get = tcase_create("list_get");
    tcase_add_checked_fixture(tc_lst_get, _setup_full, teardown);
    tcase_add_test(tc_lst_get, test_lst_get);

    //cm_lst_get_p()
    tc_lst_get_p = tcase_create("list_get_p");
    tcase_add_checked_fixture(tc_lst_get_p, _setup_full, teardown);
    tcase_add_test(tc_lst_get_p, test_lst_get_p);

    //cm_lst_get_n()
    tc_lst_get_n = tcase_create("list_get_n");
    tcase_add_checked_fixture(tc_lst_get_n, _setup_full, teardown);
    tcase_add_test(tc_lst_get_n, test_lst_get_n);

    //cm_lst_set()
    tc_lst_set = tcase_create("list_set");
    tcase_add_checked_fixture(tc_lst_set, _setup_full, teardown);
    tcase_add_test(tc_lst_set, test_lst_set);

    //cm_lst_set_n()
    tc_lst_set_n = tcase_create("list_set_n");
    tcase_add_checked_fixture(tc_lst_set_n, _setup_full, teardown);
    tcase_add_test(tc_lst_set_n, test_lst_set_n);

    //cm_lst_ins()
    tc_lst_ins = tcase_create("list_ins");
    tcase_add_checked_fixture(tc_lst_ins, _setup_full, teardown);
    tcase_add_test(tc_lst_ins, test_lst_ins);

    //cm_lst_ins()
    tc_lst_ins_nb = tcase_create("list_ins_nb");
    tcase_add_checked_fixture(tc_lst_ins_nb, _setup_full, teardown);
    tcase_add_test(tc_lst_ins_nb, test_lst_ins_nb);

    //cm_lst_ins()
    tc_lst_ins_na = tcase_create("list_ins_na");
    tcase_add_checked_fixture(tc_lst_ins_na, _setup_full, teardown);
    tcase_add_test(tc_lst_ins_na, test_lst_ins_na);

    //cm_lst_uln()
    tc_lst_uln = tcase_create("list_uln");
    tcase_add_checked_fixture(tc_lst_uln, _setup_full, teardown);
    tcase_add_test(tc_lst_uln, test_lst_uln);

    //cm_lst_uln_n()
    tc_lst_uln_n = tcase_create("list_uln_n");
    tcase_add_checked_fixture(tc_lst_uln_n, _setup_full, teardown);
    tcase_add_test(tc_lst_uln_n, test_lst_uln_n);

    //cm_lst_rem()
    tc_lst_rem = tcase_create("list_rem");
    tcase_add_checked_fixture(tc_lst_rem, _setup_full, teardown);
    tcase_add_test(tc_lst_rem, test_lst_rem);

    //cm_lst_rem_n()
    tc_lst_rem_n = tcase_create("list_rem_n");
    tcase_add_checked_fixture(tc_lst_rem_n, _setup_full, teardown);
    tcase_add_test(tc_lst_rem_n, test_lst_rem_n);

    //cm_lst_emp()
    tc_lst_emp = tcase_create("list_emp");
    tcase_add_checked_fixture(tc_lst_emp, _setup_full, teardown);
    tcase_add_test(tc_lst_emp, test_lst_emp);


    //add test cases to list suite
    suite_add_tcase(s, tc_new_lst);
    suite_add_tcase(s, tc_del_lst);
    suite_add_tcase(s, tc_lst_apd);
    suite_add_tcase(s, tc_lst_get);
    suite_add_tcase(s, tc_lst_get_p);
    suite_add_tcase(s, tc_lst_get_n);
    suite_add_tcase(s, tc_lst_set);
    suite_add_tcase(s, tc_lst_set_n);
    suite_add_tcase(s, tc_lst_ins);
    suite_add_tcase(s, tc_lst_ins_nb);
    suite_add_tcase(s, tc_lst_ins_na);
    suite_add_tcase(s, tc_lst_uln);
    suite_add_tcase(s, tc_lst_uln_n);
    suite_add_tcase(s, tc_lst_rem);
    suite_add_tcase(s, tc_lst_rem_n);
    suite_add_tcase(s, tc_lst_emp);

    return s;
}
