//system headers
#include <unistd.h>

//external libraries
#include <check.h>

//local headers
#include "test_data.h"

//remote headers
#include "../lib/cmore.h"
#include "../lib/vector.h"


#define V_GET(v, i) (((data *) v.data) + i)


/*
 *  --- [FIXTURES] ---
 */

//globals
cm_vector v;
data d;


//empty vector setup
void setup_empty() {

    cm_new_vector(&v, sizeof(v));
    d.x = 0;

}

void teardown_empty() {

    cm_del_vector(&v);
    d.x = -1;
}



/*
 *  --- [HELPERS] ---
 */


static int power(int base, const int exp) {

    int result = base;

    for (int i = 1; i < exp; ++i) {
        result = result * base;
    }

    return result;
}


void assert_state(const int len, const size_t size, 
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

} END_TEST



//cm_vector_append()
START_TEST(test_vector_append) {

    int ret;

    //append to empty vector
    ret = cm_vector_append(&v, (cm_byte *) &d);
    ck_assert_int_eq(ret, 0);
    assert_state(1, VECTOR_DEFAULT_SIZE, 0, 0);
    
    d.x++;

    //append to non-empty vector
    ret = cm_vector_append(&v, (cm_byte *) &d);
    ck_assert_int_eq(ret, 0);
    assert_state(2, VECTOR_DEFAULT_SIZE, 1, 1); 

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
        assert_state(i+1, VECTOR_DEFAULT_SIZE 
                          * power(2, (i+1) % VECTOR_DEFAULT_SIZE), i, i);

    } //end for

    return;
}



/*
 *  --- [SUITE] ---
 */

Suite * vector_suite() {

    TCase * tc_new_vector;
    TCase * tc_vector_append;
    TCase * tc__grow;

    Suite * s = suite_create("vector");
    

    //cm_new_vector()
    tc_new_vector = tcase_create("new_vector");
    tcase_add_test(tc_new_vector, test_new_vector);

    //cm_vector_append()
    tc_vector_append = tcase_create("vector_append");
    tcase_add_checked_fixture(tc_vector_append, setup_empty, teardown_empty);   
    tcase_add_test(tc_vector_append, test_vector_append);
    
    //_grow()
    tc__grow = tcase_create("_grow");
    tcase_add_checked_fixture(tc__grow, setup_empty, teardown_empty);
    tcase_add_test(tc__grow, test__grow);

   
    //add test cases to vector suite
    suite_add_tcase(s, tc_new_vector);
    suite_add_tcase(s, tc_vector_append);
    suite_add_tcase(s, tc__grow);

    return s;
}
