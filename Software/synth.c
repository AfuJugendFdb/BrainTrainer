/*********************************
Verwendung des Mini-Squarewave-Synthesizers
Version 3
**********************************/

#include <avr/io.h>
#include "synth.h"

uint16_t tones[] = {7645,6810,6067,5727,5102,4545,4050,3822,3822,3405,3034,2863,2551,2273,2025,1911,1911,1703,1517,1432,1276,1136,1012,956,956,851,758,716,638,568,506,478,478,426,379,358,319,284,253,239,239,213,190,179,159,142,127,119,119,106,95,89,80,71,63,60,60,53,47,45,40,36,32,30,30,27,24,22,20,18,16,15,15,13,12,11,10,9,8,7};


void synth_init(void)
{
	//DDRB = 0xFF; //done globally in main.c
	
	//Timer init
	TCCR1A |= (1 << COM1A0);
	TCCR1B |= (1<<WGM12) | (1<<CS11) | (0<<CS10); //8-Teiler
	OCR1A = 0xFFFF;
	
	toneOff();
}

void playTone(uint8_t octave, uint8_t tone)
{		
	OCR1A = tones[octave*8 + tone]/2; //Ton ausgeben, Prescaler 8 statt 4
	TCNT1 = 0x00;
	TCCR1A |= (1<<COM1A0);
}

void playSound(uint16_t value)
{
	OCR1A = value; //Ton ausgeben, the raw value
	TCNT1 = 0x00;
	TCCR1A |= (1<<COM1A0);
}

void toneOff(void)
{
	//tone = 0x00;
	TCCR1A &= ~(1<<COM1A0);
}
