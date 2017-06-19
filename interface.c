#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#include "enigma.h"
#define DEBUG_I

void loadSettings(enigma* enig);
void checkOpp(char* ans, char* options);
void encrMssg(char ans, enigma* enig);
void wiring(enigma* enig);
void rotors(enigma* enig);

int main() {
	enigma* enig = malloc(sizeof(enigma));
	while(1) {
		loadSettings(enig);
		printf("----------\n| ENIGMA |\n----------\n\n");
		printf("%-5c%s\n%-5c%s\n%-5c%s\n%-5c%s\n\nChoose one: ",
			WR_AND_EN, "WRITE AND ENCRYPT", 
			EN, "ENCRYPT PREXISTING MESSAGE", MOD_WIRING, "MODIFY WIRING", 
			MOD_ROT, "MODIFY ROTORS");
		char ans[3]; 
		fgets(ans, sizeof(ans), stdin);
		checkOpp(ans, MENU);
	
		if(ans[0] == WR_AND_EN || ans[0] == EN) 
			encrMssg(ans[0], enig);
		else if(ans[0] == MOD_WIRING)
			wiring(enig);
		else 
			rotors(enig);
		
		printf("Continue?\n[Y]%10s\n[N]%10s\n", "YES", "NO");
		fgets(ans, sizeof(ans), stdin);
		checkOpp(ans, "YN");
		if(ans[0] == 'N') break;
	}
	free(enig);
	return 0;
}

/** *Obtains the settings from EXISTING, PROPPERLY FORMATTED
	*enigma.enig file and initializes enigma structure pointed 
	*to by enig **/
void loadSettings(enigma* enig) {
	//Creates default file if it does not exist	
	if(access("enigma.enig", F_OK) == -1) {
		FILE *conf = fopen(CONF, "w");	
		fprintf(conf, "%s\n%s\n%d\n%d\n%d\n%s\n%d\n%d\n%d\n%s\n%d\n%d\n%d\n%s\n",
			    "QWERTYUIOPASDFGHJKLZ", UKW_B,
				5, 0, TURN_III, ROTOR_III, 21, 0, TURN_II, ROTOR_II, 16, 0, TURN_I, ROTOR_I);
		fclose(conf);	
	}	
	
	FILE *conf = fopen(CONF, "r");
		fscanf(conf, "%s %s %hhu %hhu %hhu %s %hhu %hhu %hhu %s %hhu %hhu %hhu %s", 
			&enig->wiring[0], 
			&enig->reflector[0],
			&enig->parts[0].pos, &enig->parts[0].set, &enig->parts[0].turnPoint,&enig->parts[0].alph[0],	
			&enig->parts[1].pos, &enig->parts[1].set, &enig->parts[1].turnPoint,&enig->parts[1].alph[0],
			&enig->parts[2].pos, &enig->parts[2].set, &enig->parts[2].turnPoint,&enig->parts[2].alph[0]);	
	fclose(conf);
	#ifdef DEBUG_I
		printf("*******\n");
		for(int i = 0; i < 3; i++) {
			printf("ROTOR %d: |%s| %d SET: %d\n", i, enig->parts[i].alph, enig->parts[i].pos, enig->parts[i].set );
		}
		printf("UKW: %s\nWIRING: %s\n", enig->reflector, enig->wiring);
		printf("*******\n");
	#endif
}

void checkOpp(char* ans, char* options) {
	bool badOpp = 1;
	if(ans[0] > 'Z') ans[0] -= CASE_GAP;
	for(int i = 0; i < strlen(options); i++) {
		if(ans[0] == options[i]) {
			badOpp = 0;
			break;
		}
	}
	while(badOpp || ans[1] != '\n') {
		while(ans[1] != '\n'){
			ans[1] = fgetc(stdin);
			#ifdef DEBUG_I
				printf("buffer: %c\n", ans[1]);
			#endif
		}
		printf("\nERROR: Type ONE of the letters above\n");
		fgets(ans, sizeof(ans), stdin);
		#ifdef DEBUG_I
			printf("1%c 2%c 3%c  %d\n", ans[0], ans[1], ans[2], (int)badOpp);
		#endif
		if(ans[0] > 'Z') ans[0] -= CASE_GAP;
		badOpp = 1;
		for(int i = 0; i < strlen(options); i++) {
			if(ans[0] == options[i]) {
				badOpp = 0;
				break;
			}	
		}
	}
}

