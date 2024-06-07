#!/bin/bash

#TODO [set as required] TODO
INSTALL_DIR=/usr/local/lib
INCLUDE_INSTALL_DIR=/usr/local/include
MAN_INSTALL_DIR=/usr/local/share/man/man3

# [internal]
BUILD_DIR=./build/lib
INCLUDE_DIR=./src/lib
MAN_DIR=./doc/roff/man3

LIBRARY=libcmore.so
HEADER=libcmore.h


#check that the library has been built
if [ ! -f "build/lib/libcmore.so" ]; then
    echo "'libcmore.so' build artifact missing. Please remember to run 'make lib'."
    exit 1
fi

#install files
if [ "$1" == "uninstall" ]; then
    rm -f ${INSTALL_DIR}/${LIBRARY}
    rm -f ${INCLUDE_INSTALL_DIR}/${HEADER}
    rm -f ${MAN_INSTALL_DIR}/libcmore_*
else
    cp ${BUILD_DIR}/${LIBRARY} ${INSTALL_DIR}
    cp ${INCLUDE_DIR}/${HEADER} ${INCLUDE_INSTALL_DIR}
    mkdir -p ${MAN_INSTALL_DIR}
    cp -R ${MAN_DIR}/* ${MAN_INSTALL_DIR}
fi
