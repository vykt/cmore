#ifndef VECTOR_H
#define VECTOR_H

#include <unistd.h>

#include "cmore.h"


#define VECTOR_DEFAULT_SIZE 8


//controls if user provided index should be verified for accessing elements 
//or for adding new elements
enum _index_mode {INDEX = 0, ADD_INDEX = 1};

//controls if vector elements are shifted up or down in memory
enum _shift_mode {SHIFT_UP = 1, SHIFT_DOWN = -1};

//function signatures
int cm_vector_get_val(const cm_vector * vector, const int index, cm_byte * buf);
cm_byte * cm_vector_get_ref(const cm_vector * vector, const int index);

int cm_vector_set(cm_vector * vector, const int index, const cm_byte * data);
int cm_vector_insert(cm_vector * vector, const int index, const cm_byte * data);
int cm_vector_append(cm_vector * vector, const cm_byte * data);
int cm_vector_remove(cm_vector * vector, const int index);
void cm_vector_empty(cm_vector * vector);

int cm_new_vector(cm_vector * vector, const size_t data_size);
void cm_del_vector(cm_vector * vector);


#endif
