# BSD Zero Clause License
#
# Copyright (c) 2025 David M Crumpton david.m.crumpton [at] gmail [dot] com
#
# Permission to use, copy, modify, and/or distribute this software for any
# purpose with or without fee is hereby granted.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
# REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
# AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
# INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
# LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
# OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
# PERFORMANCE OF THIS SOFTWARE.

# Default to release build
PROG            = buffy
TEST_PROG       = buffy-unittest
MAN             = buffy.6
INSTALLPATH     = /usr/local/bin
MANPATH         = /usr/local/man/man6
BINOWN          = root
BINMODE         = 555

# Compiler and flags
CC              = cc
CFLAGS          = -Wall -O2
TEST_CFLAGS     = -g -D__UNIT_TEST__ -Wall
CPPFLAGS        = -I. -I/usr/local/include
LDFLAGS         = -lncurses
TEST_LDFLAGS    = -L/usr/local/lib -lcunit -lncurses

# Source and object files
SRCS            = buffy.c gamestate.c fangs.c playerio.c patient.c diagnostic.c
OBJS            = $(SRCS:.c=.o)
HDRS            = buffy.h gamestate.h fangs.h playerio.h patient.h diagnostic.h

# Targets
all: $(PROG) $(TEST_PROG)

$(PROG): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS)

$(TEST_PROG): $(OBJS)
	$(CC) $(TEST_CFLAGS) $(CPPFLAGS) $(TEST_LDFLAGS) -o $@ $(SRCS)

%.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(PROG) $(TEST_PROG) *.dSYM *.BAK

install:
	install -m $(BINMODE) -o $(BINOWN) $(PROG) $(INSTALLPATH)/$(PROG)
	install -m 444 $(MAN) $(MANPATH)/$(MAN)

.PHONY: all clean install
