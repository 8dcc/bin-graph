
CC=gcc
CPPFLAGS=-DBIN_GRAPH_HEATMAP
CFLAGS=-std=c99 -Wall -Wextra -Wpedantic -ggdb3
LDLIBS=-lm -lpng

SRC=main.c args.c byte_array.c image.c util.c file.c generate_grayscale.c generate_ascii.c generate_entropy.c generate_entropy_histogram.c generate_histogram.c generate_bigrams.c generate_dotplot.c transform_squares.c transform_zigzag.c transform_hilbert.c export_png.c export_escaped_text.c
OBJ=$(addprefix obj/, $(addsuffix .o, $(SRC)))

BIN=bin-graph
COMPLETION=bin-graph-completion.bash

PREFIX=/usr/local
BINDIR=$(PREFIX)/bin
COMPLETIONDIR=$(PREFIX)/share/bash-completion/completions

#-------------------------------------------------------------------------------

.PHONY: all clean install install-bin install-completion

all: $(BIN)

clean:
	rm -f $(OBJ)
	rm -f $(BIN)

install: install-bin install-completion

install-bin: $(BIN)
	install -D -m 755 $^ -t $(DESTDIR)$(BINDIR)

install-completion: $(COMPLETION)
	install -D -m 644 $^ $(DESTDIR)$(COMPLETIONDIR)/$(BIN)

#-------------------------------------------------------------------------------

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ $^ $(LDLIBS)

obj/%.c.o : src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ -c $<
