//standard library
#include <stdlib.h>
#include <string.h>

//system headers
#include <unistd.h>

//local headers
#include "cmore.h"
#include "debug.h"
#include "vector.h"



/*
 *  --- [INTERNAL] ---
 */

DBG_STATIC
int _vector_alloc(cm_vector * vector) {

    vector->data = malloc(vector->data_size * vector->size);
    if (!vector->data) {
        cm_errno = CM_ERR_MALLOC;
        return -1;
    }

    return 0;
}



DBG_STATIC 
int _vector_grow(cm_vector * vector) {

    vector->size = vector->size * 2;

    vector->data = realloc(vector->data, vector->data_size * vector->size);
    if (!vector->data) {
        cm_errno = CM_ERR_REALLOC;
        return -1;
    }

    return 0;
}



DBG_STATIC DBG_INLINE 
int _vector_normalise_index(const cm_vector * vector, 
                            int index, const enum _vector_index_mode mode) {

    //if negative index supplied
    if (index < 0) {

        index = vector->len + index;
        if (mode == ADD_INDEX) index++;
    }

    return index;
}



DBG_STATIC 
void * _vector_traverse(const cm_vector * vector, const int index) {

    return vector->data + (vector->data_size * index);
}



DBG_STATIC 
void _vector_shift(cm_vector * vector,
                   const int index, const enum _vector_shift_mode mode) {

    int diff;

    //calculate how many indeces remain from the given index
    diff = vector->len - index;
    if (diff <= 0) return;

    //calculate how many bytes the remaining indeces constitute
    int move_size = diff * vector->data_size;

    void * data = _vector_traverse(vector, index);
    memmove(data + (ssize_t) (vector->data_size * mode), data, move_size);

    return;
}



DBG_STATIC DBG_INLINE 
void _vector_set(cm_vector * vector, const int index, const void * data) {

    void * index_data = _vector_traverse(vector, index);
    memcpy(index_data, data, vector->data_size);

    return;
}



DBG_STATIC DBG_INLINE 
int _vector_assert_index_range(const cm_vector * vector, const int index, 
                               const enum _vector_index_mode mode) {

    /*
     *  If inserting, maximum index needs to be +1 higher than for other operations.
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

int cm_vector_get_val(const cm_vector * vector, const int index, void * buf) {

    int norm_index = _vector_normalise_index(vector, index, INDEX);
    if (_vector_assert_index_range(vector, norm_index, INDEX)) return -1;
    
    void * data = _vector_traverse(vector, norm_index); 
    memcpy(buf, data, vector->data_size);

    return 0;
}



void * cm_vector_get_ref(const cm_vector * vector, const int index) {

    int norm_index = _vector_normalise_index(vector, index, INDEX);
    if (_vector_assert_index_range(vector, norm_index, INDEX)) return NULL;
    
    void * data = _vector_traverse(vector, norm_index); 
    return data;
}



int cm_vector_set(cm_vector * vector, const int index, const void * data) {

    int norm_index = _vector_normalise_index(vector, index, INDEX);
    if (_vector_assert_index_range(vector, norm_index, INDEX)) return -1;

    _vector_set(vector, norm_index, data);

    return 0;
}



int cm_vector_insert(cm_vector * vector, const int index, const void * data) {

    int norm_index = _vector_normalise_index(vector, index, ADD_INDEX);
    if (_vector_assert_index_range(vector, norm_index, ADD_INDEX)) return -1;

    //grow the vector if there is no space left to insert new elements
    if ((size_t) vector->len == vector->size) {
        if(_vector_grow(vector)) return -1;
    }

    _vector_shift(vector, norm_index, SHIFT_UP);
    _vector_set(vector, norm_index, data);
    ++vector->len;

    return 0;
}



int cm_vector_append(cm_vector * vector, const void * data) {
 
    //grow the vector if there is no space left to insert new elements
    if ((size_t) vector->len == vector->size) {
        if(_vector_grow(vector)) return -1;
    }

    _vector_set(vector, vector->len, data);
    ++vector->len;
    return 0;
}



int cm_vector_remove(cm_vector * vector, const int index) {

    int norm_index = _vector_normalise_index(vector, index, INDEX);
    if (_vector_assert_index_range(vector, norm_index, INDEX)) return -1;

    _vector_shift(vector, norm_index + 1, SHIFT_DOWN);
    --vector->len;

    return 0;
}



int cm_vector_fit(cm_vector * vector) {

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
    
    if (_vector_alloc(vector)) return -1;
    
    return 0;
}



void cm_del_vector(cm_vector * vector) {

    free(vector->data);
}
