#include <unistd.h>

#include "../lib/libcmore.h"
#include "test.h"


//run list test
void test_list() {
    
    int ret;
    cm_list list;
    
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
    cm_new_list(&list, sizeof(data_0));

    //append
    ret = cm_list_append(&list, (cm_byte *) &data_0);
    ret = cm_list_append(&list, (cm_byte *) &data_1);

    //insert
    ret = cm_list_insert(&list, 1, (cm_byte *) &data_2);
    ret = cm_list_insert(&list, 3, (cm_byte *) &data_3);
    ret = cm_list_insert(&list, -2, (cm_byte *) &data_4);

    //remove
    cm_list_remove(&list, 1);  //remove data_1
    cm_list_remove(&list, -1); //remove data_4

    //set
    cm_list_set(&list, 2, (cm_byte *) &data_1);
    
    //get by value     | value should equal data_1
    ret = cm_list_get_val(&list, 2, (cm_byte *) &data_5_val);

    //get by reference | pointer should point to data_0
    data_6_ref = (test_data *) cm_list_get_ref(&list, 0);

    //empty
    cm_list_empty(&list);

    //delete
    cm_del_list(&list);

    return;
}
