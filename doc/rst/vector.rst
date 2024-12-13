============
CMORE VECTOR
============

:date: Dec 2024

LIBRARY
=======
The C More Library (*libcmore*, *-lcmore*)

DESCRIPTION
===========

The **CMore** *vector* is a typical vector. It is represented by a 
``cm_vector`` structure. It holds the size of one data element (in bytes), the \
length of the *vector* (in # of elements), the current allocation size (in # \
of elements), and a pointer to the allocation.

A *vector* is created by calling ``cm_new_vector()``. This initialiser \
function does allocate memory. The startin size of a *vector* allocation \
is **8**. A *vector* can be destroyed with `cm_del_vector()`. A *vector* may \
be emptied with ``cm_vector_empty()``.

	int ret;
	cm_vector vector;

	//initialise the vector
	ret = cm_new_vector(&vector, sizeof(int));

	//destroy the vector
	cm_del_vector(&vector);

``cm_vector_get_val()`` gets the data at an index and copies it to a buffer \
``buf``. ``cm_vector_get_ref()`` returns a pointer to the data at an index. \
Both positive and negative indeces can be used. If an index is not in range, \
a *CM_ERR_USER_INDEX* error will be stored in *cm_errno*::

	cm_vector vector;
	int ret, data, * data_ptr;

	//initialise the vector
	ret = cm_new_vector(&vector, sizeof(int));

    //[populate the vector]

	//get the second index by value
	ret = cm_vector_get_val(&vector, 2, &data);

	//get a pointer to third-to-last index
	data_ptr = cm_vector_get_ref(&vector, -3);

	//destroy the vector
	cm_del_vector(&vector);

``cm_vector_set()`` overrides the value at an index. ``cm_vector_insert()`` \
inserts a value at the specified index. ``cm_vector_append()`` inserts a value \
at the end of the *vector*. 

When the length of a *vector* exceeds the allocated size, the allocation \
doubles in size. This means the size of the *vector* allocation grows \
exponentially. Removing elements from the *vector* or emptying it does not \
reduce the size of the allocation. The allocation of a *vector* can be set to \
its length by calling ``cm_vector_fit()``. Requesting to insert, set, or \
remove at an index that is out of range will result in a *CM_ERR_USER_INDEX* \
error::

	cm_vector vector;
	int ret, data;

	//initialise the vector
	ret = cm_new_vector(&vector, sizeof(int));

	//insert a value
	data = 5;
	int = cm_vector_insert(&vector, 0, &data);

	//append a value
	data = 3;
	int = cm_vector_append(&vector, &data);

	//overwrite the appended value
	data = 10;
	int = cm_vector_set(&vector, -1, &data);

	//remove a value
	ret = cm_vector_remove(&vector, 1);
	
	//destroy the vector
	cm_del_vector(&vector);

On error, *NULL* or *-1* is returned depending on the function. See **CMore** \
**error** documentation to determine the precise cause of an error.
