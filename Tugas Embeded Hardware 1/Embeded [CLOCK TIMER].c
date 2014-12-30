/*----------------------------------------------------------------------------------------/

   									CLOCK TIMER    
						       TUGAS EMBEDED [EC-4060]

 							Oleh : - Istantyo [13204102]
  								   - M Zein   [13204116]
/----------------------------------------------------------------------------------------*/



#include <reg51.h>               // special function register declarations for 89s51                    

#define TH 0xee					 //	timer0 HighByte reload
#define TL 0x00					 // timer0 LowByte reload
 								 // interupt timer setiap 10ms
#define RELEASED 0				 // button state
#define PRESSED  1				 // button state
#define SS_PORT	 P0				 // seven segmen port
#define MUX_PORT P2				 // multiplexing port for seven segment

unsigned char convert[10] = {0x3F,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f}; //seven segment decoder 
unsigned int digit=0, digit0=0, digit1=0, digit2=0, digit3=0;  	//to store seven segment digit data
unsigned int minute_on=0, hour_on=0, minute_off=0, hour_off=0; 	//to store time  to turn on and off
unsigned int sec100=0,second=0,minute=0,hour=0;					//to store the time
unsigned int mode=0, blink=0, unblink=50, sample_button=0;
unsigned char Data[4]={0,0,0,0};								
unsigned int dot_on=0, automode=0;

sbit B1 = P3^6;	 //Button1 pin
sbit B2 = P3^7;	 //Button2 pin

sbit OUT = P1^7;	 //Output pin to Drive the the led

sbit M_ON  = P1^5;	 //pin to led mode set waktu on 
sbit M_OFF = P1^6;	 //pin to led mode set waktu off

void update_time();
void SampleButton1(void);
void SampleButton2(void);
void Button1PressAction();
void blink_minute();
void blink_hour();
void check_n_set_minute_on();
void check_n_set_hour_on();
void check_n_set_minute_off();
void check_n_set_hour_off();
void check_n_set_minute_waktu();
void check_n_set_hour_waktu();
void set_digit_on();
void set_digit_off();
void set_digit_waktu();
void Display();
void check_n_set_timer();
void blink_dot();


//////////////////////////////////////////////////////////////////////////////
void main(){
   	EA 		= 1 ;
   	ET0 	= 1 ;  			// set bit EA and Timer0 enable 
   	TMOD 	= 0x01; 		// timer 0 and 1 run 16 bit counter 
   	TR0  	= 1 ; 			// run timer 0 
   	OUT  	= 0 ;			// output led off
   	M_ON	= 0 ;			// led mode set waktu_on = off 
   	M_OFF	= 0 ;			// led mode set waktu_off = off 
	MUX_PORT= 0x00;			// turn off seven segment

	while(1){				// forever loop
	}
}
////////////////////////////////////////////////////////////////////////////
void timer0int (void)  interrupt 1 using 1   {
	TH0 = TH;  	//reload timer
	TL0 = TL;
	
				if(++sample_button>=5){
					SampleButton1(); //read and debounce input Button1 
					SampleButton2(); //read	and debounce input Button2
					sample_button=0;
				}

				update_time(); 
		
				//store data that will be send to seven segment port
				Data[0]=~convert[digit0];
				Data[1]=~convert[digit1];
				Data[2]=~convert[digit2];
				Data[3]=~convert[digit3];
				
				switch(mode){
				 	case 0:
						blink_dot();
				 		break;
				 	case 1:
						blink_minute();
						break;
					case 2:
						blink_hour();
						break;
					case 3:
						blink_minute();
						break;
					case 4:
						blink_hour();
						break;
					case 5:
						blink_minute();
						break;	
					case 6:
						blink_hour();
						break;
				}

				Display(); //Multiplexing routine to 7 segment						
}

/////////////////////////////////////////////////////////////////////////
void update_time(){
	if (++sec100>=200){
		dot_on=50;
		sec100=0;
		++second;
		if(second>=60){
			second=0;
			++minute;
			check_n_set_minute_waktu();
			check_n_set_timer();			
		}	
	}
}

//////////////////////////////////////////////////////////////////////////
void Button1PressAction(){	
	switch(mode){
	case 0:
		break;
	case 1:
		second = 0;
		++minute;
		check_n_set_minute_waktu();
		break;
	case 2:
		second = 0;
		++hour;
		check_n_set_hour_waktu();
		break;
	case 3:
	 	++minute_on;	
		check_n_set_minute_on();
		break;

	case 4:
		 ++hour_on;
		check_n_set_hour_on();
		break;

	case 5:
		++minute_off;
		check_n_set_minute_off();
		break;
	 case 6:
	 	++hour_off;
		check_n_set_hour_off();
		break;
	}
	automode=0;
}

