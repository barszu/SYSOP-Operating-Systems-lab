CC=gcc
CFLAGS = -Wall -g

# none, ignore, handler, mask

all: main.c
	$(CC) $(CFLAGS) main.c -o program

run_none: all
	./program none

run_ignore: all
	./program ignore

run_handler: all
	./program handler

run_mask: all
	./program mask


clean:
	rm program