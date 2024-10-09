#!/bin/bash

#TODO [set as required] TODO
INSTALL_DIR=/usr/local/lib
INCLUDE_INSTALL_DIR=/usr/local/include
MAN_INSTALL_DIR=/usr/local/share/man/man3
MD_INSTALL_DIR=/usr/local/share/doc/libcmore

# [internal]
BUILD_DIR=./build/lib
INCLUDE_DIR=./src/lib
MAN_DIR=./doc/roff/man3
MD_DIR=./doc/md

LIBRARY=libcmore.so
HEADER=libcmore.h


#uninstall
if [ "$1" == "uninstall" ]; then
    rm -vf ${INSTALL_DIR}/${LIBRARY}
    rm -vf ${INCLUDE_INSTALL_DIR}/${HEADER}
    rm -vf ${MAN_INSTALL_DIR}/libcmore_*
    rm -vf ${MD_INSTALL_DIR}/*
    rmdir ${MD_INSTALL_DIR}
    exit 0

#install md docs
elif [ "$1" == "install_doc" ]; then
    mkdir -pv ${MD_INSTALL_DIR}
    cp -v ${MD_DIR}/* ${MD_INSTALL_DIR}
    exit 0
fi


#check that the library has been built
if [ ! -f "build/lib/libcmore.so" ]; then
    echo "'libcmore.so' build artifact missing. Please remember to run 'make lib'."
    exit 1
fi

#install files
mkdir -pv ${INSTALL_DIR}
cp -v ${BUILD_DIR}/${LIBRARY} ${INSTALL_DIR}
mkdir -pv ${INCLUDE_INSTALL_DIR}
cp -v ${INCLUDE_DIR}/${HEADER} ${INCLUDE_INSTALL_DIR}
mkdir -pv ${MAN_INSTALL_DIR}
cp -v -R ${MAN_DIR}/* ${MAN_INSTALL_DIR}
exit 0
