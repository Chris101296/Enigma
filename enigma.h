/* Defines constants of the user's interface with the Enigma Machine.
   Also declares two functions from enigma.c for interface.c
 * 21/06/2017
*/

#ifndef ENIGMA_PARTS
#include "parts.h"

#endif

#ifndef ENIGMA
#define ENIGMA

#define CONF "enigma.enig"	//File for Enigma Configuration
#define MSSG "mssg.txt"		//File ofor input
#define ENCR "encr.txt"		//File to store output
#define MENU "ABCD"			//Options for start menu
#define WR_AND_EN  'A'		//Writing new message
#define EN         'B'		//Encrypting old message
#define MOD_WIRING 'C'		//Change Wiring setting
#define MOD_ROT    'D'		//Change rotor settigns and positions

#define WIRING_ERROR "ERROR\n Type 2 unspaced letters (CU):\nAGAIN "
#define WIRING_REPEAT "You have already used one of these letters\nAGAIN "
#define CHAR_IN 3	//#characters to read when checking char input

//Functions defined in enigma.c
extern char encrypt(enigma *enig, char c); 
extern void chooseRotor(enigma* enig, char* rotor, uint8_t sec);
extern void printSettings(enigma* enig);
#endif

