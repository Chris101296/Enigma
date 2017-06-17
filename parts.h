/* Defines constants of the rotors and refletor of the Enigma Machine.

 * DD/MM/YYYY
*/
#ifndef ENIGMA_PARTS
#define ENIGMA_PARTS

#define ALPH_LENGTH 26	//length of alphabet of each rotor
#define WORD_COUNT 20		//Number of words plugged by wires 
							//(number of wires * 2)
#define WIRE_COUNT 10		//Number of wires
#define PART_COUNT 3		//Number of rotors
#define CASE_GAP  32	// Equivilant to 'a' - 'A'

/* Alphabets of respective Rotors */
#define ROTOR_I    "EKMFLGDQVZNTOWYHXUSPAIBRCJ"
#define ROTOR_II   "AJDKSIRUXBLHWTMCQGZNPYFVOE"
#define ROTOR_III  "BDFHJLCPRTXVZNYEIWGAKMUSQO"
#define ROTOR_IV   "ESOVPZJAYQUIRHXLNFTGKDCMWB"
#define ROTOR_V    "VZBRGITYUPSDNHLXAWMJQOFECK"

/* Turining positions of respective Rotors */
#define TURN_I   16		// 'Q' - 'A'
#define TURN_II   4		// 'E' - 'A'
#define TURN_III 21		// 'V' - 'A'
#define TURN_IV   9		// 'J' - 'A'
#define TURN_V   25		// 'Z' - 'A'

/*Reflector Alphabets*/
#define UKW_B     "YRUHQSLDPXNGOKMIEBFZCWVJAT"
//Following Not used in WWII too much
#define UKW_A     "EJMZALYXVBWFCRQUONTSPIKHGD"
#define UKW_C     "FVPJIAOYEDRZXWGCTKUQSBNMHL"

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

#endif

