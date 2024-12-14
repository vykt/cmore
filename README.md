# CMore

<p align="center">
    <img src="cmore.png" width="150" height="150">
</p>


### ABOUT:

The C More Library (**CMore**) aims to provide essential data structures \
and algorithms for the C language. Presently it implements *lists*, *vectors*, \
and *red-black trees*.

This library is a cornerstone of my major projects, namely:

- [lain](https://github.com/vykt/lain) - A memory manipulation library.
- [ptrscan](https://github.com/vykt/ptrscan) - A pointer scanner for linux.

On a more personal note, this library is also where I test out new approaches \
to development, new tooling, and practice implementing data structures for \
interviews.



### BUILDING:

Building **CMore** requires `make`. Your Linux distribution will \
likely package `make` as part of its `build-essential` or `base-devel` \
meta-packages. Once `make` is installed, proceed through the following steps:


The main branch is used for development and is not guaranteed to be \
functional. Clone the latest release instead:

```
$ git clone --branch 1.0.0 https://github.com/vykt/cmore
```

You have the option of building a dynamic and/or static version of the \
library. By default, **CMore** will build with the **-O3** flag. If you are \
interested in debugging you can include debug symbols and turn off \
optimisations by passing the `build=debug` variable to make.

```
$ cd cmore
$ make shared            #builds a shared object
$ make static            #builds a static object archive
$ make all build=debug   #builds a debug version of both
```

To remove build artifacts:

```
$ make clean
```


Before installing, make sure to check the `Makefile` installation directories. \

```
$ ${EDITOR} Makefile
# make install
```

The `install` target will install manpage documentation. To install additional \
markdown documentation, run the `install_docs` target:

```
# make install_docs
```

To uninstall **CMore** run the `uninstall` target:

```
# make uninstall
```

### DOCUMENTATION:

Documentation is available in the `./doc` directory. Section 7 manpages are \
installed system-wide as part of a default installation. **CMore** \
documentation describes the functionality of individual components and \
provides examples. Individual functions are not documented however; users are \
encouraged to take a look at the `cmore.h` header installed in \
`/usr/local/include` by default. For further examples, consider reviewing unit \
tests in `./src/test`.

The contents of the manpages are available in alternatives formats found in \
the `./docs` directory. They can optionally be installed system-wide.
