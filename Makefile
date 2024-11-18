.RECIPEPREFIX:=>

#TODO [set as required] TODO
CC=gcc
CFLAGS=-ggdb -Wall -Wextra

LIB_DIR="./src/lib"
TEST_DIR="./src/test"

BUILD_DIR=$(shell pwd)/build


#[set build options]
ifeq ($(build),debug)
	CFLAGS += -O0 -fsanitize=address -DDEBUG
	LDFLAGS += -static-libasan
else
	CFLAGS += -O3
endif


#[process targets]
test: shared
> $(MAKE) -C ${TEST_DIR} test CC='${CC}' BUILD_DIR='${BUILD_DIR}/test' \
                                         LIB_BIN_DIR='${BUILD_DIR}/lib'

all: shared static

shared:
> $(MAKE) -C ${LIB_DIR} shared CC='${CC}' CFLAGS='${CFLAGS} -fPIC' \
	                                      LDFLAGS='${LDFLAGS}' \
	                                      BUILD_DIR='${BUILD_DIR}/lib'

static:
> $(MAKE) -C ${LIB_DIR} static CC='${CC}' CFLAGS='${CFLAGS}' \
	                                      LDFLAGS='${LDFLAGS}' \
	                                      BUILD_DIR='${BUILD_DIR}/lib'

clean:
> $(MAKE) -C ${TEST_DIR} clean CC='${CC}' BUILD_DIR='${BUILD_DIR}/test'
> $(MAKE) -C ${LIB_DIR} clean CC='${CC}'CFLAGS='${CFLAGS}'\
	                                    BUILD_DIR='${BUILD_DIR}/lib'
