#include <avr\io.h>
#include "fix_fft.h"
#include "LEDCtl.h"
#define NUM_POINTS 256
#define LOG2_NUM_POINTS 8

void adc_init()
{
    ADMUX = (1<<6);//AVCC with external cap 1<<6
    ADCSRA =0b10000010;//ADC enable and prescaller of 4
}
 
char adc_read()
{
    ADCSRA |= 1<<ADSC;
    while(!ADIF);
    ADCSRA |= 1<<ADIF;
    return (ADCL>>2)|(ADCH<<6);
}

void SetupTimer()
{
	OCR0A = 42;
}



int main(void)
{
	SetupTimer();
	SetupLEDCntrl();
	adc_init();

	while(1){
		TCNT0 = 0; 
		TCCR0B = (1<<CS01);
		char samples[NUM_POINTS]; //= {1,3, 7,15,31,63,127,255};
		for ( int i = 0; i < NUM_POINTS; ++i )
		{
			samples[i] = adc_read();
			while ( !(TIFR0&OCF0A) );
			TCNT0 = 0;
			TIFR0 = 0;
		}

		fix_fftr(samples,LOG2_NUM_POINTS,0);
		char dspBuffer[16];
		for( int i = 0; i < NUM_POINTS; i+=4)
		{
			int average = samples[i];
			average+= samples[i+1];
			average+= samples[i+2];
			average+= samples[i+3];
			dspBuffer[i>>2] = average>>2;
		}		

		for( int i = 0; i < 16; i++)
		{
			if ( dspBuffer[i] < 15 ) dspBuffer[i] = 0;
			else if ( dspBuffer[i] < 42 ) dspBuffer[i] = 1;
			else if ( dspBuffer[i] < 64 ) dspBuffer[i] = 3;
			else if ( dspBuffer[i] < 96 ) dspBuffer[i] = 7;
			else if ( dspBuffer[i] < 128) dspBuffer[i] = 15;
			else if ( dspBuffer[i] < 160 ) dspBuffer[i] = 31;
			else if ( dspBuffer[i] < 192 ) dspBuffer[i] = 63;
			else if ( dspBuffer[i] < 224 ) dspBuffer[i] = 127;
			else dspBuffer[i] = 255;
		}
		writeDisplay(0, dspBuffer);
		writeDisplay(1, dspBuffer+8);

		for( long i = 0; i < 30000; ++i) asm("nop");
	}
}
