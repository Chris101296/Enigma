/* Defines constants of the rotors and refletor of the Enigma Machine.

 * DD/MM/YYYY
*/

#ifndef ENIGMA_PARTS
#include "parts.h"

#endif

#ifndef ENIGMA
#define ENIGMA

#define CONF "enigma.enig"
#define MSSG "mssg.txt"
#define ENCR "encr.txt"
#define MENU "ABCD"
#define WR_AND_EN  'A'
#define EN         'B'
#define MOD_WIRING 'C'
#define MOD_ROT    'D'

#define WIRING_ERROR "ERROR\n Type 2 unspaced letters (CU):\nAGAIN "
#define WIRING_REPEAT "You have already used one of these letters\nAGAIN "
#define CHAR_IN 3	//number of characters to read when checking if correct character entered
extern char encrypt(enigma *enig, char c);
extern void chooseRotor(enigma* enig, char* rotor, uint8_t sec);
#endif

