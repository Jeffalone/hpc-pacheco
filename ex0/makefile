BIN := bin
SRC := src
CC := gcc
LDFLAGS := -lm

SRCS := $(wildcard $(SRC)/*.c)
BINS := $(patsubst $(SRC)/%.c, $(BIN)/%, $(SRCS))

$(BIN)/%: $(SRC)/%.c
	$(CC) $< -o $@ $(LDFLAGS)

all: setup $(BINS)  

setup: mv.txt
	mkdir $(BIN)
	cp mv.txt $(BIN)/

clean:
	rm -rf $(BIN)

.PHONY: all clean setup
