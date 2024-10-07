# libcmore

<p align="center">
    <img src="libcmore.png" width="150" height="150">
</p>


### ABOUT

The C More library (**libcmore**) provides a robust, minimal, and highly efficient implementation of the list and vector data structures for the C language. The goal of the library is to alleviate two major pain points of C programming:

- Inflexibility of fixed size built-in arrays.
- A lack of an indexable data structure that can be mutated without pointer invalidation.


### INSTALLATION

Building the C More library requires `make`. Your Linux distribution will likely package `make` as part of its `build-essential` or `base-devel` meta-packages. Once make is installed, proceed through the following steps:


Clone this repository:
```
$ git clone https://github.com/vykt/libcmore
```
The main branch will always contain the latest tested release. Alternatively, you may also download a release directly from the releases page.


Build the release target:
```
$ cd libcmore
$ make lib
```
Debug symbols are included with the release build.


Run the installation script:
```
$ vim install.sh
# ./install.sh
```
Before running the installation script, open the script in an editor and confirm the installation paths for the library, the C header, and the manpages.

To optionally install markdown documenation:
```
# ./install.sh doc
```

To uninstall:
```
# ./install.sh uninstall
```


### LINKING

To use the C More library as part of your project, you must include its header and pass the appropriate linking flags to your C compiler.

Include the `libcmore.h` C header:**
```c
#include <libcmore.h>
```


When compiling the executable, pass the `-lcmore` flag to the compiler:
```bash
gcc -o your_executable main.c -lcmore
```
If you encounter difficulties at this stage, ensure the installation directory of **libcmore** is searched by the runtime linker (`man 8 ldconfig`).


### DOCUMENTATION:

Documentation is available in the markdown format inside the `./doc/md` directory. Manpages are installed on the system as part of the base installation and are available under `./doc/roff/man.3`. If you are unable to view the manpages, ensure your manpage path includes the path specified in the installation script (`man 1 manpath`).
