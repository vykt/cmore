---
date: Dec 2024
title: CMORE LIST
---

# LIBRARY

The C More Library (*libcmore*, *-lcmore*)

# DESCRIPTION

The **CMore** *lst* is a doubly linked list. A *lst* is represented by a
`cm_lst` structure. It holds the length of a *lst*, the size (in bytes)
of every data element in a *lst* (`data_sz`), and a pointer to the head
of the *lst*.

Each node in a list is represented by a `cm_lst_node` structure. It
consists of a next pointer, a previous pointer, and a pointer to an
allocation of size `data_sz` (in bytes) that stores the data for this
node.

A *lst* is created with `cm_new_lst()`. This initialiser function does
not allocate any memory. A *lst* is destroyed with `cm_del_lst()`. A
*lst* may be emptied with `cm_list_emp()`. For example:

    cm_list list;

    //initialise the list
    cm_new_list(&list, sizeof(int));

    //destroy the list
    cm_del_list(&list);

`cm_list_get()` gets the data at an index and copies it to a buffer
`buf`. `cm_list_get_p()` returns a pointer to the data at an index.
`cm_list_get_n()` returns the node at an index, allowing for further
traversal using the `next` and `prev` pointers.

Both positive and negative indeces can be used. Specifying a negative
index will cause a *lst* to be traversed using the `prev` pointers.
Specifying an index that would cause the *lst* traversal to perform a
loop will result in a *CM_ERR_USER_INDEX* error:

    cm_lst list;
    cm_lst_node * node;
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

Performing operations on a *lst* can be done using either an index or a
pointer to a node. `cm_list_set()` overwrites the value at an index.
`cm_list_set_n()` overwrites the value of the provided node.
`cm_list_ins()` inserts a value at the specified index.
`cm_list_ins_nb()` inserts a value before the provided node.
`cm_list_ins_na()` inserts a value after the provided
node`.`[cm_list_apd()]{.title-ref}\` inserts a value at the end of the
*lst*. `cm_list_rem()` removes a value at an index from the *lst*.
`cm_list_rem_n()` removes the provided node from the *lst*.
`cm_list_uln()` unlinks a node at an index from the *lst*.
`cm_list_uln_n()` unlinks the provided node from the *lst*. All of these
functions return a pointer to the node the operation was performed on
for convenience, with the exceptions of `cm_list_rem()` and
`cm_list_rem_n()`. In the case of `cm_list_uln()` and `cm_list_uln_n()`,
the returned node must later be freed with `cm_del_lst_node()` to
prevent a memory leak. Requesting to insert, set, remove, or unlink at
an index that is out of range will result in a *CM_ERR_USER_KEY* error:

    cm_lst list;
    cm_lst_node * node;
    int ret, data;

    //initialise the list
    cm_new_list(&list, sizeof(int));

    //insert a value by index: [5]
    data = 5;
    node = cm_list_ins(&list, 0, &data);

    //insert before the 'node' pointer: [7, 5]
    data = 7;
    node = cm_list_ins_nb(&list, node, &data);

    //insert after the 'node' pointer: [7, 9, 5]
    data = 9;
    node = cm_list_ins_na(&list, node, &data);

    //append a value: [7, 9, 5, 3]
    data = 3;
    node = cm_list_apd(&list, &data);

    //overwrite the appended value by index: [7, 9, 5 ,-3]
    data = -3;
    node = cm_list_set(&list, -1, &data);

    //overwrite the second node with its pointer: [7, -9, 5 ,-3]
    data = -9;
    node = cm_list_set_n(&list, list.head->next, &data);

    //remove a value bu index: [7, 5, -3]
    ret = cm_list_rem(&list, 1);

    //remove a value by pointer: [5, -3]
    node = cm_list_rem_n(&list, list.head);

    //unlink a node by index and free it: [-3]
    node = cm_list_uln(&list, 0);
    cm_del_lst_node(node);

    //inlink a node with its pointer and free it:
    node = cm_list_uln_n(&list, list.head);
    cm_del_lst_node(node);

    //destroy the list
    cm_del_list(&list);

On error, *NULL* or *-1* is returned depending on the function. See
**CMore** **error** documentation to determine the precise cause of an
error.
