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
#include "../lib/vct.h"



/*
 *  [BASIC TEST]
 *
 *     Vectors are simple; internal functions 
 *     are tested through exported functions.
 */



//globals
static cm_vct v;
static data d;




/*
 *  --- [HELPERS] ---
 */

#define V_GET(v, i) (((data *) v.data) + i)



static void _print_vct() {

    data e;

    //for each entry
    for (int i = 0; i < v.len; ++i) {

        cm_vct_get(&v, i, &e);
        printf("%d ", e.x);
        
    } //end for

    putchar('\n');

    return;
}



static int _power(int base, const int exp) {

    int result = 1;

    for (int i = 0; i < exp; ++i) {
        result = result * base;
    }

    return result;
}



//assert the state of a vector & a specific index into the vector
static void _assert_state(const int len, const size_t size, 
                          const int index, const int value) {

    ck_assert_int_eq(v.len, len);
    ck_assert_int_eq(v.sz, size);
    ck_assert_int_eq(V_GET(v, index)->x, value);
}



/*
 *  --- [FIXTURES] ---
 */

//empty vector setup
static void _setup_emp() {

    cm_new_vct(&v, sizeof(d));
    d.x = 0;

    return;
}



//false populated vector setup
static void _setup_stub() {

    /*
     *  Stub vector:
     *
     *  [v, v, v, v, v, v, v, v, v, v] where v is an undefined value
     */

    v.len = 10;
    v.sz = VECTOR_DEFAULT_SIZE;
    v.data_sz = sizeof(d);
    v.data = malloc(v.data_sz * v.sz);

    return;
}



//populated vector setup
#define TEST_LEN_FULL 10
static void _setup_full() {

    /*
     *  Full vector:
     *
     *  [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
     */

    cm_new_vct(&v, sizeof(d));
    d.x = 0;

    for (int i = 0; i < TEST_LEN_FULL; ++i) {
        cm_vct_apd(&v, &d);
        d.x++;
    }

    return;
}



static void _teardown() {

    cm_del_vct(&v);
    d.x = -1;

    return;
}



/*
 *  --- [UNIT TESTS] ---
 */

//cm_new_vct() & cm_del_vct() [no fixture]
START_TEST(test_new_del_vct) {

    //only test: create a new vector & destroy it
    int ret = cm_new_vct(&v, sizeof(data));
    
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(v.len, 0);
    ck_assert_int_eq(v.sz, VECTOR_DEFAULT_SIZE);
    ck_assert_int_eq(v.data_sz, sizeof(data));

    cm_del_vct(&v);

    return;

} END_TEST



//cm_vct_apd() [empty fixture]
START_TEST(test_vct_apd) {

    int ret;


    //first test: append to an empty vector
    ret = cm_vct_apd(&v, &d);
    ck_assert_int_eq(ret, 0);
    _assert_state(1, VECTOR_DEFAULT_SIZE, 0, 0);
    
    d.x++;

    //second test: append to a non-empty vector
    ret = cm_vct_apd(&v, &d);
    ck_assert_int_eq(ret, 0);
    _assert_state(2, VECTOR_DEFAULT_SIZE, 1, 1); 

} END_TEST



//_grow() [empty fixture]
START_TEST(test__grow) {

    int ret;

    //only test: append repeatedly to grow allocated area
    for (int i = 0; i < (VECTOR_DEFAULT_SIZE * 2) + 1; ++i) {

        ret = cm_vct_apd(&v, &d);
        d.x++;

        ck_assert_int_eq(ret, 0);
        _assert_state(i+1, VECTOR_DEFAULT_SIZE 
                           * _power(2, (i / VECTOR_DEFAULT_SIZE)), i, i);

    } //end for

    return;
}



//cm_vct_get() [full fixture]
START_TEST(test_vct_get) {

    int ret;
    
    data d;

    //first test: get every vector entry by value (positive index)
    for (int i = 0; i < TEST_LEN_FULL; ++i) {

        ret = cm_vct_get(&v, i, &d);
        ck_assert_int_eq(ret, 0);
        ck_assert_int_eq(d.x, i);

    } //end for


    //second test: get every vector entry by value (negative index)
    for (int i = -1; i > TEST_LEN_FULL * -1; --i) {

        ret = cm_vct_get(&v, i, &d);
        ck_assert_int_eq(ret, 0);
        ck_assert_int_eq(d.x, TEST_LEN_FULL + i);

    } //end for


    //third test: get invalid index (+ve index)
    cm_errno = 0;
    ret = cm_vct_get(&v, TEST_LEN_FULL, &d);
    ck_assert_int_eq(ret, -1);
    ck_assert_int_eq(cm_errno, CM_ERR_USER_INDEX);


    //fourth test: get invalid index (-ve index)
    cm_errno = 0;
    ret = cm_vct_get(&v, -TEST_LEN_FULL - 1, &d);
    ck_assert_int_eq(ret, -1);
    ck_assert_int_eq(cm_errno, CM_ERR_USER_INDEX);

    return;
    
} END_TEST