///////////////////////////////////////////////////////////
void Button2PressAction(){
	 mode++;

	 if (mode>=7){
	 mode=0;
	 }

	 switch(mode){
	 case 0:
	 	M_OFF=0;
	 	set_digit_waktu();
		break;
	 case 1:
	 case 2:
		set_digit_waktu();
		break;
	 case 3:
	 case 4:
	 	M_ON=1;
		set_digit_on();
		break;
	 case 5:
	 case 6:
	 	M_ON=0;
	 	M_OFF=1; 
		set_digit_off();
		break;

	 }
}

//////////////////////////////////////////////////////////
void SampleButton1()
{
  static int oldState = RELEASED;
  int newState;

  newState = B1;      			// Read Button1 pin
  if (oldState == RELEASED && newState == PRESSED)
    	Button1PressAction();  	// Call the button1 press routine 

  oldState = newState;
}

/////////////////////////////////////////////////////////////
void SampleButton2(){
  static int oldState2 = RELEASED;
  int newState2;

  newState2 = B2;      			// Read Button2 pin
  if (oldState2 == RELEASED && newState2 == PRESSED)
   		Button2PressAction();  	// Call the button2 press routine 
 
  oldState2 = newState2;
}

////////////////////////////////////////////////////////////
void blink_minute(){
if (automode<=5){
		if(unblink<=100){
			Data[0]=0xff;
			Data[1]=0xff;
			++unblink;
		}else if(blink>100){ 
			unblink=0;
			automode++;
		}

		if(blink<=100){
			++blink;
		}else if(unblink>100){
			blink=0;
		}
	}
	else {
	automode=0;
	mode=0;
	set_digit_waktu();
	}
}

////////////////////////////////////////////////////////////
void blink_hour(){
if (automode<=5){
		if(unblink<=100){
			Data[2]=0xff;
			Data[3]=0xff;
			++unblink;
		}else if(blink>100){ 
			unblink=0;
			automode++;
		}

		if(blink<=100){
			++blink;
		}else if(unblink>100){
			blink=0;
		}
	}
	else {
	automode=0;
	mode=0;
	set_digit_waktu();
	}
}

////////////////////////////////////////////////////////////
void blink_dot(){
	if(dot_on>0){
		Data[2]&=0x7f;
		--dot_on;
	}
}
/////////////////////////////////////////////////////////////
void check_n_set_minute_on(){
 	if(minute_on>=60){
	   minute_on=0;
	   ++hour_on;
	   check_n_set_hour_on();
	}
	
	digit1=	minute_on/10;
	digit0= minute_on%10;

}

/////////////////////////////////////////////////////////////
void check_n_set_hour_on(){		
		if(hour_on>=24){
		   hour_on=0;
		}
		digit3= hour_on/10;
		digit2= hour_on%10;
}

/////////////////////////////////////////////////////////////
void check_n_set_minute_off(){
	if(minute_off>=60){
	   minute_off=0;
	   ++hour_off;
	   check_n_set_hour_off();
	}
	
	digit1=	minute_off/10;
	digit0= minute_off%10;
}

/////////////////////////////////////////////////////////////
void check_n_set_minute_waktu(){
	if(minute>=60){
		minute=0;
		++hour;
		check_n_set_hour_waktu();
	}

	digit1=	minute/10;
	digit0= minute%10;
}

/////////////////////////////////////////////////////////////
void check_n_set_hour_waktu(){
	if(hour>=24){
		hour=0;
	}
   	digit3= hour/10;
	digit2= hour%10;
}

///////////////////////////////////////////////////////////
void check_n_set_hour_off(){
	if(hour_off>=24){
		hour_off=0;
	}
	digit3= hour_off/10;
	digit2= hour_off%10;
}

//////////////////////////////////////////////////////////
void set_digit_on(){
	digit3= hour_on/10;
	digit2= hour_on%10;
   	digit1=	minute_on/10;
	digit0= minute_on%10;
}

//////////////////////////////////////////////////////////
void set_digit_off(){
	digit3= hour_off/10;
	digit2= hour_off%10;
  	digit1=	minute_off/10;
	digit0= minute_off%10;
}

///////////////////////////////////////////////////////////
void set_digit_waktu(){
 	digit3= hour/10;
	digit2= hour%10;
	digit1=	minute/10;
	digit0= minute%10;		  
}

///////////////////////////////////////////////////////////
void Display(){
	digit++ ;                       // next digit
    SS_PORT = 0x00 ;                // turn 7 segment off
            if(digit>3)             // last digit 
            {
				digit=0;
                MUX_PORT = 0x01;    // light the first digit
                   }
            else
            {
                MUX_PORT <<= 1 ;    // light the next digit
			}

			SS_PORT=Data[digit];	// assign 7 segments value
}

/////////////////////////////////////////////////////////////
void check_n_set_timer(){
	if((minute==minute_on)&&(hour==hour_on)){
		OUT = 1;
	}

	if((minute==minute_off)&&(hour==hour_off)){
		OUT = 0;
	}

}///////////////////////////////////////////////////////////