//standard library
#include <stdint.h>

//external libraries
#include <check.h>

//local headers
#include "suites.h"

//test target headers
#include "../lib/cmore.h"
#include "../lib/func.h"



/*
 *  [BASIC TEST]
 *
 *     Functional tools are simple; internal functions 
 *     are tested through exported functions.
 */



//globals
static cm_meta_type t;

enum type_table {
    TYPE_A=1,
    TYPE_B=2
};
void * void_ctx = (void *) 0x1337; 

static cm_monad m;



/*
 *  --- [HELPERS] ---
 */

static void _assert_meta_type(const cm_meta_type * value,
                              const void * data, const size_t sz,
                              const int type_id, const bool is_init) {

    ck_assert_int_eq(*(int *) value->data, *(int *) data);
    ck_assert_int_eq(value->sz, sz);
    ck_assert_int_eq(value->type_id, type_id);
    ck_assert_int_eq(value->is_init, is_init);

    return;
}



static cm_meta_type * _add_one(cm_meta_type * value, void * ctx) {

    int * cast_value = (int *) (value->data);
    ++*cast_value;
    ck_assert_ptr_eq(ctx, void_ctx); /* use ctx to suppress warning */

    return value;
}



static cm_meta_type * _set_type_b(cm_meta_type * value, void * ctx) {

    value->type_id = TYPE_B;
    int * cast_value = (int *) (value->data);
    *cast_value = -1;
    ck_assert_ptr_eq(ctx, void_ctx); /* use ctx to suppress warning */

    return value;
}



static cm_meta_type * _set_type_fail(cm_meta_type * value, void * ctx) {

    value->type_id = CM_MONAD_FAIL_TYPE;
    ck_assert_ptr_eq(ctx, void_ctx); /* use ctx to suppress warning */

    return value;
}



/*
 *  --- [FIXTURES] ---
 */

//'meta_type' fixture
static void _setup_meta_type() {

    int ret;
    const int primitive_data = 0;


    ret = cm_new_meta_type(&t, TYPE_A,
                           &primitive_data, sizeof(primitive_data));

    return;
}


//'monad' fixture
static void _setup_monad() {

    int ret;
    const int primitive_data = 0;


    ret = cm_new_meta_type(&t, TYPE_A,
                           &primitive_data, sizeof(primitive_data));
    cm_new_monad(&m);
    
    return;
}



static void _teardown_meta_type() {

    cm_del_meta_type(&t);

    return;
}



static void _teardown_monad() {

    cm_del_monad(&m);
    cm_del_meta_type(&t);

    return;
}



/*
 *  --- [UNIT TESTS] ---
 */

//cm_new_meta_type() & cm_del_meta_type() [no fixture]
START_TEST(test_new_del_meta_type) {

    int ret;
    const int primitive_value = 0;

    //only test: create a new meta type & destroy it
    ret = cm_new_meta_type(&t, (int) TYPE_A,
                               &primitive_value, sizeof(primitive_value));
    ck_assert_int_ne(ret, -1);
    _assert_meta_type(&t, &primitive_value,
                      sizeof(primitive_value), (int) TYPE_A, true);

    cm_del_meta_type(&t);
    ck_assert_int_eq(t.is_init, false);

    return;

} END_TEST



//cm_meta_type_set() [meta type fixture]
START_TEST(test_meta_type_set) {

    const int primitive_value = 8086;

    //only test: set a new value
    cm_meta_type_set(&t, &primitive_value);
    _assert_meta_type(&t, &primitive_value,
                      sizeof(primitive_value), (int) TYPE_A, true);

    return;

} END_TEST



//cm_meta_type_upd()
START_TEST(test_meta_type_upd) {

    int ret;
    const uint16_t primitive_value = -1;

    //only test: update to a new type
    ret = cm_meta_type_upd(&t, TYPE_B,
                           &primitive_value, sizeof(primitive_value));
    ck_assert_int_eq(ret, 0);
    _assert_meta_type(&t, &primitive_value,
                      sizeof(primitive_value), (int) TYPE_B, true);

    return;

} END_TEST



//cm_meta_type_cpy() [meta type fixture]
START_TEST(test_meta_type_cpy) {

    int ret;
    cm_meta_type u;
    int primitive_value = 0;

    //only test: copy meta type value t into u
    ret = cm_meta_type_cpy(&u, &t);
    ck_assert_int_eq(ret, 0);
    cm_del_meta_type(&u);

    return;

} END_TEST



//cm_new_del_monad() [no fixture]
START_TEST(test_new_del_monad) {

    //only test: create a new monad & destroy it
    cm_new_monad(&m);
    ck_assert_int_eq(m.is_init, true);

    cm_del_monad(&m);
    ck_assert_int_eq(m.is_init, false);

    return;

} END_TEST



