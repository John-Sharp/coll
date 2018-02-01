test: generateListCode testColl.c coll.c coll.h ../jTypes/jTypes.h ../jTypes/jTypes.c
	gcc testColl.c ../jTypes/jTypes.c -g -lm -o test

generateListCode: ../dataStructures/templates/listTemplate.h ../dataStructures/templates/listTemplate.inc
	../dataStructures/generateList.py listHeaders listCode jcObject:jcObject jcPairing:jcPairing jcRegisteredCollHandler:jcRegisteredCollHandler