void wiring(enigma* enig) {
	char in[4];
	printf("\nCHANGING WIRE SETTINGS:\n");
	printf("For each wire, type two letters (without spaces) to link together:\n");

	for(uint8_t  i = 0; i < WORD_COUNT; i+=2) {
		printf("WIRE # %d: ", i/2 + 1);
		fgets(in, sizeof(in), stdin);
		//Catch capitalization issues		
		if(in[0] > 'Z')
			in[0] -= CASE_GAP;
		if(in[1] > 'Z')
			in[1] -= CASE_GAP;
		#ifdef DEBUG_I
			printf("%d 0%c 1%c 2%c 3%c\n",sizeof(in), in[0], in[1], in[2], in[3]);
		#endif
				
		if( in[2] != '\n' || in[0] == in[1] || 
			in[0] < 'A' || in[0] > 'Z' || in[1] < 'A' || in[1] > 'Z' ) {
			
			while(in[2] != '\n' && in[1] != '\n' && in[0] != '\n') in[2] = fgetc(stdin);
			printf(WIRING_ERROR);
			i-=2;
		}
		else {
			enig->wiring[i] = in[0];
			enig->wiring[i+1] = in[1];
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
	FILE *conf = fopen(CONF, "r+");
	fprintf(conf, "%s\n", enig->wiring);
	fclose(conf);
}	

void rotors(enigma* enig) {
	FILE *conf = fopen(CONF, "r+");
	char *trash = malloc(28*sizeof(char));
	fscanf(conf, "%s %s", trash, trash);
	free(trash);
	printf("CHANGING ROTOR SETTINGS:\n");
	printf("Type the Roman Numeral of the rotor you choose,\n");
	printf("the startting position (Letter) you wish for each rotor,\n");
	printf("and the ring setting (Letter) you wish for each rotor\n");
	const char pos [3][7] = {"SLOW", "MIDDLE","FAST"};
	char in[5];
	for(uint8_t i = 0; i < PART_COUNT; i++) {
			printf("\n%s ROTOR: ", pos[i]); //Choose a rotor to use
			fgets(in, sizeof(in), stdin);
			if(in[0] > 'Z')
				in[0] -= CASE_GAP;
			if(in[1] > 'Z')
				in[1] -= CASE_GAP;
			if(in[2] > 'Z')
				in[2] -= CASE_GAP;
			
			while((in[0]!='I' && (in[0]!='V' || in[1]!='\n')) ||  
				 (in[0]=='I' && in[1]!= '\n' && 
				( (in[1]!='I' && (in[1]!='V' || in[2]!='\n')) || 
				(in[1]=='I' && in[2]!='\n' && (in[2]!='I' || in[3]!='\n'))) )){	
				#ifdef DEBUG_I
						printf("\nCHARS: %d %d %d %d %d\n", in[0],
						in[1], in[2], in[3], in[4]);
				#endif	
			
				while(in[0] != '\n' && in[1] != '\n'&& 
					in[2] != '\n' && in[3] != '\n') 
					in[3] = fgetc(stdin);
				
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
			chooseRotor(enig, in, i);

			printf("ROTOR POSITION: ");	//Choose rotor's position
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
			enig->parts[i].pos = in[0] - 'A';

			printf("RING SETTING: ");	//Choose a ring setting
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

void encrMssg(char ans, enigma* enig) {
	if(ans == 'A' || access(MSSG, F_OK) == -1) {
		system("echo 'TYPE YOUR MESSAGE IN THIS FILE' > mssg.txt");	//MSSG
		system("nano mssg.txt");	//MSSG
	}
	FILE *mssg = fopen(MSSG, "r");
	FILE *encr = fopen(ENCR, "w");
	char c[100];	
	char*stat = fgets(c, sizeof(c), mssg);
	while(*c != '\0' && stat != NULL) {
		#ifdef DEBUG_I
			printf("B4\n%s\n", c);
		#endif		
		for(int i = 0; i < 100 && c[i] != '\0'; i++)
			c[i] = encrypt(enig, c[i]);		 	
		
		fputs(c, encr);		
		printf("%s\n", c); 
		stat = fgets(c, sizeof(c), mssg);
	}
	fclose(mssg);
	fclose(encr);
}

