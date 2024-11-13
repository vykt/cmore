#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#include "cmore.h"
#include "vector.h"



/*
 *  --- [INTERNAL] ---
 */

static int _alloc(cm_vector * vector) {

    vector->data = malloc(vector->data_size * vector->size);
    if (!vector->data) {
        cm_errno = CM_ERR_MALLOC;
        return -1;
    }

    return 0;
}



static int _grow(cm_vector * vector) {

    vector->size = vector->size * 2;

    vector->data = realloc(vector->data, vector->data_size * vector->size);
    if (!vector->data) {
        cm_errno = CM_ERR_REALLOC;
        return -1;
    }

    return 0;
}



static inline int _normalise_index(const cm_vector * vector, 
                                   int index, const enum _index_mode mode) {

    //if negative index supplied
    if (index < 0) {

        index = vector->len + index;
        if (mode == ADD_INDEX) index++;
    }

    return index;
}



static cm_byte * _traverse(const cm_vector * vector, const int index) {

    return vector->data + (vector->data_size * index);
}



static void _shift(cm_vector * vector,
                   const int index, const enum _shift_mode mode) {

    int diff;

    //calculate how many indeces remain from the given index
    diff = vector->len - index;
    if (diff <= 0) return;

    //calculate how many bytes the remaining indeces constitute
    int move_size = diff * vector->data_size;

    cm_byte * data = _traverse(vector, index);
    memmove(data + (ssize_t) (vector->data_size * mode), data, move_size);

    return;
}



static inline void _set(cm_vector * vector, 
                        const int index, const cm_byte * data) {

    cm_byte * index_data = _traverse(vector, index);
    memcpy(index_data, data, vector->data_size);

    return;
}



static inline int _assert_index_range(const cm_vector * vector, 
                                      const int index, const enum _index_mode mode) {

    /*
     *  if inserting, maximum index needs to be +1 higher than for other operations
     */

    //check for < 0 to range-check normalised negative indeces
    if (index >= (vector->len + (int) mode) || index < 0) {
        cm_errno = CM_ERR_USER_INDEX;
        return -1;
    }

    return 0;
}



/*
 *  --- [EXTERNAL] ---
 */

int cm_vector_get_val(const cm_vector * vector, const int index, cm_byte * buf) {

    int norm_index = _normalise_index(vector, index, INDEX);
    if (_assert_index_range(vector, norm_index, INDEX)) return -1;
    
    cm_byte * data = _traverse(vector, norm_index); 
    memcpy(buf, data, vector->data_size);

    return 0;
}



cm_byte * cm_vector_get_ref(const cm_vector * vector, const int index) {

    int norm_index = _normalise_index(vector, index, INDEX);
    if (_assert_index_range(vector, norm_index, INDEX)) return NULL;
    
    cm_byte * data = _traverse(vector, norm_index); 
    return data;
}



int cm_vector_set(cm_vector * vector, const int index, const cm_byte * data) {

    int norm_index = _normalise_index(vector, index, INDEX);
    if (_assert_index_range(vector, norm_index, INDEX)) return -1;

    _set(vector, norm_index, data);

    return 0;
}



int cm_vector_insert(cm_vector * vector, const int index, const cm_byte * data) {

    int norm_index = _normalise_index(vector, index, ADD_INDEX);
    if (_assert_index_range(vector, norm_index, ADD_INDEX)) return -1;

    //grow the vector if there is no space left to insert new elements
    if ((size_t) vector->len == vector->size) {
        if(_grow(vector)) return -1;
    }

    _shift(vector, norm_index, SHIFT_UP);
    _set(vector, norm_index, data);
    ++vector->len;

    return 0;
}



int cm_vector_append(cm_vector * vector, const cm_byte * data) {
 
    //grow the vector if there is no space left to insert new elements
    if ((size_t) vector->len == vector->size) {
        if(_grow(vector)) return -1;
    }

    _set(vector, vector->len, data);
    ++vector->len;
    return 0;
}



int cm_vector_remove(cm_vector * vector, const int index) {

    int norm_index = _normalise_index(vector, index, INDEX);
    if (_assert_index_range(vector, norm_index, INDEX)) return -1;

    _shift(vector, norm_index + 1, SHIFT_DOWN);
    --vector->len;

    return 0;
}



int cm_vector_shrink_to_fit(cm_vector * vector) {

    //half allocation size down to VECTOR_DEFAULT_SIZE
    while (((size_t) vector->len < vector->size) 
            && (vector->size != VECTOR_DEFAULT_SIZE)) {
        vector->size /= 2;
    }

    //perform reallocation
    vector->data = realloc(vector->data, vector->size * vector->data_size);
    if (vector->data == NULL) {
        
        cm_errno = CM_ERR_REALLOC;
        return -1;
    }

    return 0;
}



void cm_vector_empty(cm_vector * vector) {

    vector->len = 0;

    return;
}



int cm_new_vector(cm_vector * vector, const size_t data_size) {

    vector->len = 0;
    vector->size = VECTOR_DEFAULT_SIZE;
    vector->data_size = data_size;
    
    if (_alloc(vector)) return -1;
    
    return 0;
}



void cm_del_vector(cm_vector * vector) {

    free(vector->data);
}
