.RECIPEPREFIX:=>

#TODO [set as required] TODO
CC=gcc
CFLAGS=-ggdb -Wall

LIB_DIR="./src/lib"
TEST_DIR="./src/test"

BUILD_DIR=$(shell pwd)/build


#[set build options]
ifeq ($(build),debug)
	CFLAGS += -O0 -fsanitize=address
else
	CFLAGS += -O3
endif


#[process targets]
test: lib
> $(MAKE) -C ${TEST_DIR} test CC='${CC}' BUILD_DIR='${BUILD_DIR}/test'

all: shared static

shared:
> $(MAKE) -C ${LIB_DIR} shared CC='${CC}' CFLAGS='${CFLAGS} -fPIC' \
	                                      BUILD_DIR='${BUILD_DIR}/lib'

static:
> $(MAKE) -C ${LIB_DIR} static CC='${CC}' CFLAGS='${CFLAGS}' \
	                                      BUILD_DIR='${BUILD_DIR}/lib'

clean:
> $(MAKE) -C ${TEST_DIR} clean CC='${CC}' BUILD_DIR='${BUILD_DIR}/test'
> $(MAKE) -C ${LIB_DIR} clean CC='${CC}'CFLAGS='${CFLAGS}'\
	                                    BUILD_DIR='${BUILD_DIR}/lib'
