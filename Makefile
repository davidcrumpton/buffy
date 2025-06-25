# Default to release build
PROG=          buffy
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

# If running unit tests, change binary name, sources, flags
#.if defined(UNITTEST)
#PROG=          main-unittest
#SRCS=          main.c
#CFLAGS+=        -g -D__UNIT_TEST__ -Wall
#LDADD+=        -L/usr/local/lib -lcunit
#.else
#.endif

#.include <bsd.prog.mk>
