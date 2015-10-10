PREFIX = /usr/local
SRC = x-session.c 
X11SRC = x-move-resize.c x-alt-tab.c x-focus-manager.c x-placement-manager.c x-undecorate.c x-window-list.c
SCRIPTS= scripts/x-banish scripts/x-root-clean scripts/x-launcher

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

clean:
	rm $(BIN) $(X11BIN) 

install: $(X11BIN)
	install -m 0755 $(BIN) $(X11BIN) $(SCRIPTS) ${PREFIX}/bin/.
