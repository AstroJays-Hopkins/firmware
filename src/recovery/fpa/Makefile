CC = gcc
CFLAGS = -std=c99 -pedantic -Wall -Wextra

# create executable
test: test.o fpa.o
	$(CC) test.o fpa.o -o test

test.o: test.c
	$(CC) $(CFLAGS) -c test.c

fpa.o: fpa.c
	$(CC) $(CFLAGS) -c fpa.c

clean:
	rm test *.o
