CC = gcc

CFLAGS = -g -Wall -lm -lSDL2 -lSDL2_image

all: ballInBox.c ../coll.h ../coll.c ../../jTypes/jTypes.h ../../jTypes/jTypes.c
	$(CC) ballInBox.c ../coll.c ../../jTypes/jTypes.c $(CFLAGS) -o ballInBox

clean:
	rm -f *.o
