#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#include "enigma.h"
//#define DEBUG_I

void loadSettings(enigma* enig);
void checkOpp(char* ans, char* options);
void encrMssg(char ans, enigma* enig);
void wiring(enigma* enig);
void rotors(enigma* enig);

int main() {
	enigma* enig = malloc(sizeof(enigma)); //Create space for enigma data
	loadSettings(enig);
	while(1) {
		//Title asking user to pick an option
		printf("------------\n| ENIGMA I |\n------------\n\n");
		printSettings(enig);
		printf("%-5c%s\n%-5c%s\n%-5c%s\n%-5c%s\n\nChoose one: ",
			WR_AND_EN, "WRITE AND ENCRYPT", 
			EN, "ENCRYPT PREXISTING MESSAGE", MOD_WIRING, "MODIFY WIRING", 
			MOD_ROT, "MODIFY ROTORS");
		
		char ans[3]; 
		fgets(ans, sizeof(ans), stdin);
		checkOpp(ans, MENU);
	
		//Executes user's choice
		if(ans[0] == MOD_ROT) //D
			rotors(enig);
		else if(ans[0] == MOD_WIRING) //C
			wiring(enig);
		else {//A or B chosen
			encrMssg(ans[0], enig);
			loadSettings(enig); //reinitialize enigma
		}
		
		//Ask user whether to stop
		printf("\nContinue?\n[Y]%10s\n[N]%10s\n", "YES", "NO");
		fgets(ans, sizeof(ans), stdin);
		checkOpp(ans, "YN");
		if(ans[0] == 'N') break;
	}
	free(enig); //free maloced space
	return 0;
}

/** *Obtains the settings from EXISTING, PROPPERLY FORMATTED
	*enigma.enig file and initializes enigma structure pointed 
	*to by enig **/
void loadSettings(enigma* enig) {
	//Creates default file if it does not exist	
	if(access("enigma.enig", F_OK) == -1) { 
		FILE *conf = fopen(CONF, "w");	
		fprintf(conf,"%s\n%s\n%d\n%d\n%d\n%s\n%d\n%d\n%d\n%s\n%d\n%d\n%d\n%s\n",
			    "QWERTYUIOPASDFGHJKLZ", UKW_B,
				5, 0, TURN_III, ROTOR_III, 21, 0, TURN_II, 
				ROTOR_II, 16, 0, TURN_I, ROTOR_I);
		fclose(conf);	
	}	
	
	FILE *conf = fopen(CONF, "r"); //Read settings
		fscanf(conf, "%s %s %hhu %hhu %hhu %s %hhu %hhu %hhu "
			"%s %hhu %hhu %hhu %s", 
			&enig->wiring[0], 
			&enig->reflector[0],
			&enig->parts[0].pos, &enig->parts[0].set, 
			&enig->parts[0].turnPoint,&enig->parts[0].alph[0],	
			&enig->parts[1].pos, &enig->parts[1].set, 
			&enig->parts[1].turnPoint,&enig->parts[1].alph[0],
			&enig->parts[2].pos, &enig->parts[2].set, 
			&enig->parts[2].turnPoint,&enig->parts[2].alph[0]);	
	fclose(conf);
}

/** Takes a single character address (ans) and makes sure that it is in the 
    string options. If it isn't. the user will be notified until he changes 
    ans to a character in options **/
void checkOpp(char* ans, char* options) {
	bool badOpp = 1;
	if(ans[0] > 'Z') ans[0] -= CASE_GAP; //Make uppercase
	for(int i = 0; i < strlen(options); i++) {
		if(ans[0] == options[i]) {
			badOpp = 0; //Makes badOpp false if ans[0] is in options
			break;
		}
	}
	//Makes sure badOpp is false and only one character was typed.
	while(badOpp || ans[1] != '\n') {
		while(ans[1] != '\n'){ //Clear stdin buffer
			ans[1] = fgetc(stdin);
			#ifdef DEBUG_I
				printf("buffer: %c\n", ans[1]);
			#endif
		}
		printf("\nERROR: Type ONE of the letters above\n");
		fgets(ans, sizeof(ans), stdin); //Take new input
		#ifdef DEBUG_I
			printf("1%c 2%c 3%c  %d\n", ans[0], ans[1], ans[2], (int)badOpp);
		#endif
		if(ans[0] > 'Z') ans[0] -= CASE_GAP;
		badOpp = 1; //recalculate badOpp
		for(int i = 0; i < strlen(options); i++) {
			if(ans[0] == options[i]) {
				badOpp = 0;
				break;
			}	
		}
	}
}

