//external libraries
#include <check.h>

//local headers
#include "suites.h"

//test target headers
#include "../lib/cmore.h"
#include "../lib/alg.h"



/*
 *  [BASIC TEST]
 *
 *     Algorithms are simple; internal functions 
 *     are tested through exported functions.
 */


/*
 *  --- [UNIT TESTS] ---
 */

//cm_clamp() [no fixture]
START_TEST(test_clamp) {

    long ret;

    const long lower = -10;
    const long upper = 20;


    //first test: return an unmodified value
    ret = cm_clamp(10, lower, upper);
    ck_assert_int_eq(ret, 10);

    //second test: return a lower clamped value
    ret = cm_clamp(-15, lower, upper);
    ck_assert_int_eq(ret, lower);

    //third test: return an upper clamped value
    ret = cm_clamp(25, lower, upper);
    ck_assert_int_eq(ret, upper);
    
} END_TEST



/*
 *  --- [SUITE] ---
 */

Suite * alg_suite() {

    //test cases
    TCase * tc_clamp;

    Suite * s = suite_create("algorithm");


    //cm_clamp()
    tc_clamp = tcase_create("clamp");
    tcase_add_test(tc_clamp, test_clamp);


    //add test cases to algorithm suite
    suite_add_tcase(s, tc_clamp);

    return s;
}
