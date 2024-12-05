//standard library
#include <stdio.h>

//system headers
#include <unistd.h>
#include <getopt.h>

//external libraries
#include <check.h>

//local headers
#include "suites.h"



enum _test_mode {UNIT, RB_TREE_EXPL};

//determine which tests to run
static enum _test_mode _get_test_mode(int argc, char ** argv) {

    const struct option long_opts[] = {
        {"unit-tests", no_argument, NULL, 'u'},
        {"rb-tree-explore", no_argument, NULL, 'r'},
        {0,0,0,0}
    };

    int opt;
    enum _test_mode test_mode = UNIT;

    
    while((opt = getopt_long(argc, argv, "ur", long_opts, NULL)) != -1 
          && opt != 0) {

        //determine parsed argument
        switch (opt) {

            case 'u':
                test_mode = UNIT;
                break;

            case 'r':
                test_mode = RB_TREE_EXPL;
                break;

        }
    }

    return test_mode;
}



//run unit tests
static void _run_unit_tests() {

    Suite * s_vector;
    Suite * s_list;
    Suite * s_rb_tree;
    Suite * s_error;

    SRunner * sr;

    //initialise test suites
    //s_vector  = vector_suite();
    //s_list    = list_suite();
    s_rb_tree = rb_tree_suite(); 

    //create suite runner
    //sr = srunner_create(s_vector);
    //srunner_add_suite(sr, s_list);
    //srunner_add_suite(sr, s_rb_tree);
    sr = srunner_create(s_rb_tree); //TODO remove

    //run tests
    srunner_run_all(sr, CK_VERBOSE);

    //cleanup
    srunner_free(sr);

    return;
}



//dispatch tests
int main(int argc, char ** argv) {

    enum _test_mode mode = _get_test_mode(argc, argv);

    switch (mode) {

        case UNIT:
            _run_unit_tests();
            break;

        case RB_TREE_EXPL:
            break;
        
    }
    
    return 0;
}
