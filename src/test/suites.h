#ifndef SUITES_H
#define SUITES_H

//external libraries
#include <check.h>


//unit test suites
Suite * lst_suite();
Suite * vct_suite();
Suite * rbt_suite();

//other tests
void rbt_explore();

#endif
