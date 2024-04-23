CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Werror -g -I./include
CLIBS = -lcap

BIN = docky
OUTDIR = dist
SRCDIR = src
SRC = $(wildcard $(SRCDIR)/*.c)
OBJ = $(SRC:$(SRCDIR)/%.c=$(OUTDIR)/%.o)
DEPS = $(wildcard include/*.h)

$(shell mkdir -p $(OUTDIR))

# $@: target
# $^: all dependencies
# $<: first dependency

$(OUTDIR)/$(BIN): $(OBJ)
	$(CC) -o $@ $^ $(CLIBS)
	sudo setcap cap_sys_admin+ep $@

$(OUTDIR)/%.o: $(SRCDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: clean

clean:
	rm -rf $(OUTDIR)
