// Author: Jeffrey Garfield
// Version:4/16/18
// File: mock unlock and lock file

//#define F_CPU 1000000UL
#include <avr/io.h>		// Always include this
#include <stdio.h>		// If you want to use printf
//#include <util/delay.h>		// for delays (but you'll write your own)
#include "lcd.h"

static FILE lcd_stdout=FDEV_SETUP_STREAM(lcd_putc,NULL,_FDEV_SETUP_WRITE);


#define BUFFSIZE 16

// the following is provided in case it is needed in "getkey()" -- see class notes
#define nop() asm volatile("nop")


signed char getkey(void);			// forward declarations
signed char mapkey(signed char *key);
char getpw(char maxsize, char * const pw); 	// Input keys into the array
char compareArrays(char size, const char * const a1, const char * const a2);
void delaycycles(unsigned int cycles);
void mydelayms(unsigned int ms);
void makeNoise(int);
void printStars(char);
void printError(int, int);

void main(void) {
	char passwd[] = {0};			// Hardcode the password here
	char pwsize = 0;
	char pwbuff[BUFFSIZE];

	DDRA = 0xF0; // While running the lower 4 bits of PORTA will be inputs and the 
				 // upper 4 bits will be outputs, but we'll make them outputs one at 
				 // a time, so we'll actually control the data direction in getkey(),
				 // the keypad scanning routine.
	
	TCCR1B  = 0x01;		// Enable Timer 1 to count normally using the internal clock 


	lcd_init();		// Init the LCD display
	stdout=&lcd_stdout;

	// Your main code here
	printf("Set Password");
	pwsize = getpw(BUFFSIZE, &passwd);
	
	while(1){
		lcd_iwrite(LCD_CLR);
		printf("Enter Password");
		if(getpw(BUFFSIZE, &pwbuff) != pwsize){
			printError(0, 250);
			makeNoise(0);
		}
		else{
			if(compareArrays(pwsize, &passwd, &pwbuff)){
				printError(1, 2000);
				makeNoise(1);
			}
			else {
				printError(0, 250);
				makeNoise(0);
			}
		}
	}
}

// delaycycles() and mydelayms() functions here
signed char getkey() {
    int key = -1;
    int i, j;
    int row = 0;
    int col = 0;
    int maskRow = 0b00010000;
    int maskCol = 0b00000001;
    
    for (i = 4; i < 8; i++){
        DDRA = maskRow << ( i - 4);
        PORTA = maskRow << (i - 4);
        row = (i - 4);
		nop();
        
        for (j = 0; j < 4; j++){
            if ((PINA &(maskCol << j)) == (maskCol <<j)) {
                col = j;
                key = 4 * row + col;
				nop();
            }
        }
    }
    return key;
}

signed char mapkey(signed char key){
    if((key < 0) || (key >15)){
        return key = -1;
    }
    signed char numkey[16] = {1, 2, 3, 10, 4, 5, 6, 11, 7, 8 , 9 , 12, '*', 0, '#', 13};
    key = numkey[key];
    return key;
}

char getpw(char size, char * const password){
    char *buffer = password;
	char i = 0;
    while(1){
        while(getkey() != -1){}
        while(getkey() == -1){}
        
        if((buffer - password) < size){
            if(mapkey(getkey()) <= 13) {
                *buffer = mapkey(getkey());
                buffer++;
				printStars(&buffer, & password);
				
            }
        }
        if((mapkey(getkey()) == '*')&&((buffer - password) > 0)){
            buffer--;
            *buffer = NULL; 
			printStars(&buffer, & password);
        }
        if(mapkey(getkey()) == '#'){
            return buffer - password;
        }
	}
}

char compareArrays(char SIZE, const char *a, const char *b){
        char *temp = a;
        while ((a - temp) < SIZE){  
            if (*a != *b){
                return 0; //0 if different arrays
            }
            a++;
            b++;
        }
        return 1; //1 if arrays are the same
}

void delaycycles(unsigned int cycles){
	
	
}

void mydelayms(unsigned int ms){
	int i = 0;
	for(i = 0; i < ms; i++){
		delaycycles(1000);
	}
}


void makeNoise(int x){
	//if 1, happy noise;
	if(x == 1){
		
	}
	else{
		
	}
}

void printStars(char *buffer, char * password){
	lcd_iwrite(LCD_CLR);
	for (i = 0; i < (buffer - password); i++){
		printf("*");				
	}
}

void printError(int x, int y){
lcd_iwrite(LCD_CLR);
	if (x == 1){
			printf("OPEN LOCK);
			mydelayms(y);
		}
	}
	else{
		for (int i = 0; i < 4; i++){
			printf("ERROR");
			mydelayms(y);
			lcd_iwrite(LCD_CLR);
			mydelayms(y);
		}
	}
}
