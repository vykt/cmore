==========
CMORE LIST
==========

:date: Dec 2024

LIBRARY
=======
The C More Library (*libcmore*, *-lcmore*)

DESCRIPTION
===========

The **CMore** *list* is a doubly linked list. A *list* is represented by a \
``cm_list`` structure. It holds the length of a *list*, the size (in bytes) \
of every data element in a *list* (``data_size``), and a pointer to the head \
of the *list*.

Each node in a list is represented by a ``cm_list_node`` structure. It \
consists of a next pointer, a previous pointer, and a pointer to an \
allocation of size ``data_size`` (in bytes) that stores the data for this node.

A *list* is created with ``cm_new_list()``. This initialiser function does \
not allocate any memory. A list is destroyed with ``cm_del_list()``. A *list* \
may be emptied with ``cm_list_empty()``. For example::

	cm_list list;

	//initialise the list
	cm_new_list(&list, sizeof(int));

	//destroy the list
	cm_del_list(&list);

``cm_list_get_val()`` gets the data at an index and copies it to a buffer \
``buf``. ``cm_list_get_ref()`` returns a pointer to the data at an index. 
``cm_list_get_node()`` returns the node at an index, allowing for further \
traversal using the ``next`` and ``prev`` pointers.

Both positive and negative indeces can be used. Specifying a negative index \
will cause the *list* to be traversed using the ``prev`` pointers. Specifying \
an index that would cause the *list* traversal to perform a loop will result \
in a *CM_ERR_USER_INDEX* error::

	cm_list list;
	cm_list_node * node;
	int ret, data, * data_ptr;

	//initialise the list
	cm_new_list(&list, sizeof(int));
	
    //[populate the list]

	//get the second index by value
	ret = cm_list_get_val(&list, 2, &data);

	//get a pointer to second-to-last index
	data_ptr = cm_list_get_ref(&list, -2);

	//get a pointer to the third index's node
	node = cm_list_get_node(&list, 3);

	//get a pointer to the fourth index's node
	node = node->next;

	//destroy the list
	cm_del_list(&list);
	
``cm_list_set()`` overwrites the value at an index. ``cm_list_insert()`` \
inserts a value at the specified index. ``cm_list_append()`` inserts a value \
at the end of the *list*. ``cm_list_remove()`` removes a value from the list. \
``cm_list_unlink()`` unlinks a node from the tree. All of these functions \
return a pointer to the node the operation was performed on for convenience, \
with the exception of ``cm_list_remove()``. In the case of \
``cm_list_unlink()``, the returned node must later be freed with \
``cm_del_list_node()`` to prevent a memory leak. Requesting to insert, set, \
remove, or unlink at an index that is out of range will result in a \
*CM_ERR_USER_KEY* error::
	
	cm_list list;
	cm_list_node * node;
	int ret, data;

	//initialise the list
	cm_new_list(&list, sizeof(int));

	//insert a value
	data = 5;
	node = cm_list_insert(&list, 0, &data);

	//append a value
	data = 3;
	node = cm_list_append(&list, &data);

	//overwrite the appended value
	data = 10;
	node = cm_list_set(&list, -1, &data);

	//remove a value
	ret = cm_list_remove(&list, 1);

	//unlink and free a node
	node = cm_list_unlink(&list, 0);
	cm_del_list_node(node);
	
	//destroy the list
	cm_del_list(&list);

On error, *NULL* or *-1* is returned depending on the function. See **CMore** \
**error** documentation to determine the precise cause of an error.