//cm_vct_get_p() [full fixture]
START_TEST(test_vct_get_p) {

    data * p;

    //first test: get every vector entry by address (positive index)
    for (int i = 0; i < TEST_LEN_FULL; ++i) {

        p = (data *) cm_vct_get_p(&v, i);
        ck_assert_ptr_nonnull(p);
        ck_assert_int_eq(p->x, i);
    
    } //end for


    //second test: get every vector entry by address (negative index)
    for (int i = -1; i > TEST_LEN_FULL * -1; --i) {

        p = (data *) cm_vct_get_p(&v, i);
        ck_assert_ptr_nonnull(p);
        ck_assert_int_eq(p->x, TEST_LEN_FULL + i);

    } //end for


    //third test: get ref to invalid index (+ve index)
    cm_errno = 0;
    p = (data *) cm_vct_get_p(&v, TEST_LEN_FULL);
    ck_assert_ptr_null(p);
    ck_assert_int_eq(cm_errno, CM_ERR_USER_INDEX);


    //fourth test: get ref to invalid index (-ve index)
    cm_errno = 0;
    p = (data *) cm_vct_get_p(&v, -TEST_LEN_FULL - 1);
    ck_assert_ptr_null(p);
    ck_assert_int_eq(cm_errno, CM_ERR_USER_INDEX);

    return;

} END_TEST



//cm_vct_set() [full fixture]
START_TEST(test_vct_set) {

    int ret;
    
    data e;


    //output the starting state of the vector
    printf("[test_vct_set] starting values:             ");
    _print_vct();

    //first test: set every vector entry (positive index)
    for (int i = 0; i < TEST_LEN_FULL; ++i) {

        e.x = i * -1;
        ret = cm_vct_set(&v, i, &e);
        ck_assert_int_eq(ret, 0);
        
        ret = cm_vct_get(&v, i, &d);
        ck_assert_int_eq(ret, 0);
        ck_assert_int_eq(d.x, i * -1);

    } //end for

    //output the state of the vector after setting all values with a +ve index
    printf("[test_vct_set] final values (+ve index):    ");
    _print_vct();
    printf("[test_vct_set] expected values (+ve index): \
0 -1 -2 -3 -4 -5 -6 -7 -8 -9\n");


    //second test: set every vector entry (negative index)
    for (int i = TEST_LEN_FULL * -1; i < 0; ++i) {

        //value of e.x will range from -11 to -19 (skip index 0)
        e.x = -TEST_LEN_FULL - (TEST_LEN_FULL + i);
        ret = cm_vct_set(&v, i, &e);
        ck_assert_int_eq(ret, 0);

        ret = cm_vct_get(&v, i, &d);
        ck_assert_int_eq(ret, 0);
        ck_assert_int_eq(d.x, -TEST_LEN_FULL - (TEST_LEN_FULL + i));

    } //end for


    //output the state of the vector after setting all values with a -ve index
    printf("[test_vct_set] final values (-ve index):    ");
    _print_vct();
    printf("[test_vct_set] expected values (-ve index): \
-10 -11 -12 -13 -14 -15 -16 -17 -18 -19\n");


    //third test: set invalid index (+ve index)
    cm_errno = 0;
    ret = cm_vct_set(&v, TEST_LEN_FULL, &d);
    ck_assert_int_eq(ret, -1);
    ck_assert_int_eq(cm_errno, 1100);


    //fourth test: set invalid index (-ve index)
    cm_errno = 0;
    ret = cm_vct_set(&v, -TEST_LEN_FULL - 1, &d);
    ck_assert_int_eq(ret, -1);
    ck_assert_int_eq(cm_errno, 1100);

    return;

} END_TEST



