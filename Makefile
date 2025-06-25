# Default to release build
PROG=          buffy
TEST_PROG= 	buffy-unittest
TEST_CFLAGS=        -g -D__UNIT_TEST__ -Wall
TEST_LDFLAGS=      -Wl,-L/usr/local/lib,-lcunit
TEST_SRCS=	unittest/unittest.c
SRCS=          buffy.c gamestate.c 
MAN=           buffy.1 
BINOWN=        root
BINMODE=       555

# Include all headers from local dir and /usr/local
CPPFLAGS+=     -I${.CURDIR} -I/usr/local/include

buffy: ${SRCS}
	cc -g -o ${PROG} -Wall ${SRCS}

clean: 
	rm -rf *.dSYM *.o buffy

buffy-unittest: ${TEST_PROG}
	LDFLAGS="${TEST_LDFLAGS}" cc -g ${TEST_CFLAGS} ${CPPFLAGS} -o ${TEST_PROG} -Wall ${SRCS} ${TEST_SRCS}
