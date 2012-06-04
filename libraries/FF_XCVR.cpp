/*File for Firefly XCVR Functionality*/
#include "FF_XCVR.h"
#include <avr/interrupt.h>

//Constants

//Wait Times
#define BIT_TIME_uS 102

//Buffers
#define MAXBUFFERSIZE 20

//File Globals
unsigned char xcvr_buffer[MAXBUFFERSIZE];
unsigned char buffer_head = 0;
unsigned char buffer_tail = 0;


//Private Fuctional Prototypes
void RX_on(void);
void TX_on(void);
inline void tunedDelay(unsigned char);
//////////////////////////////


//Public Functions
void XCVR_init(void)
{
	pinMode(TX_nRX,OUTPUT);	//set the TX/!RX select pin to output
	RX_on();				//set the xcvr to rx
	PCICR |= (1 << PCIE0);	//set up an interrupt on the data pin
	PCMSK0 |= (1 << PCINT1);
}


void XCVR_send_char(unsigned char data)
{
	TX_on();								//set the xcvr to tx
	//Start
	digitalWrite(XCVR_DATA,HIGH); 			//start bit
	delayMicroseconds(BIT_TIME_uS);			//wait one bit
	for(unsigned char i = 1;i;i<<=1)		//index bit mask
	{
		if(data&i)							//bit mask the data to tx
			digitalWrite(XCVR_DATA,HIGH);	//set data pin high
		else
			digitalWrite(XCVR_DATA,LOW); 
		delayMicroseconds(BIT_TIME_uS);		//wait a bit
		
	}	
	
	RX_on();								//turn back to rx
	delayMicroseconds(BIT_TIME_uS);			//wait at least one bit for stop
}

unsigned char XCVR_available(void)			//returns how many bytes are currently in the buffer
{
	return buffer_head>buffer_tail ? buffer_head-buffer_tail : buffer_tail-buffer_head;
}

unsigned char XCVR_read(void)				//read a byte out of buffer
{
	unsigned char temp;
	if(buffer_tail == buffer_head)			//return a 0 if there is nothing in the buffer
		return 0;
	
	temp = xcvr_buffer[buffer_tail];		//read the buffer
	//buffer maintenance
	buffer_tail = (buffer_tail + 1)%MAXBUFFERSIZE;
	
	return temp;
}


void XCVR_flush(void)
{
	buffer_tail = buffer_head;				//get rid of the buffer
}


//Private Functions

void RX_on(void)
{
	digitalWrite(XCVR_DATA,LOW);			//idle the data line low
	pinMode(XCVR_DATA,INPUT);				//set the data line to input
	digitalWrite(TX_nRX,LOW);				//set the xcvr to rx

	interrupts();							//turn interrupts back on
}

void TX_on(void)
{
	digitalWrite(TX_nRX,HIGH);				//set the xcvr to tx
	delayMicroseconds(400);					//wait for the xcvr to stabilize
	noInterrupts();							//turn of the interrupts while we tx
	pinMode(XCVR_DATA,OUTPUT);				//set the data pin to an output
}


//Interrupt Service Routines
//interrupt to read a byte into the buffer
ISR(PCINT0_vect)
{
	unsigned char d = 0;
	noInterrupts();						//turn interrupts off so it wont retrigger on further bits
	if(!digitalRead(XCVR_DATA))			//if no start bit then its not for us
	{	
		interrupts();
		return;
	}
	delayMicroseconds(BIT_TIME_uS/2);
	for (unsigned char i=1; i; i <<= 1)
    {
      delayMicroseconds(BIT_TIME_uS);
      if (digitalRead(XCVR_DATA))
        d |= i;
      else
        d &= ~i;
    }
	delayMicroseconds(BIT_TIME_uS);
	
	//add d to buffer and do buffer maintenance
	xcvr_buffer[buffer_head] = d;
	buffer_head = (buffer_head+1)%MAXBUFFERSIZE;
	if(buffer_head == buffer_tail)
	{
		buffer_tail++;
		//overflow = 1;
	}
	
	interrupts();
}

