CC=gcc
CFLAGS = -Wall -g

reverse: reverse.c
	$(CC) $(CFLAGS) reverse.c -o program

reverse_bytes: reverse.c
	$(CC) $(CFLAGS) reverse.c -o program -D BYTES_ONLY

clean: 
	rm program input.txt output.txt

dummy_data:
	echo "some dummy data 12345678" > input.txt
	touch output.txt

run:
	./program input.txt output.txt

