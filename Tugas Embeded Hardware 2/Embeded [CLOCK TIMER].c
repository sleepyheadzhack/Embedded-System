#include <reg51.h>               // special function register declarations for 89s51 
#include <stdio.h>

#define TH 0xee					 //	set timer0 HighByte and LowByte to interrupt every 5 ms
#define TL 0x00					 

#define RELEASED 0				 // button state
#define PRESSED  1				 // button state

#define TRUE 1 					 //boolean state         
#define FALSE 0					 //boolean state

#define    XTAL 11059200 		         // CPU Oscillator Frequency
#define    baudrate 1200 		         // 9600 bps communication baudrate

#define    OLEN  8 					// size of serial transmission buffer
unsigned   char  ostart; 		    // transmission buffer start index
unsigned   char  oend; 		        // transmission buffer end index
char idata outbuf[OLEN];	 	    // storage for transmission buffer

#define    ILEN  8 					// size of serial receiving buffer
unsigned   char  istart; 		    // receiving buffer start index
unsigned   char  iend; 		        // receiving buffer end index
char idata inbuf[ILEN]; 		    // storage for receiving buffer

bit sendfull; 			         	// flag: marks transmit buffer full
bit sendactive; 			        // flag: marks transmitter active

#define outport P0				  	//led output on P1

sbit col1 = P2^3;       					//				        P2.1 P2.2 P2.3
sbit col2 = P2^2;       					//					      |	   |	|
sbit col3 = P2^1;       					//				        ---------------
sbit row1 = P2^7;				       		//			P2.4	--> | 1 || 2 || 3 |
sbit row2 = P2^6;							//			P2.5	--> | 4 || 5 || 6 |
sbit row3 = P2^5;							//			P2.6	--> | 7 || 8 || 9 |
sbit row4 = P2^4;							//			P2.7	--> | * || 0 || # |
											//						---------------

void get_key(void);					// Function to get pressed key 
void keypad_init(void);				// initialisation for keypad

unsigned int sample_button=0;		// Function for keypad debounce using sampling method
void SampleButton1();
void SampleButton2();
void SampleButton3();
void SampleButton4();
void SampleButton5();
void SampleButton6();
void SampleButton7();
void SampleButton8();
void SampleButton9();
void SampleButtonAsterix();
void SampleButton0();
void SampleButtonCres();

void serial_init(void);		 	// initialisation for serial

void main(){
		 		
   	EA 		= 1 ;				// global interupt enable
	outport = 0x00 ;			// output led off
	keypad_init();
	serial_init();

	while(1){					// forever loop
		char c;
    	c = getchar ();
    	printf ("\nYou typed the character %c.\n", c);
	}
}

/*
 * Timer0 Interupt Service Routine to scaning the keypad
 */
void timer0int (void)  interrupt 1 using 1   {
	TH0 = TH;  	//reload timer0
	TL0 = TL;
		if(++sample_button>=5){		//scanning keypad every 25 ms
			get_key();
			sample_button=0;
		}
}

/*
 * Function to initialize the keypad port and timer0.
 */
void keypad_init(){
	ET0 	= 1 ;  			// set Timer0 enable for scanning keypad routine 
   	TMOD 	= 0x01; 		// timer 0 and 1 run 16 bit counter 
   	TR0  	= 1 ; 			// run timer 0
	P2 = 0xff;

}

/*
 * Function to initialize the serial port and the UART baudrate.
 */
void serial_init(){
	istart = 0; 			        // empty receive buffers
  	iend = 0;
  	ostart = 0; 			        // empty transmit buffers
  	oend = 0;
  	sendactive = 0; 			    // transmitter is not active
  	sendfull = 0;
	
	PCON |= 0x80; 			        // 0x80=SMOD: set serial baudrate doubler
  	TMOD |= 0x20; 			        // put timer 1 into MODE 2
  	TH1 = (unsigned char) (256 - (XTAL / (16L * 12L * baudrate)));	//set timer 1 to get serial baudrate
  	TR1 = 1; 						// start timer 1  
  	SCON = 0x50;			        // serial port MODE 1, enable serial receiver
  	ES = 1; 						// enable serial interrupts
}

/*
 * Function to scanning the keypad.
 */
void get_key(){
        unsigned char i;
        	for(i=0;i<4;i++){               //set every pin
					if(i==0){
					row1 = 0;
					row2 = 1;
					row3 = 1;
					row4 = 1;
						SampleButton1();
						SampleButton2(); 
						 SampleButton3(); 
					}else if(i==1){
					row1 = 1;
					row2 = 0;
					row3 = 1;
					row4 = 1;
						SampleButton4();
						SampleButton5(); 
						SampleButton6(); 
					}else if(i==2){
					row1 = 1;
					row2 = 1;
					row3 = 0;
					row4 = 1;
						SampleButton7();
						SampleButton8(); 
						SampleButton9(); 
					}else if(i==3){
					row1 = 1;
					row2 = 1;
					row3 = 1;
					row4 = 0;
						SampleButtonAsterix();
						SampleButton0(); 
						SampleButtonCres();
					}
					
        }
}


/*
 * putbuf: write a character to SBUF or transmission buffer
 */

void putbuf (char c) {
	if (!sendfull) { 					// transmit only if buffer not full
    	if (!sendactive) { 		        // if transmitter not active:
      		sendactive = 1; 			// transfer first character direct
      		SBUF = c; 			      	// to SBUF to start transmission
    	}
    	else {
      		ES = 0; 								// disable serial interrupts during buffer update
      		outbuf[oend++ & (OLEN-1)] = c; 			// put char to transmission buffer
      		if (((oend ^ ostart) & (OLEN-1)) == 0) {
         		sendfull = 1;
      		} 				         				// set flag if buffer is full
      		ES = 1; 								// enable serial interrupts again
    	}
  	}
}

