CC=gcc
CFLAGS=-Wall -Wextra -std=c99

build: 2048

2048: 2048.c
	$(CC) $(CFLAGS) 2048.c -o 2048.out -lncurses
run:
	./2048.out
clean:
	rm -f 2048.out