//cm_vct_ins() [full fixture]
START_TEST(test_vct_ins) {

    int ret;
    int len = TEST_LEN_FULL;
    
    data e;


    //output the starting state of the vector
    printf("[test_vct_ins] starting values: ");
    _print_vct();

    //first test: insert in the third index (positive index)
    e.x = -1;
    ret = cm_vct_ins(&v, 3, &e);
    ck_assert_int_eq(ret, 0);

    ret = cm_vct_get(&v, 3, &d);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(d.x, -1);
    len++;


    //second test: insert in the third from last index (negative index)
    e.x = -2;
    ret = cm_vct_ins(&v, -3, &e);
    ck_assert_int_eq(ret, 0);

    ret = cm_vct_get(&v, -3, &d);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(d.x, -2);
    len++;


    //third test: insert at the end (positive index)
    e.x = -3;
    ret = cm_vct_ins(&v, len, &e);
    ck_assert_int_eq(ret, 0);

    ret = cm_vct_get(&v, len, &d);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(d.x, -3);
    len++;


    //fourth test: insert at the end (negative index)
    e.x = -4;
    ret = cm_vct_ins(&v, -1, &e);
    ck_assert_int_eq(ret, 0);

    ret = cm_vct_get(&v, len, &d);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(d.x, -4);
    len++;


    //fifth test: insert at the beginning (zero index)
    e.x = -5;
    ret = cm_vct_ins(&v, 0, &e);
    ck_assert_int_eq(ret, 0);

    ret = cm_vct_get(&v, 0, &d);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(d.x, -5);
    len++;


    //sixth test: insert at the beginning (max negative index)
    e.x = -6;
    ret = cm_vct_ins(&v, (len * -1) - 1, &e);
    ck_assert_int_eq(ret, 0);

    ret = cm_vct_get(&v, 0, &d);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(d.x, -6);
    len++;
    
    //output the state of the vector after every successful type of insertion
    printf("[test_vct_ins] final values:    ");
    _print_vct();
    printf("[test_vct_ins] expected values: \
-6 -5 0 1 2 -1 3 4 5 6 7 -2 8 9 -3 -4\n");


    //seventh test: insert invalid index (+ve index)
    cm_errno = 0;
    ret = cm_vct_ins(&v, len+1, &e);
    ck_assert_int_eq(ret, -1);
    ck_assert_int_eq(cm_errno, 1100);

    //eighth test: insert invalid index (-ve index)
    cm_errno = 0;
    ret = cm_vct_ins(&v, (len+2) * -1, &e);
    ck_assert_int_eq(ret, -1);
    ck_assert_int_eq(cm_errno, 1100);

    return;

} END_TEST



//cm_vct_rmv() [full fixture]
START_TEST(test_vct_rmv) {

    int ret;
    int len = TEST_LEN_FULL;

    data e;


    //output the starting state of the vector
    printf("[test_vct_rmv] starting values: ");
    _print_vct();

    //first test: remove the third index (positive index)
    ret = cm_vct_rmv(&v, 3);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(v.len, len - 1);
    len--;

    ret = cm_vct_get(&v, 3, &e);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(e.x, 4);


    //second test: remove the third from last index (negative index)
    ret = cm_vct_rmv(&v, -3);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(v.len, len - 1);
    len--;

    ret = cm_vct_get(&v, -3, &e);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(e.x, 6);


    //third test: remove at end (positive index)
    ret = cm_vct_rmv(&v, len - 1);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(v.len, len - 1);
    len--;

    ret = cm_vct_get(&v, len - 1, &e);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(e.x, 8);


    //fourth test: remove at end (negative index)
    ret = cm_vct_rmv(&v, -1);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(v.len, len - 1);
    len--;

    ret = cm_vct_get(&v, len - 1, &e);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(e.x, 6);

    //fifth test: remove at start (zero index)
    ret = cm_vct_rmv(&v, 0);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(v.len, len - 1);
    len--;

    ret = cm_vct_get(&v, 0, &e);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(e.x, 1);

    //sixth test: remove at start (max negative index)
    ret = cm_vct_rmv(&v, (len) * -1);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(v.len, len - 1);
    len--;

    ret = cm_vct_get(&v, 0, &e);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(e.x, 2);


    //seventh test: remove invalid index (+ve index)
    cm_errno = 0;
    ret = cm_vct_rmv(&v, 99);
    ck_assert_int_eq(ret, -1);
    ck_assert_int_eq(cm_errno, 1100);


    //eighth test: remove invalid index (-ve index)
    cm_errno = 0;
    ret = cm_vct_rmv(&v, -99);
    ck_assert_int_eq(ret, -1);
    ck_assert_int_eq(cm_errno, 1100);


    //output the state of the vector after every type of removal
    printf("[test_vct_rmv] final values:    ");
    _print_vct();
    printf("[test_vct_rmv] expected values: \
2 4 5 6\n");

    return;

} END_TEST;



