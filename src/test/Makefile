.RECIPEPREFIX:=>

# This makefile takes the following variables:
#
# CC        - Compiler.
# BUILD_DIR - Unit test build directory.

CFLAGS:=-ggdb 
LDFLAGS:= -lcheck

SOURCES_TEST=check_vectpr.c
HEADERS_TEST=test_data.h
OBJECTS_TEST=${SOURCES_LIB:%.c=${BUILD_DIR}/$.o}

UNIT_TESTS=unit_tests


test: ${UNIT_TESTS}
> mkdir -p ${BUILD_DIR}
> mv ${SHARED} ${BUILD_DIR}

${UNIT_TESTS}: ${OBJECTS_TEST}
> ${CC} ${CFLAGS} -o $@

${BUILD_DIR}/%.o: %.c
> ${CC} ${CFLAGS} -c $< -o $@

clean:
> -rm -v ${BUILD_DIR}/{*.o,${UNIT_TESTS}}
