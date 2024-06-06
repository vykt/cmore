#include <unistd.h>

#include "../lib/libcmore.h"
#include "test.h"


//run vector test
void test_error() {
    
    int ret;
    char * error_text;

    cm_vector vector;
    test_data data;

    //cause error
    ret = cm_new_vector(&vector, sizeof(data));
    ret = cm_vector_insert(&vector, 2, (cm_byte *) &data);

    //check error functions
    cm_perror();
    error_text = (char *) cm_strerror(cm_errno);
   
    cm_del_vector(&vector);

    return;
}
