CC = gcc
CFLAGS = -O2 -Wall -Wextra -Werror
PROGRAM = intelgpio
INSTALL = /usr/bin/env install
PREFIX	= /usr/local

all: $(PROGRAM)

$(PROGRAM): intelgpio.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

install: $(PROGRAM)
	$(INSTALL) $(PROGRAM) $(PREFIX)/sbin

clean:
	rm -f *.o $(PROGRAM)

distclean: clean

%.o: %.c
	$(CC) $(CFLAGS) -c $^ -I. -o $@

.PHONY: all install clean distclean
