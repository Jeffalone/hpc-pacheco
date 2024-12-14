# Makefile for simple hammer applications

# You can change these.

TARGET="ms"

CC=mpicc
CFLAGS=-O -Wall -std=c99 -g
LDFLAGS=

# You shouldn't change these unless you really know what you're doing.

SRC=$(wildcard *.c)
OBJ=$(SRC:.c=.o)
DEP=$(OBJ:.o=.d)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

-include $(DEP)

%d: %c
	@$(CPP) $(CFLAGS) $< -MM -MT $(@:.d=.o) >$@

.PHONY: clean
clean:
	rm -f $(TARGET) *.[oadD] *.[mM][oO][dD]

.PHONY: cleandep
cleandep:
	rm -rf $(DEP)

.PHONY: cleanlog
cleanlog:
	rm -rf job.*.{err,out}

.PHONY: cleanall
cleanall: clean cleandep cleanlog
