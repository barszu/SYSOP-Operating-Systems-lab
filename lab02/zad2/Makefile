# z uzyciem materialow z upela, komentarze przeklejone z upela
CC = gcc
CFLAGS = -Wall -g

all:
	make static shared

static:
	$(CC) $(CGLAGS) -c collatz.c -o collatz.o
	ar rcs libcollatz.a collatz.o

# (-fPIC position independent code, potrzebny do kodu biblioteki współdzielonej)

shared:
	$(CC) $(CFLAGS) -fPIC -c collatz.c -o collatz.o
	$(CC) $(CFLAGS) -shared -o libcollatz.so collatz.o

clean:
	rm -rf *.o *.a *.so 