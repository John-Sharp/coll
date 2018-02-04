CC = gcc

CFLAGS = -g -Wall -lm -lSDL2 -lSDL2_image

all: generateListCode ballInBox.c ../coll.h ../coll.c ../../jTypes/jTypes.h ../../jTypes/jTypes.c ../../engine/engine.c ../../engine/engine.h \
	../../inputProcessor/inputProcessor.c ../../inputProcessor/inputProcessor.h
	$(CC) ballInBox.c ../coll.c ../../jTypes/jTypes.c ../../engine/engine.c ../../inputProcessor/inputProcessor.c $(CFLAGS) -o ballInBox

generateListCode: ../../dataStructures/templates/listTemplate.h ../../dataStructures/templates/listTemplate.inc
	../../dataStructures/generateList.py ../../engine/listHeaders ../../engine/listCode actor:actor preLogicCallBack:preLogicCallBack

clean:
	rm -f *.o
