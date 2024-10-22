### LIBRARY
C more library (libcmore, -lcmore)


### SYNOPSIS
```c
_Thread_local int cm_errno;

void cm_perror();
const char * cm_strerror(const int cm_errnum);
```


### STRUCTURE
When a **libcmore** function return a value indicating an error (typically -1 or NULL), the integer *cm_errno* is set to hold a unique error number that describes the error that occurred. Each error number has a corresponding textual description.  
  
There are 3 classes of error numbers:  

- *11XX* : Errors caused by the user of the library.
- *12XX* : Errors caused by an internal bug in the library.
- *13XX* : Errors caused by the environment (such as a failure to allocate memory).  
  
The *cm_errno* value is stored in thread-local storage. Setting it in one thread does not affect its value in any other thread.


### FUNCTIONS
The **cm_perror()** function outputs the textual description of the last error to occur, stored in *cm_errno*, to standard error. 

The **cm_strerror()** takes a error number and returns a pointer to the textual description for the said error number.  


### RETURN VALUES
The **cm_strerror()** function returns a pointer to the error string.  


### EXAMPLES
See *src/test/error.c* for examples.  
  

### SEE ALSO
**libcmore_list**(3), **libcmore_vector**(3)
