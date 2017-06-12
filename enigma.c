#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <stdbool.h>
#include <stdint.h>
#include <assert.h>

#include "parts.h"

/*Function for normal stepping on the Enigma Machine*/
#define STEP(step) ((step + 1) % ALPH_LENGTH )
#define CASE_GAP  32	// Equivilant to 'a' - 'A' 

/*Steps through entire machine
*/
void stepMachine(enigma* enig) {

	bool midTurn = enig->parts[2].set == enig->parts[2].turnPoint;
	bool rightTurn = enig->parts[1].set == enig->parts[1].turnPoint;

	if(rightTurn) 
		enig->parts[0].set = STEP(enig->parts[0].set);
	
	if(midTurn || rightTurn)	// Watching for double step as well as normal step
		enig->parts[1].set = STEP(enig->parts[1].set);	
	
	enig->parts[2].set = STEP(enig->parts[2].set);
}

/**Simualtes the portion of encryption conducted by the wire board**/
char wireSwap(enigma* enig, char c) {
	for(int i = 0; i < WORD_COUNT; i++) {
		if(c == enig->wiring[i]) {
			c = enig->wiring[i + 1 - 2 * (i % 2)]; //Adds 1 for evens, 
										    	  //subtracts 1 for odds
			break;
		}
	}
	return c;
}

/** Encrypts a single character using the Enigma machine
	enig is a pointer to the enigma structure in use**/
char encrypt(enigma* enig, char c) {	
	if(c > 'Z' || c < 'A') {
		if(c >= 'a' && c <= 'z')
			return encrypt(enig, c - CASE_GAP);
		else
			return c;
	}
	stepMachine(enig);

	#ifdef DEBUG
		printf("%d|%d|%d\n",enig->parts[0].set, 
						   enig->parts[1].set, 
						   enig->parts[2].set);
	#endif

	//Begin Encryption
	c = wireSwap(enig, c);
	
	//Fast Rotor
	c -= 'A'; 						//Converts char to rotor notation (0 - 25)
	uint8_t index = (c + enig->parts[2].set) % ALPH_LENGTH; //index of letter to swap
	c =  enig->parts[2].alph[index] - 'A'; //get final result of swap
	#ifdef DEBUG
		printf("%d ", c);
	#endif
	
	//Rest of Rotors
	for(int i = PART_COUNT - 2; i > -1; i--) {
		index = (c + enig->parts[i].set - enig->parts[i + 1].set + ALPH_LENGTH) % ALPH_LENGTH;
		c = enig->parts[i].alph[index] - 'A'; //get final result of swap
		#ifdef DEBUG
			printf("%d ", c);
		#endif
	}
	
	//Reflector Part
	index = (c - enig->parts[0].set + ALPH_LENGTH) % ALPH_LENGTH;
	c = enig->reflector[index] - 'A';
    #ifdef DEBUG
		printf("%d ", c);
	#endif
	
	//Reverse
	c = (c + enig->parts[0].set) % ALPH_LENGTH + 'A';
	c = strchr(enig->parts[0].alph, c) - (char*)&enig->parts[0].alph;
    #ifdef DEBUG
		printf("%d ", c);
	#endif

	for(int i = 1; i < PART_COUNT; i++) {
		c = (c + enig->parts[i].set - enig->parts[i - 1]. set + ALPH_LENGTH) % ALPH_LENGTH;
		c = c + 'A';
		c = strchr(enig->parts[i].alph, c) - (char*)&enig->parts[i].alph;
	    #ifdef DEBUG
			printf("%d ", c);
		#endif
	}	 

	//Ready for Wire Board	
	c = (c - enig->parts[2].set + ALPH_LENGTH) % ALPH_LENGTH +'A';
	c = wireSwap(enig, c);
    #ifdef DEBUG
		printf("%c\n", c);
	#endif
	return c;
}

//TEST
int main() {
	printf("%d %d\n", sizeof(char), sizeof(uint8_t) );
	enigma* enig = malloc(sizeof(enigma));
	const char wires[ALPH_LENGTH]= "AAAAAAAAAAAAAAAAAAAA";
	memmove(enig->wiring, wires, ALPH_LENGTH); 
	
	const char r[ALPH_LENGTH] = UKW_B;
	memmove(enig->reflector, r, ALPH_LENGTH);

	const char r1 [ALPH_LENGTH] = ROTOR_I;
	const char r2 [ALPH_LENGTH] = ROTOR_II;
	const char r3 [ALPH_LENGTH] = ROTOR_III;
	
	memmove(enig->parts[2].alph, r1, ALPH_LENGTH);
	enig->parts[2].set = 16;
	enig->parts[2].turnPoint = TURN_I;

	memmove(enig->parts[1].alph, r2, ALPH_LENGTH);
	enig->parts[1].set = 21;
	enig->parts[1].turnPoint = TURN_II;

	memmove(enig->parts[0].alph, r3, ALPH_LENGTH);
	enig->parts[0].set = 5;
	enig->parts[0].turnPoint = TURN_III;

	printf("*******\n");
	for(int i = PART_COUNT - 1; i >= 0; i--) {
		printf("ROTOR %d: |%s| %d TURN: %d\n", i, enig->parts[i].alph, enig->parts[i].set, enig->parts[i].turnPoint );
	}
	printf("UKW: %s\nWIRING%s\n", enig->reflector, enig->wiring);
	printf("*******\n");

	char mssg [88] = "El que guarda su boca guarda su alma\nmas el que mucho abre sus labios tendra calamidad.";
	for(int i = 0; i < 87; i++) {
		mssg[i] = encrypt(enig, mssg[i]);
	}
	
	printf("%s\n", mssg);

	return 0;
}
		





