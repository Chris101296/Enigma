#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <stdbool.h>
#include <stdint.h>
#include <assert.h>

#include "parts.h"

/*Function for normal stepping on the Enigma Machine*/
#define STEP(step) ((step + 1) % ALPH_LENGTH ) 
//#define DEBUG

/*Steps through entire machine, called before encrypting letter
*/
void stepMachine(enigma* enig) {
	//booleans deciding whether middle and left rotors turn.
	bool midTurn = enig->parts[2].pos == enig->parts[2].turnPoint;
	bool leftTurn = enig->parts[1].pos == enig->parts[1].turnPoint;

	if(leftTurn) 
		enig->parts[0].pos = STEP(enig->parts[0].pos);
	
	if(midTurn || leftTurn)	// Takes into account double-stepping
		enig->parts[1].pos = STEP(enig->parts[1].pos);	
	
	enig->parts[2].pos = STEP(enig->parts[2].pos);
}

/**Simualtes the portion of encryption conducted by the wire board
   Looks for letter in enig->wiring and if found, returns the letter
   paired to the input **/
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
extern char encrypt(enigma* enig, char c) {	
	if(c > 'Z' || c < 'A') {
		if(c >= 'a' && c <= 'z')
			return encrypt(enig, c - CASE_GAP);
		else
			return c;
	}
	stepMachine(enig);

	#ifdef DEBUG
		printf("%d|%d|%d\n",enig->parts[0].pos, 
						   enig->parts[1].pos, 
						   enig->parts[2].pos);
	#endif

	//Begin Encryption
	c = wireSwap(enig, c);
	#ifdef DEBUG
		printf("%d ", c - 'A');
	#endif
	
	//Fast Rotor
	c -= 'A'; 						//Converts char to rotor notation (0 - 25)
	uint8_t index = (c + enig->parts[2].pos - 
			enig->parts[2].set + ALPH_LENGTH) % ALPH_LENGTH; //index of letter to swap
	
	c =  (enig->parts[2].alph[index] - 'A'+ 
		enig->parts[2].set - enig->parts[2].pos + 
		ALPH_LENGTH) % ALPH_LENGTH; //get final result of swap
	
	#ifdef DEBUG
		printf("%d ", c);
	#endif
	
	//Rest of Rotors
	for(int i = PART_COUNT - 2; i > -1; i--) {
		index = (c + enig->parts[i].pos - enig->parts[i].set 
			+ ALPH_LENGTH) % ALPH_LENGTH;
		c = (enig->parts[i].alph[index] - 'A' + enig->parts[i].set 
			- enig->parts[i].pos + ALPH_LENGTH) % ALPH_LENGTH;
		#ifdef DEBUG
			printf("%d|%d ", index, c);
		#endif
	}
	
	//Reflector Part
	index = c;
	c = enig->reflector[index] - 'A';
    #ifdef DEBUG
		printf("%d ", c);
	#endif
	
	//Reverse
	c = (c + enig->parts[0].pos - enig->parts[0].set 
		+ ALPH_LENGTH) % ALPH_LENGTH + 'A';
	c = strchr(enig->parts[0].alph, c) - (char*)&enig->parts[0].alph; //Index of calculated letter
	c = (c - enig->parts[0].pos + enig->parts[0].set + ALPH_LENGTH) % ALPH_LENGTH;  
	#ifdef DEBUG
		printf("%d ", c);
	#endif

	for(int i = 1; i < PART_COUNT; i++) {
		c = (c + enig->parts[i].pos - enig->parts[i].set + 
				ALPH_LENGTH) % ALPH_LENGTH;
		c = c + 'A';
		c = strchr(enig->parts[i].alph, c) - (char*)&enig->parts[i].alph;
		c = (c - enig->parts[i].pos + enig->parts[i].set 
			+ ALPH_LENGTH) % ALPH_LENGTH;	    
		#ifdef DEBUG
			printf("%d ", c);
		#endif
	}	 

	//Ready for Wire Board	
	c = c +'A';
	c = wireSwap(enig, c);
    #ifdef DEBUG
		printf("%c\n", c);
	#endif
	return c;
}

/*Given, string rotor is one of the 5 rotors, sec is between 0 and 2 
  incluseve, this function takes rotor and returns the corresponding 
  alphabet*/
extern void chooseRotor(enigma* enig, char* rotor, uint8_t sec) {
	if(strcmp(rotor, "I\n") == 0) {
		strncpy(enig->parts[sec].alph, ROTOR_I, ALPH_LENGTH + 1);
		enig->parts[sec].turnPoint = TURN_I;	
	}
	
	else if(strcmp(rotor, "II\n") == 0) {
		strncpy(enig->parts[sec].alph, ROTOR_II, ALPH_LENGTH + 1);
		enig->parts[sec].turnPoint = TURN_II;	
	}

	else if(strcmp(rotor, "III\n") == 0) {
		strncpy(enig->parts[sec].alph, ROTOR_III, ALPH_LENGTH + 1);
		enig->parts[sec].turnPoint = TURN_III;	
	}

	else if(strcmp(rotor, "IV\n") == 0) {
		strncpy(enig->parts[sec].alph, ROTOR_IV, ALPH_LENGTH + 1);
		enig->parts[sec].turnPoint = TURN_IV;	
	}

	else if(strcmp(rotor, "V\n") == 0) {
		strncpy(enig->parts[sec].alph, ROTOR_V, ALPH_LENGTH + 1);
		enig->parts[sec].turnPoint = TURN_V;	
	}
}
/** Returns appropriate string for given turn point 
    Helpper function for printSettings. Assumes turn is one
    of the 5 constants in the if statements below**/
char* rotSymb(uint8_t turn) {
	if(turn == TURN_I)
		return "I";
	if(turn == TURN_II)
		return "II";
	if(turn == TURN_III)
		return "III";
	if(turn == TURN_IV)
		return "IV";
	else  			//(turn == TURN_V)
		return "V";
}

/** Prints settings of Enigma Machine**/
extern void printSettings(enigma* enig) {
	printf("WIRING: ");
	for(int i = 0; i < WORD_COUNT; i+=2)
		printf("%c-%c ", enig->wiring[i], enig->wiring[i+1]);
	printf("\nREFLECTOR = UKW-B\n");
	const char rots[3][7] = {"SLOW", "MIDDLE", "FAST"};
	for(int i = 0; i < PART_COUNT; i++) {
		printf("\n%s ROTOR:\nROTOR: %-4s  SETTING: %c  "
		"START POSITION: %c\n", rots[i], 
		rotSymb(enig->parts[i].turnPoint), 
		enig->parts[i].set + 'A', enig->parts[i].pos + 'A');	
	}
	printf("\n");
}
