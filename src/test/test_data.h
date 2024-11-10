#ifndef TEST_DATA_H
#define TEST_DATA_H


//test data
#define Z_LEN 7
#define DATA(data, X, Y, Z) data.x = X; data.y = Y; strncpy(data.z, Z, Z_LEN);

typedef struct {

    int  x;
    long y;
    char z[Z_LEN];

} data;


#endif
