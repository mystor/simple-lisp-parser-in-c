CC=clang

parse.out: parse.o
	$(CC) -o parse.out parse.o

.PHONY: run
run: parse.out
	./parse.out
