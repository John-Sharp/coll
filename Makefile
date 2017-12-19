test: generateListCode testColl.c coll.c coll.h
	gcc testColl.c -lm -o test

generateListCode: templates/listTemplate.h templates/listTemplate.c
	sed templates/listTemplate.h -e "s/<pf>/jcObject/g" -e "s/<tp>/jcObject/g" > listHeaders/jcObjectList.h
	sed templates/listTemplate.c -e "s/<pf>/jcObject/g" -e "s/<tp>/jcObject/g" > listCode/jcObjectList.c
	sed templates/listTemplate.h -e "s/<pf>/jcPairing/g" -e "s/<tp>/jcPairing/g" > listHeaders/jcPairingList.h
	sed templates/listTemplate.c -e "s/<pf>/jcPairing/g" -e "s/<tp>/jcPairing/g" > listCode/jcPairingList.c
	sed templates/listTemplate.h -e "s/<pf>/jcRegisteredCollHandler/g" -e "s/<tp>/jcRegisteredCollHandler/g" > listHeaders/jcRegisteredCollHandlerList.h
	sed templates/listTemplate.c -e "s/<pf>/jcRegisteredCollHandler/g" -e "s/<tp>/jcRegisteredCollHandler/g" > listCode/jcRegisteredCollHandlerList.c
