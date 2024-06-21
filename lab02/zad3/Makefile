# z uzyciem materialow z upela, komentarze przeklejone z upela
CC = gcc
CFLAGS = -Wall -g

#bez clienta
all: static shared client

#uruchomienie
# ./program

client:
	$(CC) $(CFLAGS) -c client.c -o client.o

static:
	$(CC) $(CGLAGS) -c collatz.c -o collatz.o
	ar rcs libcollatz.a collatz.o

client_static: static client
	$(CC) client.o -o program -L. -lcollatz

# (-fPIC position independent code, potrzebny do kodu biblioteki współdzielonej)

shared:
	$(CC) $(CFLAGS) -fPIC -c collatz.c -o collatz.o
	$(CC) $(CFLAGS) -shared -o libcollatz.so collatz.o

client_shared: shared
	$(CC) client.c -o program -L. -lcollatz -Wl,-rpath,.



client_dynamic: shared
	$(CC) $(CFLAGS) client.c -o program -ldl -D DYNAMIC

clean:
	rm -rf *.o *.a *.so program