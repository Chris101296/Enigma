/* Defines constants of the rotors and refletor of the Enigma Machine.

 * DD/MM/YYYY
*/
#ifndef ENIGMA_PARTS
#define ENIGMA_PARTS

#define ALPH_LENGTH 26	//length of alphabet of each rotor
#define WORD_COUNT 10		//Number of words plugged by wires 
							//(number of wires * 2)
#define PART_COUNT 3		//Number of rotors

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

#endif

