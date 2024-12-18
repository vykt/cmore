//standard library
#include <stdlib.h>
#include <string.h>

//system headers
#include <unistd.h>

//local headers
#include "cmore.h"
#include "debug.h"
#include "vct.h"



/*
 *  --- [INTERNAL] ---
 */

DBG_STATIC
int _vct_alloc(cm_vct * vector) {

    vector->data = malloc(vector->data_sz * vector->sz);
    if (!vector->data) {
        cm_errno = CM_ERR_MALLOC;
        return -1;
    }

    return 0;
}



DBG_STATIC 
int _vct_grow(cm_vct * vector) {

    vector->sz = vector->sz * 2;

    vector->data = realloc(vector->data, vector->data_sz * vector->sz);
    if (!vector->data) {
        cm_errno = CM_ERR_REALLOC;
        return -1;
    }

    return 0;
}



DBG_STATIC DBG_INLINE 
int _vct_normalise_index(const cm_vct * vector, 
                         int index, const enum _vct_index_mode mode) {

    //if negative index supplied
    if (index < 0) {

        index = vector->len + index;
        if (mode == ADD_INDEX) index++;
    }

    return index;
}



DBG_STATIC 
void * _vct_traverse(const cm_vct * vector, const int index) {

    return vector->data + (vector->data_sz * index);
}



DBG_STATIC 
void _vct_shift(cm_vct * vector,
                const int index, const enum _vct_shift_mode mode) {

    int diff;

    //calculate how many indeces remain from the given index
    diff = vector->len - index;
    if (diff <= 0) return;

    //calculate how many bytes the remaining indeces constitute
    int move_sz = diff * vector->data_sz;

    void * data = _vct_traverse(vector, index);
    memmove(data + (ssize_t) (vector->data_sz * mode), data, move_sz);

    return;
}



DBG_STATIC DBG_INLINE 
void _vct_set(cm_vct * vector, const int index, const void * data) {

    void * index_data = _vct_traverse(vector, index);
    memcpy(index_data, data, vector->data_sz);

    return;
}



DBG_STATIC DBG_INLINE 
int _vct_assert_index_range(const cm_vct * vector, 
                            const int index, const enum _vct_index_mode mode) {

    /*
     *  If inserting, maximum index needs to be +1 higher 
     *  than for other operations.
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

int cm_vct_get(const cm_vct * vector, const int index, void * buf) {

    int norm_index = _vct_normalise_index(vector, index, INDEX);
    if (_vct_assert_index_range(vector, norm_index, INDEX)) return -1;
    
    void * data = _vct_traverse(vector, norm_index); 
    memcpy(buf, data, vector->data_sz);

    return 0;
}



void * cm_vct_get_p(const cm_vct * vector, const int index) {

    int norm_index = _vct_normalise_index(vector, index, INDEX);
    if (_vct_assert_index_range(vector, norm_index, INDEX)) return NULL;
    
    void * data = _vct_traverse(vector, norm_index); 
    return data;
}



int cm_vct_set(cm_vct * vector, const int index, const void * data) {

    int norm_index = _vct_normalise_index(vector, index, INDEX);
    if (_vct_assert_index_range(vector, norm_index, INDEX)) return -1;

    _vct_set(vector, norm_index, data);

    return 0;
}



int cm_vct_ins(cm_vct * vector, const int index, const void * data) {

    int norm_index = _vct_normalise_index(vector, index, ADD_INDEX);
    if (_vct_assert_index_range(vector, norm_index, ADD_INDEX)) return -1;

    //grow the vector if there is no space left to insert new elements
    if ((size_t) vector->len == vector->sz) {
        if(_vct_grow(vector)) return -1;
    }

    _vct_shift(vector, norm_index, SHIFT_UP);
    _vct_set(vector, norm_index, data);
    ++vector->len;

    return 0;
}



int cm_vct_apd(cm_vct * vector, const void * data) {
 
    //grow the vector if there is no space left to insert new elements
    if ((size_t) vector->len == vector->sz) {
        if(_vct_grow(vector)) return -1;
    }

    _vct_set(vector, vector->len, data);
    ++vector->len;
    return 0;
}



int cm_vct_rem(cm_vct * vector, const int index) {

    int norm_index = _vct_normalise_index(vector, index, INDEX);
    if (_vct_assert_index_range(vector, norm_index, INDEX)) return -1;

    _vct_shift(vector, norm_index + 1, SHIFT_DOWN);
    --vector->len;

    return 0;
}



int cm_vct_fit(cm_vct * vector) {

    //half allocation size down to VECTOR_DEFAULT_SIZE
    while (((size_t) vector->len < vector->sz) 
            && (vector->sz != VECTOR_DEFAULT_SIZE)) {
        vector->sz /= 2;
    }

    //perform reallocation
    vector->data = realloc(vector->data, vector->sz * vector->data_sz);
    if (vector->data == NULL) {
        
        cm_errno = CM_ERR_REALLOC;
        return -1;
    }

    return 0;
}



void cm_vct_emp(cm_vct * vector) {

    vector->len = 0;

    return;
}



int cm_new_vct(cm_vct * vector, const size_t data_sz) {

    vector->len = 0;
    vector->sz = VECTOR_DEFAULT_SIZE;
    vector->data_sz = data_sz;
    
    if (_vct_alloc(vector)) return -1;
    
    return 0;
}



void cm_del_vct(cm_vct * vector) {

    free(vector->data);
}
