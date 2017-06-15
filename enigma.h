/* Defines constants of the rotors and refletor of the Enigma Machine.

 * DD/MM/YYYY
*/

#ifndef ENIGMA_PARTS
#include "parts.h"

#endif

#ifndef ENIGMA
#define ENIGMA
extern char encrypt(enigma *enig, char c);
extern void chooseRotor(enigma* enig, char* rotor, uint8_t sec);
#endif

