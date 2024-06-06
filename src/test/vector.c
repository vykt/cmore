#include <unistd.h>

#include "../lib/libcmore.h"
#include "test.h"


//run vector test
void test_vector() {
    
    int ret;
    cm_vector vector;
    
    test_data data_0, data_1, data_2, data_3, data_4;
    test_data data_5_val;
    test_data * data_6_ref;

    data_0.w = 0xdead;
    data_0.x = 0xbeef;
    data_0.y = 'd';
    data_0.z = '0';

    data_1.w = 0xba5e;
    data_1.x = 0xba11;
    data_1.y = 'd';
    data_1.z = '1';

    data_2.w = 0xed1f;
    data_2.x = 0x1ce5;
    data_2.y = 'd';
    data_2.z = '2';

    data_3.w = 0x5caf;
    data_3.x = 0xf01d;
    data_3.y = 'd';
    data_3.z = '3';

    data_4.w = 0xdece;
    data_4.x = 0xaced;
    data_4.y = 'd';
    data_4.z = '4';


    //new
    ret = cm_new_vector(&vector, sizeof(data_0));

    //append
    ret = cm_vector_append(&vector, (cm_byte *) &data_0);
    ret = cm_vector_append(&vector, (cm_byte *) &data_1);

    //insert
    ret = cm_vector_insert(&vector, 1, (cm_byte *) &data_2);
    ret = cm_vector_insert(&vector, 3, (cm_byte *) &data_3);
    ret = cm_vector_insert(&vector, -2, (cm_byte *) &data_4);

    //remove
    cm_vector_remove(&vector, 1);  //remove data_1
    cm_vector_remove(&vector, -1); //remove data_4

    //set
    cm_vector_set(&vector, 2, (cm_byte *) &data_1);
    
    //get by value     | value should equal data_1
    ret = cm_vector_get_val(&vector, 2, (cm_byte *) &data_5_val);

    //get by reference | pointer should point to data_0
    data_6_ref = (test_data *) cm_vector_get_ref(&vector, 0);

    //empty
    cm_vector_empty(&vector);

    //check vector reallocation
    for (int i = 0; i < 9; ++i) {
        ret = cm_vector_append(&vector, (cm_byte *) &data_0);
    }

    //delete
    cm_del_vector(&vector);

    return;
}
