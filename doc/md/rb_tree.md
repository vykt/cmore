---
date: Dec 2024
title: CMORE RED-BLACK TREE
---

# LIBRARY

The C More Library (*libcmore*, *-lcmore*)

# DESCRIPTION

The **CMore** *red-black tree* is a self balancing binary sorted tree
that maintains a guarantee of **O(log n)** for traversal, insertion, and
deletion. The *red-black tree* is represented by a `cm_rb_tree`
structure. It holds the size (in \# of elements), the key size for each
node (in bytes), the data size for each node (in bytes), a pointer to
the root node, and a pointer to the *compare()* function that will be
covered shortly.

Each node in the *red-black tree* is represented by a `cm_rb_tree_node`
structure. It consists of a pointer to an allocation storing the key for
this node, a pointer to an allocation storing the data for this node, a
pointer to a left node, a pointer to a right node, a pointer to a parent
node, whether this node is to the left or the right of its parent,
represented by a `cm_rb_tree_eval` enum, and a a colour for this node
represented by a `cm_r_tree_colour` enum.

Because a *red-black tree* is a sorted generic container, it must call a
user-specified *compare()* function. This function accepts two
parameters. It must implement a comparison between these two parameters
and return `cm_rb_tree_eval` `LESS` if parameter one is less than
parameter two, `MORE` if parameter one is greater than parameter two,
and `EQUAL` if the parameters are equal, `EQUAL` should be returned.
*compare()* is not allowed to return `ROOT`. Below is an example compare
function:

    enum cm_rb_tree_eval int_compare(void * arg1, void * arg2) {

        int num1 = *(int *) arg1;
        int num2 = *(int *) arg2;

        if (num1 > num2) return MORE;
        if (num1 < num2) return LESS;

        return EQUAL;
    }

A *red-black tree* is created with `cm_new_rb_tree()`. This initialiser
function does not allocate any memory. A *red-black tree* is destroyed
with `cm_del_rb_tree()`. A *red-black tree* can be emptied with
`cm_rb_tree_empty()`:

    cm_rb_tree rb_tree;

    //initialise the red-black tree
    cm_new_rb_tree(&rb_tree, sizeof(int), sizeof(int), int_compare);

    //empty the red-black tree
    cm_rb_tree_empty(&tree);

    //destroy the red-black tree
    cm_del_rb_tree(&tree);

`cm_rb_tree_get_val()` gets the data at a key and copies it to a buffer
`buf`. `cm_rb_tree_get_ref()` returns a pointer to the data at a key.
`cm_rb_get_node()` returns the node at a key, allowing for further
traversal using the `left`, `right`, and `parent` pointers. If a key is
not present in the *red-black tree*, a *CM_ERR_USER_KEY* error is stored
in `cm_errno`:

    cm_rb_tree rb_tree;
    cm_rb_tree_node * node;
    int ret, key, data, * data_ptr;

    //initialise the red-black tree
    cm_new_rb_tree(&rb_tree, sizeof(int), sizeof(int), int_compare);

    //[populate the red-black tree]

    //get the '2' key by value
    key = 2;
    ret = cm_rb_tree_get_val(&rb_tree, &key, &data);

    //get a pointer to the '5' key
    key = 5;
    data_ptr = cm_rb_tree_get_ref(&rb_tree, &key);

    //get the node of key '8'
    key = 8;
    node = cm_rb_tree_get_node(&rb_tree, &key);

    //get the parent node of key '8'
    if (node->parent_eval != ROOT) node = node->parent;

`cm_rb_tree_set()` assigns a value to a key. If the key does not exist
in the tree, a new node is created. If the key already exists, it is
overwritten. `cm_rb_tree_remove()` removes a node with a matching key
from the *red-black tree*. `cm_rb_tree_unlink()` unlinks a node from the
*red-black tree*. All of these functions return a pointer to the node
the operation was performed on, with the exception of
[cm_rb_tree_remove()]{.title-ref}. In the case of `cm_rb_tree_unlink`,
the returned node must later be freed with `cm_del_rb_tree_node()` to
prevent a memory leak. Requesting to remove or unlink a key that is not
present in the *red-black tree* will result in a *CM_ERR_USER_KEY*
error:

    cm_rb_tree rb_tree;
    cm_rb_tree_node * node;
    int ret, key, data;

    //initialise the red-black tree
    cm_new_rb_tree(&rb_tree, sizeof(int), sizeof(int), int_compare);

    //set a key
    key = 2;
    data = 20;
    node = cm_rb_tree_set(&tree, &key, &data);

    //set another key
    key = 4;
    data = 40;
    node = cm_rb_tree_set(&tree, &key, &data);

    //remove a key
    key = 2;
    ret = cm_rb_tree_remove(&tree, &key);

    //unlink and free a node at key
    key = 4;
    node = cm_rb_tree_unlink(&tree, &key);
    cm_del_rb_tree_node(node);

    //destroy the red-black tree
    cm_del_rb_tree(&tree);

On error, *NULL* or *-1* is returned depending on the function. See
**CMore** **error** documentation to determine the precise cause of an
error.