/** Allows user to change the wiring of the Enigma Machine**/
void wiring(enigma* enig) {
	char in[4]; //Takes in two letters
	printf("\nCHANGING WIRE SETTINGS:\n");
	printf("For each wire, type two letters (without spaces) to "
			"link together:\n");

	for(uint8_t  i = 0; i < WORD_COUNT; i+=2) {
		printf("WIRE # %d: ", i/2 + 1);
		fgets(in, sizeof(in), stdin);
		//Catch capitalization issues		
		if(in[0] > 'Z')
			in[0] -= CASE_GAP;
		if(in[1] > 'Z')
			in[1] -= CASE_GAP;
		#ifdef DEBUG_I
			printf("%d 0%c 1%c 2%c 3%c\n",sizeof(in), in[0], in[1], 
				in[2], in[3]);
		#endif
		//Makes sure two unique letters were typed		
		if( in[2] != '\n' || in[0] == in[1] || 
			in[0] < 'A' || in[0] > 'Z' || in[1] < 'A' || in[1] > 'Z' ) {
			
			while(in[2] != '\n' && in[1] != '\n' && in[0] != '\n') //clear stdin
				in[2] = fgetc(stdin);
			printf(WIRING_ERROR);
			i-=2;
		}
		else {
			enig->wiring[i] = in[0];
			enig->wiring[i+1] = in[1];
			//Makes sure letters were not used before in wiring
			for(uint8_t j = 0; j < i; j++) {
				if(enig->wiring[j] == in[0] || enig->wiring[j] == in[1]) {
					printf(WIRING_REPEAT);
					i-=2;
					break;
				}
			}
		}
	}
	enig->wiring[21] = '\0';
	#ifdef DEBUG_I
			printf("\nWIRING: %s\n", enig->wiring);
	#endif
	
	//Store changes
	FILE *conf = fopen(CONF, "r+");
	fprintf(conf, "%s\n", enig->wiring);
	fclose(conf);
}	

/**	Makes changes to the rotors of the enigma machine, storeing changes in CONF 
	Changes all rotors, their alphabets, settings, and starts positions**/
