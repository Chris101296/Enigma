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


/* Struct for both the rotors and reflector of an Enigma machine.
   Contains set, a number from 0 to 25 representing the current position of 
   the rotor as well as letter, the "alphabet" used by this piece. 
   The reflector will be assigned
*/   
typedef struct __attribute__((packed)) part_t {
	uint8_t set;	//current position of rotor (or 26 for reflector)
	uint8_t turnPoint;	//Point at which the rotor turns the left rotor
						//Unused by Slow Rotor and Reflector
						//Should match alph used
	char alph [ALPH_LENGTH + 1];  //alphabet used by piece, defined in parts.h
} part;


/*	Struct for the circuit, main component, of the Enigma Machine.
	Contains a 4 element array of parts with the follwing defintions
	[3] Fast Rotor
	[2] Middle Rotor (Can Double step)
	[1] Slow Rotor
	[0] Reflector set is 26 for sanity check

	wiring represents the wire board of the machine with each
	evenly indexed char being connected to the oddly indexed 
	char to the right. All characters must be different and between A and Z.  

	the midChange boolean is used to see if
*/
typedef struct __attribute__((packed)) enigma_t {
	char wiring [WORD_COUNT + 1];	//letters to swap at beginning and end	
	char reflector [ALPH_LENGTH + 1]; //alphabet of reflector
	part parts [PART_COUNT];	//current position of rotor (or 26 for reflector)
} enigma;

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

	//Begin Encryption
	c = wireSwap(enig, c);
	
	//Fast Rotor
	c -= 'A'; 						//Converts char to rotor notation (0 - 25)
	uint8_t index = (c + enig->parts[2].set) % ALPH_LENGTH; //index of letter to swap
	c =  enig->parts[2].alph[(uint8_t)c] - 'A'; //get final result of swap
	#ifdef DEBUG
		printf("%d ", c);
	#endif
	
	//Rest of Rotors
	for(int i = PART_COUNT - 1; i > -1; i--) {
		index = (c + enig->parts[i].set - enig->parts[i - 1].set) % ALPH_LENGTH;
		index = (index + ALPH_LENGTH) % ALPH_LENGTH;
		c = enig->parts[i].alph[index] - 'A'; //get final result of swap
		#ifdef DEBUG
			printf("%d ", c);
		#endif
	}
	
	//Reflector Part
	index = ( (c - enig->parts[0].set) % ALPH_LENGTH + ALPH_LENGTH) % ALPH_LENGTH;
	c = enig->reflector[index] - 'A';
    #ifdef DEBUG
		printf("%d ", c);
	#endif
	
	//reverse
	c = (c + enig->parts[0].set) % ALPH_LENGTH + 'A';
	c = strchr(enig->parts[0].alph, c) - (char*)&enig->parts[0];
    #ifdef DEBUG
		printf("%d ", c);
	#endif

	for(int i = 1; i < PART_COUNT; i++) {
		c = (c + enig->parts[i].set - enig->parts[i - 1]. set) % ALPH_LENGTH;
		c = ((c + ALPH_LENGTH) % ALPH_LENGTH + ALPH_LENGTH) % ALPH_LENGTH + 'A';
		c = strchr(enig->parts[0].alph, c) - (char*)&enig->parts[i];
	    #ifdef DEBUG
			printf("%d ", c);
		#endif
	}	 

	c = ((c - enig->parts[2].set) % ALPH_LENGTH + ALPH_LENGTH) % ALPH_LENGTH + 'A';
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

	char mssg [88] = "El que";// guarda su boca guarda su alma\n mas el que mucho abre sus labios tendra calamidad.";
	for(int i = 0; i < 87; i++) {
		mssg[i] = encrypt(enig, mssg[i]);
	}
	
	printf("%s\n", mssg);

	return 0;
}
		





