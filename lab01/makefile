CC = gcc
CFLAGS = -Wall -std=c17 -g

.PHONY: all countdown clean

all: countdown

countdown: ./countdown.c
	$(CC) $(CFLAGS) countdown.c -o countdown 

clean:
	rm countdown