void rotors(enigma* enig) {
	FILE *conf = fopen(CONF, "r+");
	//Move straight to the rotor portion of CONF
	char *trash = malloc(28*sizeof(char));
	fscanf(conf, "%s %s", trash, trash);
	free(trash);

	printf("CHANGING ROTOR SETTINGS:\n");
	printf("Type the Roman Numeral of the rotor you choose,\n");
	printf("the startting position (Letter) you wish for each rotor,\n");
	printf("and the ring setting (Letter) you wish for each rotor\n");
	
	const char pos [3][7] = {"SLOW", "MIDDLE","FAST"}; // Designation of rotors
	char in[5];
	for(uint8_t i = 0; i < PART_COUNT; i++) {
			printf("\n%s ROTOR: ", pos[i]); //Choose a rotor to use
			fgets(in, sizeof(in), stdin);
			//Capitalize input if need be
			if(in[0] > 'Z')
				in[0] -= CASE_GAP;
			if(in[1] > 'Z')
				in[1] -= CASE_GAP;
			if(in[2] > 'Z')
				in[2] -= CASE_GAP;
			//Check if input == Roman numeral from I to V inclusive
			while((in[0]!='I' && (in[0]!='V' || in[1]!='\n')) ||  
				 (in[0]=='I' && in[1]!= '\n' && 
				( (in[1]!='I' && (in[1]!='V' || in[2]!='\n')) || 
				(in[1]=='I' && in[2]!='\n' && (in[2]!='I' || in[3]!='\n'))) )){
				
				#ifdef DEBUG_I
						printf("\nCHARS: %d %d %d %d %d\n", in[0],
						in[1], in[2], in[3], in[4]);
				#endif	
			
				while(in[0] != '\n' && in[1] != '\n'&&  //Clear buffer
					in[2] != '\n' && in[3] != '\n') 
					in[3] = fgetc(stdin);
				
				//Ask for new input
				printf("ERROR:\nType a ROMAN NUMERAL I - V\n");
				printf("AGAIN %s ROTOR: ", pos[i]);
				fgets(in, sizeof(in), stdin);
				if(in[0] > 'Z')
					in[0] -= CASE_GAP;
				if(in[1] > 'Z')
					in[1] -= CASE_GAP;
				if(in[2] > 'Z')
					in[2] -= CASE_GAP;
			}
			chooseRotor(enig, in, i); // sets current rotor with propper alph

			printf("ROTOR POSITION: ");	//Choose rotor's position, single letter
			fgets(in, CHAR_IN, stdin);
			//Catch capitalization issues		
			if(in[0] > 'Z') in[0] -= CASE_GAP;
			
			while((in[1] != '\n') || in[0] < 'A' || in[0] > 'Z') {
				#ifdef DEBUG_I
						printf("\nCHARS: %d %d\n", in[0],
						in[1]);
						printf("%d %d %d\n", in[1]!='\n',in[0]<'A',in[0]>'Z');
				#endif				
				while(in[1]!='\n' && in[0]!='\n') in[1] = fgetc(stdin);
				printf("ERROR:\nType a SINGLE LETTER\n");
				printf("AGAIN %s ROTOR: ", pos[i]);
				fgets(in, CHAR_IN, stdin);
				if(in[0] > 'Z') in[0] -= CASE_GAP;
			}
			enig->parts[i].pos = in[0] - 'A'; //set to pos to propper value

			printf("RING SETTING: ");	//Choose a ring setting
										//Similar to position above
			fgets(in, CHAR_IN, stdin);
			//Catch capitalization issues		
			if(in[0] > 'Z') in[0] -= CASE_GAP;
			
			while((in[1] != '\n')
					 || in[0] < 'A' || in[0] > 'Z') {
				#ifdef DEBUG_I
						printf("\nCHARS: %d %d\n", in[0],
						in[1]);
						printf("%d %d %d\n", in[1]!='\n',in[0]<'A',in[0]>'Z');
				#endif				
				while(in[1]!='\n' && in[0]!='\n') in[1] = fgetc(stdin);
				printf("ERROR:\nType a SINGLE LETTER\n");
				printf("AGAIN %s ROTOR: ", pos[i]);
				fgets(in, CHAR_IN, stdin);
				if(in[0] > 'Z') in[0] -= CASE_GAP;
			}
			enig->parts[i].set = in[0] - 'A';
			
			//Store changes after storeing in enig
			fprintf(conf, "\n%hhu\n%hhu\n%hhu\n%s", enig->parts[i].pos, 
					enig->parts[i].set, enig->parts[i].turnPoint, 
					enig->parts[i].alph);
	}
	fclose(conf);
	#ifdef DEBUG_I
		printf("*******\n");
		for(int i = PART_COUNT - 1; i >= 0; i--) {
			printf("ROTOR %d: |%s| POS: %d SET: %d TURN: %d\n", i, 
			enig->parts[i].alph, enig->parts[i].pos, enig->parts[i].set, 
			enig->parts[i].turnPoint );
		}
		printf("UKW: %s\nWIRING: %s\n", enig->reflector, enig->wiring);
		printf("*******\n");
	#endif
}

/** Encrypts message using enig settings
	if ans is 'A' or mssg.txt does not exist, ask user to create new message.
	otherwise, ans is supposed to be'B' **/
void encrMssg(char ans, enigma* enig) {
	if(ans == 'A' || access(MSSG, F_OK) == -1) { //Write mssg.txt
		system("echo 'TYPE YOUR MESSAGE IN THIS FILE' > mssg.txt");	//MSSG
		system("nano mssg.txt");	//MSSG
	}
	FILE *mssg = fopen(MSSG, "r");
	FILE *encr = fopen(ENCR, "w");
	char c[100]; //read 100 characters or whole line at a time
	char*stat = fgets(c, sizeof(c), mssg); //NULL if reads nothing
	while(*c != '\0' && stat != NULL) { 
		#ifdef DEBUG_I
			printf("B4\n%s\n", c);
		#endif		
		for(int i = 0; i < 100 && c[i] != '\0'; i++)
			c[i] = encrypt(enig, c[i]);//encrypt each character	 	
		
		//print c string onto ENCR and screen
		fputs(c, encr);
		printf("%s\n", c);
		stat = fgets(c, sizeof(c), mssg); //get next set of chars
	}
	fclose(mssg);
	fclose(encr);
}