/*
 * Replacement routine for the standard library putchar routine.
 * The printf function uses putchar to output a character.
 */

char putchar (char c) {
	if (c == '\n') { 			// expand new line character:
    	while (sendfull); 		// wait until there is space in buffer
    	putbuf (0x0D); 			// send CR before LF for <new line>
  	}
  	while (sendfull); 			// wait until there is space in buffer
  	putbuf (c); 			    // place character into buffer
  	return (c);
}

/*
 * Replacement routine for the standard library _getkey routine.
 * The getchar and gets functions uses _getkey to read a character.
 */

char _getkey (void) {
	char c;
  	while (iend == istart);			// wait until there are characters
  	ES = 0; 						// disable serial interrupts during buffer update
  	c = inbuf[istart++ & (ILEN-1)];
  	ES = 1; 						// enable serial interrupts again
  	return (c);
}

/*
 * Serial Interrupt Service Routine
 */

static void com_isr (void) interrupt 4 using 1 {
/*------ Transmitted data interrupt. ------------------------------------*/
	if (TI != 0) {
    	TI = 0; 								// clear interrupt request flag
    	if (ostart != oend) { 		         	// if characters in buffer and
      		SBUF = outbuf[ostart++ & (OLEN-1)]; // transmit character
      		sendfull = 0; 						// clear 'sendfull' flag
    	}
    	else { 									// if all characters transmitted
      		sendactive = 0; 					// clear 'sendactive'
    	}
  	}
}


void SampleButton1()
{

  static int oldState = RELEASED;
  int newState;

  newState = ~col1;      			// Read Button1 pin
  if (oldState == RELEASED && newState == PRESSED){
  	outport=0x01;
	inbuf[iend++ & (ILEN-1)] = '1';  	
   }
  oldState = newState;

}


void SampleButton2(){

  static int oldState2 = RELEASED;
  int newState2;

  newState2 = ~col2;      			// Read Button2 pin
  if (oldState2 == RELEASED && newState2 == PRESSED){
   outport=0x02;
   inbuf[iend++ & (ILEN-1)] = '2';  
   }
  oldState2 = newState2;
}

void SampleButton3(){
  static int oldState3 = RELEASED;
  int newState3;

  newState3 = ~col3;      			// Read Button2 pin
  if (oldState3 == RELEASED && newState3 == PRESSED){
   outport=0x03;
   inbuf[iend++ & (ILEN-1)] = '3';  
   }
  oldState3 = newState3;
}

void SampleButton4(){

  static int oldState4 = RELEASED;
  int newState;

  newState = ~col1;      			// Read Button1 pin
  if (oldState4 == RELEASED && newState == PRESSED){
  	outport=0x04;
	inbuf[iend++ & (ILEN-1)] = '4';  	
   }
  oldState4 = newState;
}

void SampleButton5(){

  static int oldState5 = RELEASED;
  int newState;

  newState = ~col2;      			// Read Button1 pin
  if (oldState5 == RELEASED && newState == PRESSED){
  	outport=0x05;
	inbuf[iend++ & (ILEN-1)] = '5';  	
   }
  oldState5 = newState;
}

void SampleButton6(){

  static int oldState6 = RELEASED;
  int newState;

  newState = ~col3;      			// Read Button1 pin
  if (oldState6 == RELEASED && newState == PRESSED){
  	outport=0x06;
	inbuf[iend++ & (ILEN-1)] = '6';  	
   }
  oldState6 = newState;

}

void SampleButton7(){

  static int oldState7 = RELEASED;
  int newState;

  newState = ~col1;      			// Read Button1 pin
  if (oldState7 == RELEASED && newState == PRESSED){
  	outport=0x07;
	inbuf[iend++ & (ILEN-1)] = '7';  	
   }
  oldState7 = newState;

}

void SampleButton8(){
  static int oldState8 = RELEASED;
  int newState;

  newState = ~col2;      			// Read Button1 pin
  if (oldState8 == RELEASED && newState == PRESSED){
  	outport=0x08;
	inbuf[iend++ & (ILEN-1)] = '8';  	
   }
  oldState8 = newState;

}

void SampleButton9(){

	static int oldState9 = RELEASED;
  	int newState;

  	newState = ~col3;      			// Read Button2 pin
  	if (oldState9 == RELEASED && newState == PRESSED){
  		 outport=0x09;
		 inbuf[iend++ & (ILEN-1)] = '9';  
   	}
  	oldState9 = newState;	
}

void SampleButtonAsterix(){

	static int oldStateAsterix = RELEASED;
  	int newState;

  	newState = ~col1;      			// Read Button2 pin
  	if (oldStateAsterix == RELEASED && newState == PRESSED){
  		 outport=0x10;
		 inbuf[iend++ & (ILEN-1)] = '*';  
   	}
  	oldStateAsterix = newState;	
}

void SampleButton0(){

	static int oldState0 = RELEASED;
  	int newState;

  	newState = ~col2;      			// Read Button2 pin
  	if (oldState0 == RELEASED && newState == PRESSED){
  		 outport=0x11;
		 inbuf[iend++ & (ILEN-1)] = '0';  
   	}
  	oldState0 = newState;	
}

void SampleButtonCres(){

	static int oldStateCres = RELEASED;
  	int newState;

  	newState = ~col3;      			// Read Button2 pin
  	if (oldStateCres == RELEASED && newState == PRESSED){
  		 outport=0x12;
		 inbuf[iend++ & (ILEN-1)] = '#';  
   	}
  	oldStateCres = newState;	
}