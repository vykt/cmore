#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#include "libcmore.h"
#include "vector.h"



/*
 *  --- [INTERNAL] ---
 */

static int _cm_vector_alloc(cm_vector * vector) {

    vector->data = malloc(vector->data_size * vector->size);
    if (!vector->data) {
        cm_errno = CM_ERR_MALLOC;
        return -1;
    }

    return 0;
}



static int _cm_vector_grow(cm_vector * vector) {

    vector->size = vector->size * 2;

    vector->data = realloc(vector->data, vector->data_size * vector->size);
    if (!vector->data) {
        cm_errno = CM_ERR_REALLOC;
        return -1;
    }

    return 0;
}



static inline int _cm_vector_normalise_index(cm_vector * vector, int index) {

    if (index < 0) { 
        index = vector->len + index;
    }

    return index;
}



static cm_byte * _cm_vector_traverse(cm_vector * vector, int index) {

    index = _cm_vector_normalise_index(vector, index);

    return vector->data + (vector->data_size * index);

}



static void _cm_vector_shift(cm_vector * vector, int index, int mode) {

    int diff;

    //calculate how many indeces remain from the given index
    diff = vector->len - index;
    if (diff <= 0) return;

    //calculate how many bytes the remaining indeces constitute
    int move_size = diff * vector->data_size;

    cm_byte * data = _cm_vector_traverse(vector, index);
    memmove(data + (ssize_t) (vector->data_size * mode), data, move_size);

    return;
}



static inline void _cm_vector_set(cm_vector * vector, int index, cm_byte * data) {

    cm_byte * index_data = _cm_vector_traverse(vector, index);
    memcpy(index_data, data, vector->data_size);

    return;
}



static inline int _cm_vector_assert_index_range(cm_vector * vector, 
                                                int index, int mode) {

    /*
     *  if inserting, maximum index needs to be +1 higher than for other operations
     */

    if (abs(index) >= (vector->len + mode)) {
        cm_errno = CM_ERR_USER_INDEX;
        return -1;
    }

    return 0;
}



/*
 *  --- [EXTERNAL] ---
 */

int cm_vector_get_val(cm_vector * vector, int index, cm_byte * buf) {

    if (_cm_vector_assert_index_range(vector, index, VECTOR_INDEX)) return -1;
    
    cm_byte * data = _cm_vector_traverse(vector, index); 
    memcpy(buf, data, vector->data_size);

    return -1;
}



cm_byte * cm_vector_get_ref(cm_vector * vector, int index) {

    if (_cm_vector_assert_index_range(vector, index, VECTOR_INDEX)) return NULL;
    
    cm_byte * data = _cm_vector_traverse(vector, index); 
    return data;
}



int cm_vector_set(cm_vector * vector, int index, cm_byte * data) {

    if (_cm_vector_assert_index_range(vector, index, VECTOR_INDEX)) return -1;

    //convert a negative index to a positive equivalent
    index = _cm_vector_normalise_index(vector, index);

    _cm_vector_set(vector, index, data);

    return 0;
}



int cm_vector_insert(cm_vector * vector, int index, cm_byte * data) {

    if (_cm_vector_assert_index_range(vector, index, VECTOR_ADD_INDEX)) return -1;

    //grow the vector if there is no space left to insert new elements
    if ((size_t) vector->len == vector->size) {
        if(_cm_vector_grow(vector)) return -1;
    }

    //convert a negative index to a positive equivalent
    index = _cm_vector_normalise_index(vector, index);

    _cm_vector_shift(vector, index, VECTOR_SHIFT_UP);
    _cm_vector_set(vector, index, data);
    ++vector->len;

    return 0;
}



int cm_vector_append(cm_vector * vector, cm_byte * data) {
 
    //grow the vector if there is no space left to insert new elements
    if ((size_t) vector->len == vector->size) {
        if(_cm_vector_grow(vector)) return -1;
    }

    _cm_vector_set(vector, vector->len, data);
    ++vector->len;
    return 0;
}



int cm_vector_remove(cm_vector * vector, int index) {

    if (_cm_vector_assert_index_range(vector, index, VECTOR_INDEX)) return -1;

    //convert a negative index to a positive equivalent
    index = _cm_vector_normalise_index(vector, index);

    _cm_vector_shift(vector, index + 1, VECTOR_SHIFT_DOWN);
    --vector->len;

    return 0;
}



void cm_vector_empty(cm_vector * vector) {

    vector->len = 0;

    return;
}



int cm_new_vector(cm_vector * vector, size_t data_size) {

    vector->len = 0;
    vector->size = VECTOR_DEFAULT_SIZE;
    vector->data_size = data_size;
    
    if (_cm_vector_alloc(vector)) return -1;
    
    return 0;
}



void cm_del_vector(cm_vector * vector) {

    free(vector->data);
}
