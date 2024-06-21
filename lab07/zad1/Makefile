CC=gcc
CFLAGS = -Wall -g

all: client serwer

client: client.c
	$(CC) $(CFLAGS) client.c -o client_program

serwer: serwer.c
	$(CC) $(CFLAGS) serwer.c -o serwer_program

clean:
	rm client_program
	rm serwer_program
