.RECIPEPREFIX:=>

#TODO [set as required] TODO
CC=gcc
CFLAGS=-ggdb -Wall -fPIC

LIB_DIR="./src/lib"
TEST_DIR="./src/test"

BUILD_DIR=$(shell pwd)/build


#[set build options]
ifeq ($(build),debug)
	CFLAGS += -O0
else
	CFLAGS += -O3
endif


#[process targets]
test: lib
> $(MAKE) -C ${TEST_DIR} test CC='${CC}' BUILD_DIR='${BUILD_DIR}'

lib:
> $(MAKE) -C ${LIB_DIR} lib CC='${CC}' CFLAGS='${CFLAGS}' BUILD_DIR='${BUILD_DIR}'

clean:
> $(MAKE) -C ${TEST_DIR} clean_all CC='${CC}' BUILD_DIR='${BUILD_DIR}'
> $(MAKE) -C ${LIB_DIR} clean_all CC='${CC}' CFLAGS='${CFLAGS}' BUILD_DIR='${BUILD_DIR}'
