#ifndef VCT_H
#define VCT_H

//system headers
#include <unistd.h>

//local headers
#include "cmore.h"
#include "debug.h"


#define VECTOR_DEFAULT_SIZE 8


//controls if user provided index should be verified for accessing elements 
//or for adding new elements
enum _vct_index_mode {INDEX = 0, ADD_INDEX = 1};

//controls if vector elements are shifted up or down in memory
enum _vct_shift_mode {SHIFT_UP = 1, SHIFT_DOWN = -1};


#ifdef DEBUG
//internal
int _vct_alloc(cm_vct * vector);
int _vct_grow(cm_vct * vector)
;
int _vct_normalise_index(const cm_vct * vector, 
                         int index, const enum _vct_index_mode mode);
void  * _vct_traverse(const cm_vct * vector, const int index);

void _vct_shift(cm_vct * vector, 
                const int index, const enum _vct_shift_mode mode);

void _vct_set(cm_vct * vector, const int index, const void  * data);
int _vct_assert_index_range(const cm_vct * vector, 
                            const int index, const enum _vct_index_mode mode);
#endif


//external
int cm_vct_get(const cm_vct * vector, const int index, void  * buf);
void  * cm_vct_get_p(const cm_vct * vector, const int index);

int cm_vct_set(cm_vct * vector, const int index, const void  * data);
int cm_vct_ins(cm_vct * vector, const int index, const void  * data);
int cm_vct_apd(cm_vct * vector, const void  * data);
int cm_vct_rmv(cm_vct * vector, const int index);
int cm_vct_fit(cm_vct * vector);
void cm_vct_emp(cm_vct * vector);

int cm_new_vct(cm_vct * vector, const size_t data_sz);
void cm_del_vct(cm_vct * vector);

#endif
