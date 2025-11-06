
# BSD Zero Clause License

# Copyright (c) 2025 David M Crumpton david.m.crumpton [at] gmail [dot] com

# Permission to use, copy, modify, and/or distribute this software for any
# purpose with or without fee is hereby granted.

# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
# REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
# AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
# INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
# LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
# OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
# PERFORMANCE OF THIS SOFTWARE.
# 
# Default to release build
PROG=          buffy
LDFLAGS=        -lncurses
INSTALLPATH=    /usr/local/bin
MANPATH=       /usr/local/man/man6
TEST_PROG=      buffy-unittest
TEST_CFLAGS=        -g -D__UNIT_TEST__ -Wall
TEST_LDFLAGS=      -L/usr/local/lib -lcunit -lncurses

SRCS=           buffy.c gamestate.c fangs.c playerio.c patient.c diagnostic.c
OBJS=           buffy.o gamestate.o fangs.o playerio.o patient.o diagnostic.o
HDRS=           buffy.h gamestate.h fangs.h playerio.h patient.h diagnostic.h
TEST_SRCS=      unittest/unittest.c
MAN=           buffy.6
BINOWN=        root
BINMODE=       555

# Include all headers from local dir and /usr/local
CPPFLAGS+=     -I${.CURDIR} -I/usr/local/include

$(PROG): $(OBJS)
		cc $(LDFLAGS) -o $@ $(OBJS)

%.o: %.c %.h
		cc -c -Wall ${CFLAGS} ${CPPFLAGS} $<

clean:
		rm -rf -- *.dSYM *.o buffy buffy-unittest *.BAK

.PHONY: all clean buffy unittest beautify
all: buffy buffy-unittest

buffy-unittest: ${TEST_PROG} ${TEST_SRCS}
		cc -g ${TEST_LDFLAGS} ${TEST_CFLAGS} ${CPPFLAGS} -o ${TEST_PROG} -Wall ${SRCS}

install:
	install -m 555 buffy $(INSTALLPATH)/buffy
	install -m 444 buffy.6 $(MANPATH)/buffy.6
	chown $(BINOWN) $(INSTALLPATH)/buffy
