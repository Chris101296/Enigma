default:
	gcc -Wall -c enigma.c && gcc -Wall -c interface.c && gcc -Wall -o enigma enigma.c interface.c 