CC	:= gcc
BIN	:= astrac
SRC	:= $(shell find src -name "*.c")
INCLUDE	:= -Iinclude
CFLAGS	:= -Wall -Wextra -pedantic -ggdb -std=c2x

all:
	$(CC) $(INCLUDE) $(CFLAGS) -o $(BIN) $(SRC)
