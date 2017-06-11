PREFIX = /usr/local
SRC = x-session.c
X11SRC = $(filter-out $(SRC),$(wildcard *.c))
SCRIPTS= $(wildcard scripts/*)

CC = gcc
CFLAGS = -O2 -Wall -std=c99 -pedantic
X11LIB = -lX11 -L/usr/X11/lib

BIN = ${SRC:.c=}
X11BIN = ${X11SRC:.c=}
all: $(BIN) $(X11BIN)

$(BIN): %: %.c
	$(CC) $(CFLAGS) $^ -o $@

$(X11BIN): %: %.c
	$(CC) $(CFLAGS) $(X11LIB) $^ -o $@

clean: dist
	@rm $(basename $(wildcard *.c)) $(wildcard *.c.orig)

dist:
	@astyle --style=google *.c

install: all
	install -m 0755 $(BIN) $(X11BIN) $(SCRIPTS) ${PREFIX}/bin/.
