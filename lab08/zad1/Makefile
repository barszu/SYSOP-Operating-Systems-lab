CC=gcc
CFLAGS = -Wall -g

all: printers users

printers: printers.c
	$(CC) $(CFLAGS) printers.c -o program_printers -lpthread

users: users.c
	$(CC) $(CFLAGS) users.c -o program_users -lpthread


clean:
	rm program_printers
	rm program_users


