.RECIPEPREFIX:=>

# This makefile takes the following variables:
#
# CC          - Compiler.
# BUILD_DIR   - Unit test build directory.
# LIB_BIN_DIR - Library artifact directory.

CFLAGS:= -O0 -ggdb -Wall -fsanitize=address
LDFLAGS:= -L${LIB_BIN_DIR} -Wl,-rpath=${LIB_BIN_DIR} -lcmore -lcheck -static-libasan
WARN_OPTS:= -Wno-unused-variable -Wno-unused-but-set-variable

SOURCES_TEST=main.c check_list.c check_vector.c check_rb_tree.c
OBJECTS_TEST=${SOURCES_TEST:%.c=${BUILD_DIR}/%.o}

UNIT_TESTS=unit_tests


test: ${UNIT_TESTS}
> mkdir -p ${BUILD_DIR}
> mv ${UNIT_TESTS} ${BUILD_DIR}

${UNIT_TESTS}: ${OBJECTS_TEST}
> ${CC} ${CFLAGS} -o $@ $^ ${LDFLAGS}

${BUILD_DIR}/%.o: %.c
> ${CC} ${CFLAGS} -c $< -o $@ ${WARN_OPTS}

clean:
> -rm -v ${BUILD_DIR}/{*.o,${UNIT_TESTS}}
