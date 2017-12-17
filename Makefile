default:
	gcc -Wall -c enigma.c && gcc -Wall -c interface.c && gcc -Wall -o enigma enigma.c interface.c 

mac:
	mkdir Mac_Release && gcc -Wall -c enigma.c && gcc -Wall -c interface.c && gcc -Wall -o enigma enigma.c interface.c && mv enigma.o Mac_Release/enigma.o && mv interface.o Mac_Release/interface.o && mv enigma Mac_Release/enigma

arm:
	mkdir ARM_Linux_Release && gcc -Wall -c enigma.c && gcc -Wall -c interface.c && gcc -Wall -o enigma enigma.c interface.c && mv enigma.o ARM_Linux_Release/enigma.o && mv interface.o ARM_Linux_Release/interface.o && mv enigma ARM_Linux_Release/enigma