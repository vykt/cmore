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
#include "../lib/vector.h"


#define V_GET(v, i) (((data *) v.data) + i)


/*
 *  [BASIC TEST]
 *
 *     Vectors are simple; internal functions are tested through exported functions.
 */


/*
 *  --- [FIXTURES] ---
 */

//globals
static cm_vector v;
static data d;



//empty vector setup
static void _setup_empty() {

    cm_new_vector(&v, sizeof(d));
    d.x = 0;

    return;
}



//false populated vector setup
static void _setup_facade() {

    v.len = 10;
    v.size = VECTOR_DEFAULT_SIZE;
    v.data_size = sizeof(d);
    v.data = malloc(v.data_size * v.size);

    return;
}



//populated vector setup
#define TEST_LEN_FULL 10
static void _setup_full() {

    cm_new_vector(&v, sizeof(d));
    d.x = 0;

    for (int i = 0; i < TEST_LEN_FULL; ++i) {
        cm_vector_append(&v, (cm_byte *) &d);
        d.x++;
    }

    return;
}



static void _teardown() {

    cm_del_vector(&v);
    d.x = -1;

    return;
}



/*
 *  --- [HELPERS] ---
 */

static void _print_vector() {

    data e;

    //for each entry
    for (int i = 0; i < v.len; ++i) {

        cm_vector_get_val(&v, i, (cm_byte *) &e);
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



static void _assert_state(const int len, const size_t size, 
                          const int index, const int value) {

    ck_assert_int_eq(v.len, len);
    ck_assert_int_eq(v.size, size);
    ck_assert_int_eq(V_GET(v, index)->x, value);
}


/*
 *  --- [UNIT TESTS] ---
 */

//cm_new_vector()
START_TEST(test_new_vector) {

    //run test
    int ret = cm_new_vector(&v, sizeof(data));
    
    //assert result
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(v.len, 0);
    ck_assert_int_eq(v.size, VECTOR_DEFAULT_SIZE);
    ck_assert_int_eq(v.data_size, sizeof(data));

    //cleanup
    cm_del_vector(&v);

    return;

} END_TEST



//cm_del_vector()
START_TEST(test_del_vector) {

    //run test
    cm_del_vector(&v);

    return;

} END_TEST



//cm_vector_append()
START_TEST(test_vector_append) {

    int ret;

    //append to empty vector
    ret = cm_vector_append(&v, (cm_byte *) &d);
    ck_assert_int_eq(ret, 0);
    _assert_state(1, VECTOR_DEFAULT_SIZE, 0, 0);
    
    d.x++;

    //append to non-empty vector
    ret = cm_vector_append(&v, (cm_byte *) &d);
    ck_assert_int_eq(ret, 0);
    _assert_state(2, VECTOR_DEFAULT_SIZE, 1, 1); 

} END_TEST



//_grow()
START_TEST(test__grow) {

    int ret;

    //append repeatedly to grow allocated area
    for (int i = 0; i < (VECTOR_DEFAULT_SIZE * 2) + 1; ++i) {

        ret = cm_vector_append(&v, (cm_byte *) &d);
        d.x++;

        //assert state
        ck_assert_int_eq(ret, 0);
        _assert_state(i+1, VECTOR_DEFAULT_SIZE 
                           * _power(2, (i / VECTOR_DEFAULT_SIZE)), i, i);

    } //end for

    return;
}



//cm_vector_get_val()
START_TEST(test_vector_get_val) {

    int ret;
    
    data d;

    //get every vector entry by value (positive index)
    for (int i = 0; i < TEST_LEN_FULL; ++i) {

        ret = cm_vector_get_val(&v, i, (cm_byte *) &d);
        ck_assert_int_eq(ret, 0);
        ck_assert_int_eq(d.x, i);

    } //end for

    //get every vector entry by value (negative index)
    for (int i = -1; i > TEST_LEN_FULL * -1; --i) {

        ret = cm_vector_get_val(&v, i, (cm_byte *) &d);
        ck_assert_int_eq(ret, 0);
        ck_assert_int_eq(d.x, TEST_LEN_FULL + i);

    } //end for

    //get invalid index (+ve index)
    cm_errno = 0;
    ret = cm_vector_get_val(&v, TEST_LEN_FULL, (cm_byte *) &d);
    ck_assert_int_eq(ret, -1);
    ck_assert_int_eq(cm_errno, CM_ERR_USER_INDEX);

    //get invalid index (-ve index)
    cm_errno = 0;
    ret = cm_vector_get_val(&v, -TEST_LEN_FULL - 1, (cm_byte *) &d);
    ck_assert_int_eq(ret, -1);
    ck_assert_int_eq(cm_errno, CM_ERR_USER_INDEX);

    return;
    
} END_TEST



//cm_vector_get_ref()
START_TEST(test_vector_get_ref) {

    data * p;

    //get every vector entry by address (positive index)
    for (int i = 0; i < TEST_LEN_FULL; ++i) {

        p = (data *) cm_vector_get_ref(&v, i);
        ck_assert_ptr_nonnull(p);
        ck_assert_int_eq(p->x, i);
    
    } //end for

    //get every vector entry by address (negative index)
    for (int i = -1; i > TEST_LEN_FULL * -1; --i) {

        p = (data *) cm_vector_get_ref(&v, i);
        ck_assert_ptr_nonnull(p);
        ck_assert_int_eq(p->x, TEST_LEN_FULL + i);

    } //end for

    //get ref to invalid index (+ve index)
    cm_errno = 0;
    p = (data *) cm_vector_get_ref(&v, TEST_LEN_FULL);
    ck_assert_ptr_null(p);
    ck_assert_int_eq(cm_errno, CM_ERR_USER_INDEX);

    //get ref to invalid index (-ve index)
    cm_errno = 0;
    p = (data *) cm_vector_get_ref(&v, -TEST_LEN_FULL - 1);
    ck_assert_ptr_null(p);
    ck_assert_int_eq(cm_errno, CM_ERR_USER_INDEX);

    return;

} END_TEST



//cm_vector_set()
START_TEST(test_vector_set) {

    int ret;
    
    data e;


    //starting state of the vector
    printf("[test_vector_set] starting values:             ");
    _print_vector();

    //set every vector entry (positive index)
    for (int i = 0; i < TEST_LEN_FULL; ++i) {

        e.x = i * -1;
        ret = cm_vector_set(&v, i, (cm_byte *) &e);
        ck_assert_int_eq(ret, 0);
        
        ret = cm_vector_get_val(&v, i, (cm_byte *) &d);
        ck_assert_int_eq(ret, 0);
        ck_assert_int_eq(d.x, i * -1);

    } //end for


    //state of the vector after setting all values with a +ve index
    printf("[test_vector_set] final values (+ve index):    ");
    _print_vector();
    printf("[test_vector_set] expected values (+ve index): \
0 -1 -2 -3 -4 -5 -6 -7 -8 -9\n");


    //set every vector entry (negative index)
    for (int i = TEST_LEN_FULL * -1; i < 0; ++i) {

        //value of e.x will range from -11 to -19 (skip index 0)
        e.x = -TEST_LEN_FULL - (TEST_LEN_FULL + i);
        ret = cm_vector_set(&v, i, (cm_byte *) &e);
        ck_assert_int_eq(ret, 0);

        ret = cm_vector_get_val(&v, i, (cm_byte *) &d);
        ck_assert_int_eq(ret, 0);
        ck_assert_int_eq(d.x, -TEST_LEN_FULL - (TEST_LEN_FULL + i));

    } //end for


    //state of the vector after setting all values with a -ve index
    printf("[test_vector_set] final values (-ve index):    ");
    _print_vector();
    printf("[test_vector_set] expected values (-ve index): \
-10 -11 -12 -13 -14 -15 -16 -17 -18 -19\n");


    //set invalid index (+ve index)
    cm_errno = 0;
    ret = cm_vector_set(&v, TEST_LEN_FULL, (cm_byte *) &d);
    ck_assert_int_eq(ret, -1);
    ck_assert_int_eq(cm_errno, 1100);

    //set invalid index (-ve index)
    cm_errno = 0;
    ret = cm_vector_set(&v, -TEST_LEN_FULL - 1, (cm_byte *) &d);
    ck_assert_int_eq(ret, -1);
    ck_assert_int_eq(cm_errno, 1100);

    return;

} END_TEST



//cm_vector_insert()
START_TEST(test_vector_insert) {

    int ret;
    int len = TEST_LEN_FULL;
    
    data e;


    //starting state of the vector
    printf("[test_vector_insert] starting values: ");
    _print_vector();

    //insert in the third index (positive index)
    e.x = -1;
    ret = cm_vector_insert(&v, 3, (cm_byte *) &e);
    ck_assert_int_eq(ret, 0);

    ret = cm_vector_get_val(&v, 3, (cm_byte *) &d);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(d.x, -1);
    len++;

    //insert in the third from last index (negative index)
    e.x = -2;
    ret = cm_vector_insert(&v, -3, (cm_byte *) &e);
    ck_assert_int_eq(ret, 0);

    ret = cm_vector_get_val(&v, -3, (cm_byte *) &d);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(d.x, -2);
    len++;

    //insert at the end (positive index)
    e.x = -3;
    ret = cm_vector_insert(&v, len, (cm_byte *) &e);
    ck_assert_int_eq(ret, 0);

    ret = cm_vector_get_val(&v, len, (cm_byte *) &d);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(d.x, -3);
    len++;

    //insert at the end (negative index)
    e.x = -4;
    ret = cm_vector_insert(&v, -1, (cm_byte *) &e);
    ck_assert_int_eq(ret, 0);

    ret = cm_vector_get_val(&v, len, (cm_byte *) &d);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(d.x, -4);
    len++;

    //insert at the beginning (zero index)
    e.x = -5;
    ret = cm_vector_insert(&v, 0, (cm_byte *) &e);
    ck_assert_int_eq(ret, 0);

    ret = cm_vector_get_val(&v, 0, (cm_byte *) &d);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(d.x, -5);
    len++;

    //insert at the beginning (max negative index)
    e.x = -6;
    ret = cm_vector_insert(&v, (len * -1) - 1, (cm_byte *) &e);
    ck_assert_int_eq(ret, 0);

    ret = cm_vector_get_val(&v, 0, (cm_byte *) &d);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(d.x, -6);
    len++;
    
    //state of the vector after every type of insertion
    printf("[test_vector_insert] final values:    ");
    _print_vector();
    printf("[test_vector_insert] expected values: \
-6 -5 0 1 2 -1 3 4 5 6 7 -2 8 9 -3 -4\n");


    //insert invalid index (+ve index)
    cm_errno = 0;
    ret = cm_vector_insert(&v, len+1, (cm_byte *) &e);
    ck_assert_int_eq(ret, -1);
    ck_assert_int_eq(cm_errno, 1100);

    //insert invalid index (-ve index)
    cm_errno = 0;
    ret = cm_vector_insert(&v, (len+2) * -1, (cm_byte *) &e);
    ck_assert_int_eq(ret, -1);
    ck_assert_int_eq(cm_errno, 1100);

    return;

} END_TEST



//cm_vector_remove()
START_TEST(test_vector_remove) {

    /*
     *  TODO This test depends on the _setup_full() initialisation. If it 
     *       is changed, the values this test compares against must be 
     *       adjusted.
     */

    int ret;
    int len = TEST_LEN_FULL;

    data e;


    //starting state of the vector
    printf("[test_vector_remove] starting values: ");
    _print_vector();

    //remove the third index (positive index)
    ret = cm_vector_remove(&v, 3);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(v.len, len - 1);
    len--;

    ret = cm_vector_get_val(&v, 3, (cm_byte *) &e);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(e.x, 4);

    //remove the third from last index (negative index)
    ret = cm_vector_remove(&v, -3);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(v.len, len - 1);
    len--;

    ret = cm_vector_get_val(&v, -3, (cm_byte *) &e);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(e.x, 6);

    //remove at end (positive index)
    ret = cm_vector_remove(&v, len - 1);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(v.len, len - 1);
    len--;

    ret = cm_vector_get_val(&v, len - 1, (cm_byte *) &e);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(e.x, 8);

    //remove at end (negative index)
    ret = cm_vector_remove(&v, -1);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(v.len, len - 1);
    len--;

    ret = cm_vector_get_val(&v, len - 1, (cm_byte *) &e);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(e.x, 6);

    //remove at start (zero index)
    ret = cm_vector_remove(&v, 0);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(v.len, len - 1);
    len--;

    ret = cm_vector_get_val(&v, 0, (cm_byte *) &e);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(e.x, 1);

    //remove at start (max negative index)
    ret = cm_vector_remove(&v, (len) * -1);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(v.len, len - 1);
    len--;

    ret = cm_vector_get_val(&v, 0, (cm_byte *) &e);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(e.x, 2);


    //remove invalid index (+ve index)
    cm_errno = 0;
    ret = cm_vector_remove(&v, 99);
    ck_assert_int_eq(ret, -1);
    ck_assert_int_eq(cm_errno, 1100);

    //remove invalid index (-ve index)
    cm_errno = 0;
    ret = cm_vector_remove(&v, -99);
    ck_assert_int_eq(ret, -1);
    ck_assert_int_eq(cm_errno, 1100);


    //state of the vector after every type of removal
    printf("[test_vector_remove] final values:    ");
    _print_vector();
    printf("[test_vector_remove] expected values: \
2 4 5 6\n");

    return;

} END_TEST;



//cm_vector_shrink_to_fit()
START_TEST(test_vector_shrink_to_fit) {

    int ret;
    int min_len = VECTOR_DEFAULT_SIZE;
    int expand_len = min_len * 4;

    //expand vector to size of min_len * 4 (realloc for more memory twice)
    for (int i = 0; i < expand_len; ++i) {

        d.x = i;
        ret = cm_vector_append(&v, (cm_byte *) &d);
        ck_assert_int_eq(ret, 0);

    } //end for
    
    //check size is has grown thrice
    ck_assert_int_eq(v.size, expand_len);

    //reduce vector length down to VECTOR_DEFAULT_SIZE
    for (int i = 0; i < (expand_len - (min_len)); ++i) {

        ret = cm_vector_remove(&v, 0);
        ck_assert_int_eq(ret, 0);
    
    } //end for

    //shrink to size
    ret = cm_vector_shrink_to_fit(&v);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(v.size, min_len);

    return;

} END_TEST;



//cm_vector_empty()
START_TEST(test_vector_empty) {

    /*
     *  Using ASAN to check for leaks here.
     */

    cm_vector_empty(&v);
    ck_assert_int_eq(v.len, 0);

    return;

} END_TEST;



/*
 *  --- [SUITE] ---
 */

Suite * vector_suite() {

    //test cases
    TCase * tc_new_vector;
    TCase * tc_del_vector;
    TCase * tc_vector_append;
    TCase * tc__grow;
    TCase * tc_vector_get_val;
    TCase * tc_vector_get_ref;
    TCase * tc_vector_set;
    TCase * tc_vector_insert;
    TCase * tc_vector_remove;
    TCase * tc_vector_shrink_to_fit;
    TCase * tc_vector_empty;

    Suite * s = suite_create("vector");
    

    //cm_new_vector()
    tc_new_vector = tcase_create("new_vector");
    tcase_add_test(tc_new_vector, test_new_vector);

    //cm_del_vector()
    tc_del_vector = tcase_create("del_vector");
    tcase_add_checked_fixture(tc_del_vector, _setup_facade, NULL);
    tcase_add_test(tc_del_vector, test_del_vector);

    //cm_vector_append()
    tc_vector_append = tcase_create("vector_append");
    tcase_add_checked_fixture(tc_vector_append, _setup_empty, _teardown);   
    tcase_add_test(tc_vector_append, test_vector_append);
    
    //_grow()
    tc__grow = tcase_create("_grow");
    tcase_add_checked_fixture(tc__grow, _setup_empty, _teardown);
    tcase_add_test(tc__grow, test__grow);

    //cm_vector_get_val()
    tc_vector_get_val = tcase_create("vector_get_val");
    tcase_add_checked_fixture(tc_vector_get_val, _setup_full, _teardown);
    tcase_add_test(tc_vector_get_val, test_vector_get_val);

    //cm_vector_get_ref()
    tc_vector_get_ref = tcase_create("vector_get_ref");
    tcase_add_checked_fixture(tc_vector_get_ref, _setup_full, _teardown);
    tcase_add_test(tc_vector_get_ref, test_vector_get_ref);

    //cm_vector_set()
    tc_vector_set = tcase_create("vector_set");
    tcase_add_checked_fixture(tc_vector_set, _setup_full, _teardown);
    tcase_add_test(tc_vector_set, test_vector_set);
  
    //cm_vector_insert()
    tc_vector_insert = tcase_create("vector_insert");
    tcase_add_checked_fixture(tc_vector_insert, _setup_full, _teardown);
    tcase_add_test(tc_vector_insert, test_vector_insert);

    //cm_vector_remove()
    tc_vector_remove = tcase_create("vector_remove");
    tcase_add_checked_fixture(tc_vector_remove, _setup_full, _teardown);
    tcase_add_test(tc_vector_remove, test_vector_remove);

    //cm_vector_shrink_to_fit()
    tc_vector_shrink_to_fit = tcase_create("vector_shrink_to_fit");
    tcase_add_checked_fixture(tc_vector_shrink_to_fit, _setup_empty, _teardown);
    tcase_add_test(tc_vector_shrink_to_fit, test_vector_shrink_to_fit);

    //cm_vector_empty()
    tc_vector_empty = tcase_create("vector_empty");
    tcase_add_checked_fixture(tc_vector_empty, _setup_full, _teardown);
    tcase_add_test(tc_vector_empty, test_vector_empty);


    //add test cases to vector suite
    suite_add_tcase(s, tc_new_vector);
    suite_add_tcase(s, tc_del_vector);
    suite_add_tcase(s, tc_vector_append);
    suite_add_tcase(s, tc__grow);
    suite_add_tcase(s, tc_vector_get_val);
    suite_add_tcase(s, tc_vector_get_ref);
    suite_add_tcase(s, tc_vector_set);
    suite_add_tcase(s, tc_vector_insert);
    suite_add_tcase(s, tc_vector_remove);
    suite_add_tcase(s, tc_vector_shrink_to_fit);
    suite_add_tcase(s, tc_vector_empty);

    return s;
}
