#include "LEDCtl.h"
#include <avr\io.h>

#define LEDDATA PORTD
#define LEDDDR DDRD

#define LEDCTL PORTB
#define LEDCTLDDR DDRB
#define LEDMODE 1
#define LEDW0 0
#define LEDW1 2
#define LEDW(x) (x?LEDW1:LEDW0)

#define LEDCtrlRegMode() LEDCTL |=(1<<LEDMODE)
#define LEDDspRegMode() LEDCTL &=~(1<<LEDMODE)

#define LED_ENABLE (1<<4)
#define LED_NODECODE (1<<5)
#define LED_SEQUENTIAL (1<<7)


void PulseWrite(int display)
{
	LEDCTL |= (1<<LEDW(display));
	asm("nop");
	LEDCTL &= ~(1<<LEDW(display));
}

void SetupLEDCntrl()
{
	LEDDDR = 0xFF;
	LEDCTLDDR = (1<<LEDMODE)|(1<<LEDW0)|(1<<LEDW1);
}


void writeDisplay(int display, char* buffer)
{
	LEDCtrlRegMode();
	LEDDATA = LED_ENABLE | LED_NODECODE | LED_SEQUENTIAL;
	PulseWrite(display);
	LEDDspRegMode();

	for( int digit = 0; digit < 8; digit++ )
	{
		LEDDATA = buffer[digit] ^ (1<<7);
		PulseWrite(display);
	}
}
