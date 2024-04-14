CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c99 -pedantic -g
CLIBS = -lm

BIN = docky
OUTDIR = dist
SRCDIR = src
SRC = $(wildcard $(SRCDIR)/*.c)
OBJ = $(SRC:$(SRCDIR)/%.c=$(OUTDIR)/%.o)

$(shell mkdir -p $(OUTDIR))

# $@: target
# $^: all dependencies
# $<: first dependency

$(OUTDIR)/$(BIN): $(OBJ)
	$(CC) -o $@ $^ $(CLIBS)

$(OUTDIR)/%.o: $(SRCDIR)/%.c
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: clean

clean:
	rm -rf $(OUTDIR)
