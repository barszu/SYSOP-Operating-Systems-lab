CC=gcc
CFLAGS = -Wall -g

all: child_procesess_maker.c
	$(CC) $(CFLAGS) child_procesess_maker.c -o program

run_dummy: all
	./program 5

clean:
	rm program