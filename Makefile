
CC=gcc
CFLAGS=-std=c99 -Wall -Wextra -Wpedantic -ggdb3
LDLIBS=-lm -lpng

OBJ_FILES=main.c.o args.c.o read_file.c.o image.c.o util.c.o liblog.c.o
OBJS=$(addprefix obj/, $(OBJ_FILES))

BIN=bin-graph
INSTALL_DIR=/usr/local/bin

#-------------------------------------------------------------------------------

.PHONY: all clean install

all: $(BIN)

clean:
	rm -f $(OBJS)
	rm -f $(BIN)

install: $(BIN)
	mkdir -p $(INSTALL_DIR)
	install -m 755 $^ $(INSTALL_DIR)

#-------------------------------------------------------------------------------

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

obj/%.c.o : src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -o $@ -c $<
