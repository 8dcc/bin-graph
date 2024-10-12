
CC=gcc
CFLAGS=-std=c99 -Wall -Wextra -Wpedantic -ggdb3
LDLIBS=-lm -lpng

SRCS=main.c args.c read_file.c image.c util.c liblog.c
OBJS=$(addprefix obj/, $(addsuffix .o, $(SRCS)))

BIN=bin-graph

PREFIX=/usr/local
BINDIR=$(PREFIX)/bin

#-------------------------------------------------------------------------------

.PHONY: all clean install

all: $(BIN)

clean:
	rm -f $(OBJS)
	rm -f $(BIN)

install: $(BIN)
	mkdir -p $(BINDIR)
	install -m 755 $^ $(BINDIR)

#-------------------------------------------------------------------------------

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

obj/%.c.o : src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -o $@ -c $<
