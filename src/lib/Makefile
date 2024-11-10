.RECIPEPREFIX:=>

# This makefile takes the following variables:
#
#	CC        - Compiler.
#	CFLAGS    - Compiler flags.
#   BUILD_DIR - Library build directory.

LDFLAGS:=

SOURCES_LIB=list.c vector.c error.c
HEADERS_LIB=list.h vector.h error.h libcmore.h
OBJECTS_LIB=${SOURCES_LIB:%.c=${BUILD_DIR}/%.o}

SHARED=libcmore.so
STATIC=libcmore.a


shared: ${SHARED}
> mkdir -p ${BUILD_DIR}
> mv ${SHARED} ${BUILD_DIR}

static: ${STATIC}
> mkdir -p ${BUILD_DIR}
> mv ${STATIC} ${BUILD_DIR}

${SHARED}: ${OBJECTS_LIB}
> ${CC} ${CFLAGS} -shared -o $@ $^ ${LDFLAGS}

${STATIC}: ${OBJECTS_LIB}
> ar rcs $@ $^

${BUILD_DIR}/%.o: %.c
> ${CC} ${CFLAGS} -c $< -o $@

clean:
> -rm -v ${BUILD_DIR}/{*.o,${SHARED},${STATIC}}
