# libcmore

<p align="center">
    <img src="libcmore.png" width="150" height="150">
</p>


### ABOUT

The C More library (*libcmore*) provides a robust, minimal, and highly efficient implementation of the list and vector data structures for the C language. The goal of the library is to alleviate two major pain points of C programming:

- Inflexibility of fixed size built-in arrays.
- A lack of an indexable data structure that can be mutated without pointer invalidation.

<br>

### INSTALLATION

Building the C More library requires *make*. Your Linux distribution will package *make* as part of its *build-essential* or *base-devel* meta-packages. Once make is installed, proceed through the following steps:

<br>

**1) Clone this repository:**
```
$ git clone https://github.com/vykt/libcmore
```
The main branch will always contain the latest tested release. Alternatively, you may also download a release directly from the releases page.

<br>

**2) Build the release target:**
```
$ cd libcmore
$ make lib
```
Debug symbols are included with the release build.

<br>

**3) Run the installation script:**
```
$ vim install.sh
# ./install.sh
# ./install.sh doc   #optionally install markdown docs
```
Before running the installation script, open the script in an editor and confirm the installation paths for the library, the C header, and the manpages.

**4) To uninstall:**
```
# ./install.sh uninstall
```

<br>

### LINKING

To use the C More library as part of your project, you must include its header and pass the appropriate linking flags to your C compiler.

**1) Include the *libcmore* C header:**
```c
#include <libcmore.h>
```

<br>

**2) When compiling the executable, pass the *-lcmore* flag to the compiler:**
```bash
gcc -o your_executable main.c -lcmore
```
If you encounter difficulties at this stage, ensure the installation directory of *libcmore* is searched by the runtime linker (*man 8 ldconfig*).

<br>

### DOCUMENTATION:

Documentation is available in the markdown format inside the */doc/md* directory. Roff (manpage) format is installed on the system as part of the installation, and is additionally available under the */doc/roff/man.3* directory. If you are unable to view the manual pages, ensure your manpage path includes the path specified in the installation script (*man 1 manpath*).

<br>
