#ifndef TEST_DATA_H
#define TEST_DATA_H



/*
 *  While the test suite originally started out with the intention of 
 *  permitting a variety of test data to be used, many tests and test
 *  tooling now rely on the data being the struct below. Modifying it 
 *  *will* break all test suites.
 */



//test data
typedef struct {

    int  x;

} data;

#endif
