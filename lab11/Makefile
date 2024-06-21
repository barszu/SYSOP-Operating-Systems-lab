CC = gcc
CFLAGS = -Wall

all: serwer client

serwer: serwer.c
	$(CC) $(CFLAGS) serwer.c -o serwer -pthread

client: client.c
	$(CC) $(CFLAGS) client.c -o client -pthread


run_serwer: serwer
	./serwer 127.0.0.1 8080

run_client1: client
	./client client1 127.0.0.1 8080

run_client2: client
	./client client2 127.0.0.1 8080
	
clean:
	rm serwer client