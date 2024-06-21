CC=gcc
CFLAGS = -Wall -g

all: main

main: main.c
	$(CC) $(CFLAGS) main.c -o program

clean:
	rm program
	rm my_fifo1
	rm my_fifo2

run: main
	./program 0.0 1.0 3