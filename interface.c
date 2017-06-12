#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <stdbool.h>
#include <stdint.h>
#include <assert.h>

#include "enigma.h"

#define CONF "enigma.enig"

void generateFile(void);
void loadSettings(enigma *e);


int main() {
	//enigma* enig = malloc(sizeof(enigma));
	//loadSettings(enig);

	printf("________\n| ENIGMA |\n________\n\n");
	//printf("[A]%20s\n[B]%20s\n[C]%20s\n");
	generateFile();
	return 0;
}

void loadSettings(enigma* enig) {
	//Create default file if it does not exist	
	if(access("Files/enigma.enig", F_OK) == -1) {
		generateFile();	
	}	
	
	FILE *conf = fopen(CONF, "r");
		fscanf(conf, "%s %s %hhu %hhu %s %hhu %hhu %s %hhu %hhu %s", 
			&enig->wiring[0], 
			&enig->reflector[0],
			&enig->parts[0].set, &enig->parts[0].turnPoint,&enig->parts[0].alph[0],	
			&enig->parts[1].set, &enig->parts[1].turnPoint,&enig->parts[1].alph[0],
			&enig->parts[2].set, &enig->parts[2].turnPoint,&enig->parts[2].alph[0]);	
	fclose(conf);
}
/*Automatically generates a enigma.eng file using a default setting*/
void generateFile() {
	FILE *conf = fopen(CONF, "w");	
	fprintf(conf, "%s\n%s\n%d\n%d\n%s\n%d\n%d\n%s\n%d\n%d\n%s\n","QWERTYUIOPASDFGHJKLZ", UKW_B,
				5, 21, ROTOR_III, 21, 4, ROTOR_II, 16, 16, ROTOR_I);
	fclose(conf);
	
}