//cm_vct_fit() [empty fixture]
START_TEST(test_vct_fit) {

    int ret;
    int min_len = VECTOR_DEFAULT_SIZE;
    int expand_len = min_len * 4;


    //setup: expand vector to size of min_len * 4
    for (int i = 0; i < expand_len; ++i) {

        d.x = i;
        ret = cm_vct_apd(&v, &d);
        ck_assert_int_eq(ret, 0);

    } //end for
    
    //check size is has grown
    ck_assert_int_eq(v.sz, expand_len);


    //setup (cont): reduce vector length down to VECTOR_DEFAULT_SIZE
    for (int i = 0; i < (expand_len - (min_len)); ++i) {

        ret = cm_vct_rmv(&v, 0);
        ck_assert_int_eq(ret, 0);
    
    } //end for


    //only test: shrink to size
    ret = cm_vct_fit(&v);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(v.sz, min_len);

    return;

} END_TEST;



//cm_vct_emp() [full fixture]
START_TEST(test_vct_emp) {

    /*
     *  Using ASAN to check for leaks here.
     */

    cm_vct_emp(&v);
    ck_assert_int_eq(v.len, 0);

    return;

} END_TEST;



/*
 *  --- [SUITE] ---
 */

Suite * vct_suite() {

    //test cases
    TCase * tc_new_del_vct;
    TCase * tc_vct_apd;
    TCase * tc__grow;
    TCase * tc_vct_get;
    TCase * tc_vct_get_p;
    TCase * tc_vct_set;
    TCase * tc_vct_ins;
    TCase * tc_vct_rmv;
    TCase * tc_vct_fit;
    TCase * tc_vct_emp;

    Suite * s = suite_create("vector");
    

    //cm_new_vct()
    tc_new_del_vct = tcase_create("new_del_vct");
    tcase_add_test(tc_new_del_vct, test_new_del_vct);

    //cm_vct_apd()
    tc_vct_apd = tcase_create("vector_apd");
    tcase_add_checked_fixture(tc_vct_apd, _setup_emp, _teardown);   
    tcase_add_test(tc_vct_apd, test_vct_apd);
    
    //_grow()
    tc__grow = tcase_create("_grow");
    tcase_add_checked_fixture(tc__grow, _setup_emp, _teardown);
    tcase_add_test(tc__grow, test__grow);

    //cm_vct_get()
    tc_vct_get = tcase_create("vector_get");
    tcase_add_checked_fixture(tc_vct_get, _setup_full, _teardown);
    tcase_add_test(tc_vct_get, test_vct_get);

    //cm_vct_get_p()
    tc_vct_get_p = tcase_create("vector_get_p");
    tcase_add_checked_fixture(tc_vct_get_p, _setup_full, _teardown);
    tcase_add_test(tc_vct_get_p, test_vct_get_p);

    //cm_vct_set()
    tc_vct_set = tcase_create("vector_set");
    tcase_add_checked_fixture(tc_vct_set, _setup_full, _teardown);
    tcase_add_test(tc_vct_set, test_vct_set);
  
    //cm_vct_ins()
    tc_vct_ins = tcase_create("vector_ins");
    tcase_add_checked_fixture(tc_vct_ins, _setup_full, _teardown);
    tcase_add_test(tc_vct_ins, test_vct_ins);

    //cm_vct_rmv()
    tc_vct_rmv = tcase_create("vector_rmv");
    tcase_add_checked_fixture(tc_vct_rmv, _setup_full, _teardown);
    tcase_add_test(tc_vct_rmv, test_vct_rmv);

    //cm_vct_fit()
    tc_vct_fit = tcase_create("vector_fit");
    tcase_add_checked_fixture(tc_vct_fit, _setup_emp, _teardown);
    tcase_add_test(tc_vct_fit, test_vct_fit);

    //cm_vct_emp()
    tc_vct_emp = tcase_create("vector_emp");
    tcase_add_checked_fixture(tc_vct_emp, _setup_full, _teardown);
    tcase_add_test(tc_vct_emp, test_vct_emp);


    //add test cases to vector suite
    suite_add_tcase(s, tc_new_del_vct);
    suite_add_tcase(s, tc_vct_apd);
    suite_add_tcase(s, tc__grow);
    suite_add_tcase(s, tc_vct_get);
    suite_add_tcase(s, tc_vct_get_p);
    suite_add_tcase(s, tc_vct_set);
    suite_add_tcase(s, tc_vct_ins);
    suite_add_tcase(s, tc_vct_rmv);
    suite_add_tcase(s, tc_vct_fit);
    suite_add_tcase(s, tc_vct_emp);

    return s;
}
