//external libraries
#include <check.h>

//local headers
#include "suites.h"


/*
 *  Run unit tests.
 */

int main() {

    Suite * s_vector;
    Suite * s_list;
    Suite * s_rb_tree;
    Suite * s_error;

    SRunner * sr;

    //initialise test suites
    s_vector = vector_suite();
    s_list = list_suite();

    //create suite runner
    sr = srunner_create(s_vector);
    srunner_add_suite(sr, s_list);

    //run tests
    srunner_run_all(sr, CK_VERBOSE);
    
    //cleanup
    srunner_free(sr);

    return 0;
}
