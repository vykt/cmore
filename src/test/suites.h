#ifndef SUITES_H
#define SUITES_H

//external libraries
#include <check.h>


//unit test suites
Suite * list_suite();
Suite * vector_suite();
Suite * rb_tree_suite();

//other tests
void rb_tree_explore();

#endif