//cm_monad_compose [monad fixture]
START_TEST(test_monad_compose) {

    int ret;
    

    //only test: compose the monad with several callbacks
    ret = cm_monad_compose(&m, _add_one);
    ck_assert_int_eq(ret, 0);

    ret = cm_monad_compose(&m, _set_type_b);
    ck_assert_int_eq(ret, 0);

    ret = cm_monad_compose(&m, _set_type_fail);
    ck_assert_int_eq(ret, 0);
    
    ck_assert_int_eq(m.thunk.len, 3);
    return;

} END_TEST



//cm_monad_eval [monad fixture]
START_TEST(test_monad_eval) {

    int ret;


    //first test: evaluate only the `add_one` callback
    ret = cm_monad_compose(&m, _add_one);
    ck_assert_int_eq(ret, 0);

    ret = cm_monad_eval(&m, &t, void_ctx);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(t.type_id, TYPE_A);
    ck_assert_int_eq(*(int *) t.data, 1);

    //second test: evaluate both the `add_one` and `set_type_b` callbacks
    *(int *) t.data = 0;
    ret = cm_monad_compose(&m, _set_type_b);
    ck_assert_int_eq(ret, 0);

    ret = cm_monad_eval(&m, &t, void_ctx);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(t.type_id, TYPE_B);
    ck_assert_int_eq(*(int *) t.data, -1);

    //third test: evaluate all `add_one`, `set_type_b`, `set_type_fail`
    *(int *) t.data = 0;
    ret = cm_monad_compose(&m, _set_type_fail);
    ck_assert_int_eq(ret, 0);

    ret = cm_monad_eval(&m, &t, void_ctx);
    ck_assert_int_eq(ret, -1);
    ck_assert_int_eq(t.type_id, CM_MONAD_FAIL_TYPE);
    ck_assert_int_eq(*(int *) t.data, -1);

    return;

} END_TEST



/*
 *  --- [SUITE] ---
 */

Suite * func_suite() {

    //test cases
    TCase * tc_new_del_meta_type;
    TCase * tc_meta_type_set;
    TCase * tc_meta_type_upd;
    TCase * tc_meta_type_cpy;
    
    TCase * tc_new_del_monad;
    TCase * tc_monad_compose;
    TCase * tc_monad_eval;

    Suite * s = suite_create("functional");


    //cm_new_meta_type()
    tc_new_del_meta_type = tcase_create("new_del_meta_type");
    tcase_add_test(tc_new_del_meta_type, test_new_del_meta_type);

    //cm_meta_type_set()
    tc_meta_type_set = tcase_create("meta_type_set");
    tcase_add_checked_fixture(tc_meta_type_set,
                              _setup_meta_type, _teardown_meta_type);
    tcase_add_test(tc_meta_type_set, test_meta_type_set);

    //cm_meta_type_upd()
    tc_meta_type_upd = tcase_create("meta_type_upd");
    tcase_add_checked_fixture(tc_meta_type_upd,
                              _setup_meta_type, _teardown_meta_type);
    tcase_add_test(tc_meta_type_upd, test_meta_type_set);

    //cm_meta_type_cpy()
    tc_meta_type_cpy = tcase_create("meta_type_cpy");
    tcase_add_checked_fixture(tc_meta_type_cpy,
                              _setup_meta_type, _teardown_meta_type);
    tcase_add_test(tc_meta_type_cpy, test_meta_type_cpy);


    //cm_new_monad()
    tc_new_del_monad = tcase_create("new_del_monad");
    tcase_add_test(tc_new_del_monad, test_new_del_monad);

    //cm_monad_compose()
    tc_monad_compose = tcase_create("monad_compose");
    tcase_add_checked_fixture(tc_monad_compose,
                              _setup_monad, _teardown_monad);
    tcase_add_test(tc_monad_compose, test_monad_compose);

    //cm_monad_eval()
    tc_monad_eval = tcase_create("monad_eval");
    tcase_add_checked_fixture(tc_monad_eval,
                              _setup_monad, _teardown_monad);
    tcase_add_test(tc_monad_eval, test_monad_eval);


    //add test cases to functional suite
    suite_add_tcase(s, tc_new_del_meta_type);
    suite_add_tcase(s, tc_meta_type_set);
    suite_add_tcase(s, tc_meta_type_upd);
    suite_add_tcase(s, tc_meta_type_cpy);

    suite_add_tcase(s, tc_new_del_monad);
    suite_add_tcase(s, tc_monad_compose);
    suite_add_tcase(s, tc_monad_eval);

    return s;
}
