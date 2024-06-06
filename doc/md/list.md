### LIBRARY
C more library (libcmore, -lcmore)


### SYNOPSIS
```c
struct _cm_list_node {

    cm_byte * data;
    struct _cm_list_node * next;
    struct _cm_list_node * prev;

};
typedef struct _cm_list_node cm_list_node;

typedef struct {

    int len;
    size_t data_size;
    cm_list_node * head;

} cm_list;

int cm_list_get_val(cm_list * list, int index, cm_byte * buf);
cm_byte * cm_list_get_ref(cm_list * list, int index);

int cm_list_set(cm_list * list, int index, cm_byte * data);
int cm_list_insert(cm_list * list, int index, cm_byte * data);
int cm_list_append(cm_list * list, cm_byte * data);
int cm_list_remove(cm_list * list, int index);
int cm_list_empty(cm_list * list);

void cm_new_list(cm_list * list, size_t data_size);
int cm_del_list(cm_list * list);
```


### STRUCTURE
The *cm_list* is a doubly-linked list consisting of *cm_list_node* nodes. The *cm_list* structure must be allocated before a pointer to it can be passed to any function. The **cm_new_list()** function allocates and set members of the *cm_list* structure, not te structure itself.  
  
The *len* member stores the number of nodes in the list.  
  
The *data_size* member stores the size in bytes of a single element in the list.  
  
The *\*head* member points to the first node, *cm_list_node*, in the list.  
  

### FUNCTIONS
The **cm_new_list()** function takes a pointer to a list *\*list* and initialises it. The initialised list holds nodes that store data of size *data_size*.  
  
The **cm_del_list()** function deinitialises a list pointed to by *\*list*.  
  
  
The **cm_list_get_val()** function performs an equivalent operation to *list[index]*, storing the result by value in the buffer *\*buf*.  
  
The **cm_list_get_ref()** function returns a pointer to the value stored at *list[index]*.  
  
  
The **cm_list_set()** function copies the contents of *\*data* to *list[index]*.  
  
The **cm_list_insert()** function inserts a new element with value *\*data* at *list[index]*. This may cause a reallocation.  
  
The **cm_list_append()** function appends a new element with value *\*data* at the end of *\*list*.  
  
The **cm_list_remove()** function removes an element at *list[index]*.  
  
The **cm_list_empty()** empties the contents of *\*list*. The memory is not overwritten.  
  

### RETURN VALUE
The **cm_list_get_ref()** function returns a pointer to the value at *list[index]* on success. On error, NULL is returned. All other functions return 0 on success or -1 on error. On error, *cm_errno* is set. See **libcmore_error**().  
  

### EXAMPLES
See *src/test/list.c* for examples.  
  

### SEE ALSO
**libcmore_vector**(3), **libcmore_error**(3)
