
CC=gcc
CFLAGS=-std=c99 -Wall -Wextra -Wpedantic -ggdb3
LDLIBS=-lm -lpng

SRC=main.c args.c read_file.c image.c util.c liblog.c
OBJ=$(addprefix obj/, $(addsuffix .o, $(SRC)))

BIN=bin-graph

PREFIX=/usr/local
BINDIR=$(PREFIX)/bin

#-------------------------------------------------------------------------------

.PHONY: all clean install

all: $(BIN)

clean:
	rm -f $(OBJ)
	rm -f $(BIN)

install: $(BIN)
	mkdir -p $(BINDIR)
	install -m 755 $^ $(DESTDIR)$(BINDIR)

#-------------------------------------------------------------------------------

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

obj/%.c.o : src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -o $@ -c $<
