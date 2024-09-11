### LIBRARY
C more library (libcmore, -lcmore)


### SYNOPSIS
```c
typedef struct {

    int len;     //number of elements used
    size_t size; //number of elements allocated
    size_t data_size;
    cm_byte * data;

} cm_vector;


int cm_vector_get_val(cm_vector * vector, int index, cm_byte * buf);
cm_byte * cm_vector_get_ref(cm_vector * vector, int index);

int cm_vector_set(cm_vector * vector, int index, cm_byte * data);
int cm_vector_insert(cm_vector * vector, int index, cm_byte * data);
int cm_vector_append(cm_vector * vector, cm_byte * data);
void cm_vector_remove(cm_vector * vector, int index);

void cm_vector_empty(cm_vector * vector);

int cm_new_vector(cm_vector * vector, size_t data_size);
void cm_del_vector(cm_vector * vector);
```


### STRUCTURE
The *cm_vector* structure must be allocated before a pointer to it can be passed to any function. The **cm_new_vector()** function allocates and set members of the *cm_vector* structure, not te structure itself.  
  
The *len* member stores the length of the vector. The *size* member stores the number of elements that can be held by the currently allocated space for this vector. When inserting or appending to a vector where *len*==*size*, the vector will automatically be reallocated and *size* doubled. *size* grows exponentially.  
  
The *data_size* member stores the size in bytes of a single element in the vector.  
  
The *\*data* member stores the allocation used by the vector for storage. It's size is equal to *size* * *data_size*.  
  

### FUNCTIONS
The **cm_new_vector()** function takes a pointer to a vector *\*vector* and initialises it. The initialised vector holds elements of size *data_size*.  
  
The **cm_del_vector()** function deinitialises a vector pointed to by *\*vector*.  
  
  
The **cm_vector_get_val()** function performs an equivalent operation to *vector[index]*, storing the result by value in the buffer *\*buf*.  
  
The **cm_vector_get_ref()** function returns a pointer to the value stored at *vector[index]*.  
  
  
The **cm_vector_set()** function copies the contents of *\*data* to *vector[index]*.  
  
The **cm_vector_insert()** function inserts a new element with value *\*data* at *vector[index]*. This may cause a reallocation.  
  
The **cm_vector_append()** function appends a new element with value *\*data* at the end of *\*vector*.  
  
The **cm_vector_remove()** function removes an element at *vector[index]*.  
  
The **cm_vector_empty()** empties the contents of *\*vector*. The memory is not overwritten.  
  

### RETURN VALUES
The **cm_vector_get_ref()** function returns a pointer to the value at *vector[index]* on success. On error, NULL is returned. 

All other functions return 0 on success or -1 on error. 

On error, *cm_errno* is set. See **libcmore_error**(3).  
  

### EXAMPLES
See *src/test/vector.c* for examples.  
  

### SEE ALSO
**libcmore_list**(3), **libcmore_error**(3)
