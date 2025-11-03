CC      ?= gcc
CFLAGS  ?= -O2 -Wall -Wextra
LDFLAGS ?=
LDLIBS  ?= -lncurses

TARGET  = nav
SRC     = nav.c draw.c load_files.c loop.c
OBJ     = $(SRC:.c=.o)

VERSION ?= $(shell git describe --tags --always --dirty 2>/dev/null | sed 's/^v//' || echo "0.0.0")
CFLAGS  += -DVERSION="\"$(VERSION)\""

PREFIX  ?= /usr/local
BINDIR  := $(PREFIX)/bin

INSTALL ?= install

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

install: $(TARGET)
	$(INSTALL) -d "$(BINDIR)"
	$(INSTALL) -m 755 "$(TARGET)" "$(BINDIR)/"	

clean:
	rm -f $(TARGET) $(OBJ)

.PHONY: all install clean
