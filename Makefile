CC      ?= cc
CFLAGS  ?= -O2 -Wall
LDFLAGS ?= #-L/opt/homebrew/opt/ncurses/lib
LDLIBS  ?= -lncurses

#CFLAGS += -I/opt/homebrew/opt/ncurses/include

TARGET = nav

all: $(TARGET)

$(TARGET): nav.c draw.c load_files.c loop.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

clean:
	rm -f $(TARGET)