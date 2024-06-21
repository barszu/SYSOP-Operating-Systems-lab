CC=gcc
CFLAGS = -Wall -g


all: catcher sender

catcher: catcher.c
	$(CC) $(CFLAGS) catcher.c -o catcher

sender: sender.c
	$(CC) $(CFLAGS) sender.c -o sender

clean:
	rm sender
	rm catcher