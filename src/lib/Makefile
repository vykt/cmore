.RECIPEPREFIX:=>

# This makefile takes the following variables:
#
#	CC        - Compiler.
#	CFLAGS    - Compiler flags.
#   BUILD_DIR - Base build directory.

SOURCES_LIB=list.c vector.c error.c
HEADERS_LIB=list.h vector.h error.h libcmore.h
OBJECTS_LIB=${SOURCES_LIB:.c=.o}

LIB=libcmore.so

lib: ${LIB}
> mkdir -p ${BUILD_DIR}/lib
> mv ${LIB} ${BUILD_DIR}/lib

${LIB}: ${OBJECTS_LIB}
> ${CC} ${CFLAGS} -shared -o ${LIB} ${OBJECTS_LIB} ${HEADERS_LIB}

${OBJECTS_LIB}: ${SOURCES_LIB} ${HEADERS_LIB}
> ${CC} ${CFLAGS} -c ${SOURCES_LIB}

clean_all: clean_src clean_build

clean_src:
> -rm -f *.o

clean_build:
> -rm ${BUILD_DIR}/lib/${LIB}
