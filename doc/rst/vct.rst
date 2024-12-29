============
CMORE VECTOR
============

:date: Dec 2024

LIBRARY
=======
The C More Library (*libcmore*, *-lcmore*)

DESCRIPTION
===========

The **CMore** *vct* is a typical vector. It is represented by a 
``cm_vct`` structure. It holds the size of one data element (in bytes), the \
length of the *vct* (in # of elements), the current allocation size (in # \
of elements), and a pointer to the allocation.

A *vct* is created by calling ``cm_new_vct()``. This initialiser \
function does allocate memory. The startin size of a *vct* allocation \
is **8**. A *vct* can be destroyed with `cm_del_vct()`. A *vct* may \
be emptied with ``cm_vct_emp()``.

	int ret;
	cm_vct vector;

	//initialise the vector
	ret = cm_new_vector(&vector, sizeof(int));

	//destroy the vector
	cm_del_vector(&vector);

``cm_vct_get()`` gets the data at an index and copies it to a buffer \
``buf``. ``cm_vct_get_p()`` returns a pointer to the data at an index. \
Both positive and negative indeces can be used. If an index is not in range, \
a *CM_ERR_USER_INDEX* error will be stored in *cm_errno*::

	cm_vct vector;
	int ret, data, * data_ptr;

	//initialise the vector
	ret = cm_new_vct(&vector, sizeof(int));

    //[populate the vector]

	//get the second index by value
	ret = cm_vct_get(&vector, 2, &data);

	//get a pointer to third-to-last index
	data_ptr = cm_vct_get_p(&vector, -3);

	//destroy the vector
	cm_del_vct(&vector);

``cm_vct_set()`` overrides the value at an index. ``cm_vct_ins()`` \
inserts a value at the specified index. ``cm_vct_apd()`` inserts a value \
at the end of the *vct*. 

When the length of a *vct* exceeds the allocated size, the allocation \
doubles in size. This means the size of the *vct* allocation grows \
exponentially. Removing elements from the *vct* or emptying it does not \
reduce the size of the allocation. The allocation of a *vct* can be set to \
its length by calling ``cm_vct_fit()``. Requesting to insert, set, or \
remove at an index that is out of range will result in a *CM_ERR_USER_INDEX* \
error::

	cm_vct vector;
	int ret, data;

	//initialise the vector
	ret = cm_new_vct(&vector, sizeof(int));

	//insert a value
	data = 5;
	int = cm_vct_ins(&vector, 0, &data);

	//append a value
	data = 3;
	int = cm_vct_apd(&vector, &data);

	//overwrite the appended value
	data = 10;
	int = cm_vct_set(&vector, -1, &data);

	//remove a value
	ret = cm_vct_rmv(&vector, 1);
	
	//destroy the vector
	cm_del_vct(&vector);

On error, *NULL* or *-1* is returned depending on the function. See **CMore** \
**error** documentation to determine the precise cause of an error.
