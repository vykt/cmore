---
date: Dec 2024
title: CMORE ERRORS
---

# LIBRARY

The C More Library (*libcmore*, *-lcmore*)

# DESCRIPTION

**CMore** implements its own version of libc\'s *errno*. If you are
familiar with it, **CMore**\'s implementation should be very intuitive.

**CMore** functions return **NULL** or **-1** on fail. The cause of the
last error to occur is stored as an *error code* in the thread-local
integer `cm_errno`. Each *error code* has a corresponding message that
describes the error.

`cm_perror()` will print the last error\'s message to **stderr**:

    //cause an error (index too large)
    cm_list_remove(&list, 1337);

    //print error code message to stderr
    cm_perror("Forced error");

    //[output]: Forced error: Index out of range.

`cm_strerror()` takes an *error code* and returns a pointer to its
corresponding message:

    //get error message for a key error
    char * key_error_msg = cm_strerror(CM_ERR_USER_KEY);

    //print the error message
    puts(key_error_msg);

    //[output]: Key not present in tree.

*Error code* values consist of four (4) digits. The first (1) digit
stores the owner of the error code. Value of **1** is reserved for
**CMore**. The second (2) digit stores the class of the error. Value
**1** represents a user error, such as specifying an invalid index.
Value **2** represents an internal **CMore** error typically caused by
bugs in **CMore**. Value **3** represents an environment error, such as
a failure to acquire memory. Digits three (3) and four (4) store the
error number in that class.
