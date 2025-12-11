CC       := gcc
CFLAGS   := -std=c99 -O2 -Wall -Wextra -Wpedantic -Iinclude

LIBNAME  := libcbst.a

SRC      := src/bst.c
OBJ      := src/bst.o

.PHONY: all clean example test

all: $(LIBNAME) example test

$(LIBNAME): $(OBJ)
	ar rcs $@ $^

src/%.o: src/%.c include/bst.h
	$(CC) $(CFLAGS) -c $< -o $@

example: $(LIBNAME) examples/main.c
	$(CC) $(CFLAGS) examples/main.c $(LIBNAME) -o examples/main.exe

test: $(LIBNAME) tests/test_bst.c
	$(CC) $(CFLAGS) tests/test_bst.c $(LIBNAME) -o tests/test_bst.exe

clean:
	rm -f src/*.o $(LIBNAME) examples/*.exe tests/*.exe