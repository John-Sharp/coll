CC = gcc

CFLAGS = -g -Wall -lm -lSDL2 -lSDL2_image

all: generateListCode engines ballInBox controlledBall 

ballInBox: generateListCode engines ballInBox.c
	$(CC) ballInBox.c coll.o engine.o jTypes.o inputProcessor.o  $(CFLAGS) -o ballInBox

controlledBall: generateListCode engines controlledBall.c
	$(CC) controlledBall.c coll.o engine.o jTypes.o inputProcessor.o  $(CFLAGS) -o controlledBall

engines: ../coll.h ../coll.c ../../jTypes/jTypes.h ../../jTypes/jTypes.c ../../engine/engine.c ../../engine/engine.h \
	../../inputProcessor/inputProcessor.c ../../inputProcessor/inputProcessor.h
	$(CC) -c ../coll.c ../../jTypes/jTypes.c ../../engine/engine.c ../../inputProcessor/inputProcessor.c # $(CFLAGS)

generateListCode: ../../dataStructures/templates/listTemplate.h ../../dataStructures/templates/listTemplate.inc
	../../dataStructures/generateList.py ../../engine/listHeaders ../../engine/listCode actor:actor preLogicCallBack:preLogicCallBack

clean:
	rm -f *.o
