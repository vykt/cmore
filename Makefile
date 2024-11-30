.RECIPEPREFIX:=>

#TODO [set as required] TODO
CC=gcc
CFLAGS=
CFLAGS_DBG=-ggdb -O0
WARN_OPTS=-Wall -Wextra
LDFLAGS=

LIB_DIR="./src/lib"
TEST_DIR="./src/test"

BUILD_DIR=$(shell pwd)/build


#[set build options]
ifeq ($(build),debug)
	CFLAGS     += -O0 -ggdb -fsanitize=address -DDEBUG
	CFLAGS_DBG += -DDEBUG
	LDFLAGS    += -static-libasan
else
	CFLAGS += -O3
endif


#[process targets]
test: shared
> $(MAKE) -C ${TEST_DIR} test CC='${CC}' _CFLAGS='${CFLAGS_DBG}' \
		                                 _WARN_OPTS='${WARN_OPTS}' \
										 BUILD_DIR='${BUILD_DIR}/test' \
                                         LIB_BIN_DIR='${BUILD_DIR}/lib'

all: shared static

shared:
> $(MAKE) -C ${LIB_DIR} shared CC='${CC}' _CFLAGS='${CFLAGS} -fPIC' \
	                                      _WARN_OPTS='${WARN_OPTS}' \
										  _LDFLAGS='${LDFLAGS}' \
	                                      BUILD_DIR='${BUILD_DIR}/lib'

static:
> $(MAKE) -C ${LIB_DIR} static CC='${CC}' _CFLAGS='${CFLAGS}' \
	                                      _WARN_OPTS='${WARN_OPTS}' \
	                                      _LDFLAGS='${LDFLAGS}' \
	                                      BUILD_DIR='${BUILD_DIR}/lib'

clean:
> $(MAKE) -C ${TEST_DIR} clean CC='${CC}' BUILD_DIR='${BUILD_DIR}/test'
> $(MAKE) -C ${LIB_DIR} clean CC='${CC}' BUILD_DIR='${BUILD_DIR}/lib'
