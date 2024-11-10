#!/bin/bash

#TODO [set as required] TODO
INSTALL_DIR=/usr/local/lib
INCLUDE_INSTALL_DIR=/usr/local/include
MAN_INSTALL_DIR=/usr/local/share/man/man3
MD_INSTALL_DIR=/usr/local/share/doc/libcmore
LD_DIR=/etc/ld.so.conf.d

# [internal]
BUILD_DIR=./build/lib
INCLUDE_DIR=./src/lib
MAN_DIR=./doc/roff/man3
MD_DIR=./doc/md

SHARED=libcmore.so
STATIC=libcmore.a
HEADER=libcmore.h


#uninstall
if [ "$1" == "uninstall" ]; then
    rm -vf ${INSTALL_DIR}/{${SHARED},${STATIC}}
    rm -vf ${INCLUDE_INSTALL_DIR}/${HEADER}
    rm -vf ${MAN_INSTALL_DIR}/libcmore_*
    rm -vf ${MD_INSTALL_DIR}/*
    rmdir ${MD_INSTALL_DIR}
    rm ${LD_DIR}/90cmore.conf
    ldconfig
    exit 0

#install md docs
elif [ "$1" == "install_doc" ]; then
    mkdir -pv ${MD_INSTALL_DIR}
    cp -v ${MD_DIR}/* ${MD_INSTALL_DIR}
    exit 0
fi


#check that the library has been built
if [ ! -f "${BUILD_DIR}/${SHARED}" ] && [ ! -f "${BUILD_DIR}/${STATIC}"]; then
    echo "Build artifacts missing. Try running 'make all'."
    exit 1
fi

#install files
mkdir -pv ${INSTALL_DIR}
cp -v ${BUILD_DIR}/{${SHARED},${STATIC}} ${INSTALL_DIR}
mkdir -pv ${INCLUDE_INSTALL_DIR}
cp -v ${INCLUDE_DIR}/${HEADER} ${INCLUDE_INSTALL_DIR}
mkdir -pv ${MAN_INSTALL_DIR}
cp -v -R ${MAN_DIR}/* ${MAN_INSTALL_DIR}
echo "${INSTALL_DIR}" > ${LD_DIR}/90cmore.conf
ldconfig
exit 0
