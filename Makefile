CC      ?= gcc
CFLAGS  ?= -O2 -Wall
LDFLAGS ?=
LDLIBS  ?= -lncurses

TARGET  = nav
SRC     = nav.c draw.c load_files.c loop.c

VERSION ?= $(shell git describe --tags --always 2>/dev/null || echo "0.0.0")
CFLAGS  += -DVERSION="\"$(VERSION)\""

PREFIX  ?= /usr/local
BINDIR  := $(PREFIX)/bin

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(LDFLAGS) $(SRC) -o $(TARGET) $(LDLIBS)

install: $(TARGET)
	mkdir -p "$(BINDIR)"
	cp "$(TARGET)" "$(BINDIR)/"

clean:
	rm -f $(TARGET)

.PHONY: all install clean