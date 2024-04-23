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
# You can play with the capabilities to see what feature breaks
	sudo setcap cap_sys_admin,cap_sys_chroot+ep $@

$(OUTDIR)/%.o: $(SRCDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: clean

clean:
	rm -rf $(OUTDIR)
