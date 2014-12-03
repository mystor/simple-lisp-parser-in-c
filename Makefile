CC=clang
CFLAGS=-Wall

parse.out: parse.o runtime.o
	$(CC) -o parse.out parse.o runtime.o -lGC

.PHONY: run
run: parse.out
	./parse.out
