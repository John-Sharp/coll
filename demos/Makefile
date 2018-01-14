CC = gcc

CFLAGS = -g -Wall -lm -lSDL2 -lSDL2_image

all: ballInBox.c ../coll.h ../coll.c 
	$(CC) ballInBox.c ../coll.c $(CFLAGS) -o ballInBox

clean:
	rm -f *.o
