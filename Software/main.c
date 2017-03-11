/*************************************
* BrainTrainer: Gedächtnisspiel: Mehrere LEDs leuchten nach zufälligem Muster der Reihe nach auf, der Spieler muss dieses Muster mit den entsprechenden Tastern reproduzieren
* Version 1.1
* 10.09.16 DM1JZ
*
* LEDs an PD0-PD3, Taster an PD4-PD7
*
* ACHTUNG: alle Zugriffe auf PORTD müssen verodert werden, damit die Konfiguration der Pullup-Widerstände nicht verändert wird!
**************************************/

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include "synth.h"

uint8_t generateNumber(void);
uint8_t readKeys(void);
void playError(void);
void playCorrect(void);
void playStartup(void);
void initRandGen(void);
void playLaunchGame(void);
void playLevelUp5(void);

uint8_t pattern[100];	//Hier kommt die LED-Abfolge rein
uint8_t patternPos = 0;	//Zähler, bis zu welcher Position die aktuelle Pattern-Folge geht (wie viele LEDs leuchten auf)
uint8_t input[100];

int main(void)
{
	DDRD = 0x0F; 	//Taster Eingang, LEDs Ausgang
	PORTD = 0xF0;	//Pull-ups an Tastern aktivieren, LEDs aus
	DDRB = (0 << PB0) | (1 << PB1); //PBO: Start-Taster, PB1: Summer
	PORTB |= (1 << PB0); //Pullup an Start-Taster an
	
	synth_init();
	
	_delay_ms(200);
	playStartup();
	_delay_ms(500);
	
	initRandGen(); //Blockierend, wartet, bis Start-Button gedrückt ist
	
	playLaunchGame();
	
	
	while(1)
	{
		//Neue Zufallszahl generieren und dem Pattern hinzufügen
		pattern[patternPos] = generateNumber(); //Achtung: 0-basierend
		
		/*
		//Oder: Komplett neue Pattern-Kette erfinden:
		for(uint8_t i = 0; i <= patternPos; i++)
		{
			pattern[patternPos] = generateNumber();
		}
		*/	
		
		//LED-Muster abspielen
		for(uint8_t i = 0; i <= patternPos; i++)
		{
			PORTD |= (1 << pattern[i]);	//verodern, um Pullup-Widerstände nicht zu ändern! Bit-Stelle stimmt mit 0-basierend überein!
			_delay_ms(500);
			PORTD &= ~(1 << pattern[i]);	//entsprechende LED wieder mit AND abschalten
			_delay_ms(250);
		}
		
		//Taster einlesen
		for(uint8_t i = 0; i <= patternPos; i++)
		{
			input[i] = readKeys();
		}
		
		_delay_ms(500);
		
		//Input mit Pattern vergleichen
		uint8_t correct = 1; //true
		
		for(uint8_t i = 0; i <= patternPos; i++)
		{
			if(input[i] != pattern[i])
			{
				correct = 0; //false
			}
		}
		if(correct == 1)
		{
			patternPos++;
			
			playLevelUp5();
			_delay_ms(500);
			
			playCorrect();
			
		}
		else
		{
			playError();
		}
		
		//Verschnaufpause
		_delay_ms(2000);
	}
	
	return 0;
}

uint8_t generateNumber(void) //Achtung: 0-basierend
{
	uint8_t number = 0;
	
	number = rand()%4;
	
	/*
	do
	{
		number = rand();
	}
	while(number >= 4); //solange number >= 4, neue Zufallszahl erzeugen --> Zahl < 4 resultiert
	*/	
		
	return number;
}

uint8_t readKeys(void)
{
	static uint8_t input_new = 0xFF;
	static uint8_t input_old = 0xFF;
	
	while(1)
	{
		input_new = PIND & 0xF0;
		
		if(input_new != input_old)
		{
			input_old = input_new;
			
			if((input_new & 0b00010000) == 0)
			{
				_delay_ms(10);
				input_new = PIND & 0xF0;
				if((input_new & 0b00010000) == 0)
				{
					return 0;
				}
			}
			
			if((input_new & 0b00100000) == 0)
			{
				_delay_ms(10);
				input_new = PIND & 0xF0;
				if((input_new & 0b00100000) == 0)
				{
					return 1;
				}
			}
			
			if((input_new & 0b01000000) == 0)
			{
				_delay_ms(10);
				input_new = PIND & 0xF0;
				if((input_new & 0b01000000) == 0)
				{
					return 2;
				}
			}
			
			if((input_new & 0b10000000) == 0)
			{
				_delay_ms(10);
				input_new = PIND & 0xF0;
				if((input_new & 0b10000000) == 0)
				{
					return 3;
				}
			}
		}
	}
}

void playError(void)
{	
	for(uint8_t i = 0; i < 100; i = i+5)
	{
		playSound(200 + i);
		_delay_ms(50);
	}
	toneOff();
}

void playCorrect(void)
{
	playTone(5, 0);
	_delay_ms(100);
	playTone(5, 7);
	_delay_ms(100);
	toneOff();
}

void playStartup(void)
{
	_delay_ms(500);
	
	playTone(4, 0);
	_delay_ms(100);
	playTone(4, 1);
	_delay_ms(100);
	playTone(4, 2);
	_delay_ms(100);
	playTone(4, 3);
	_delay_ms(100);
	playTone(4, 4);
	_delay_ms(100);
	
	toneOff();
	_delay_ms(100);
	
	playTone(3, 6);
	_delay_ms(100);
	toneOff();
	_delay_ms(100);
	playTone(4, 0);
	_delay_ms(200);
	
	toneOff();
}

void initRandGen(void)
{
	uint8_t time = 0;
	
	//solange Button nicht gedrückt ist, erhöhe Zeit nach kurzer Delayzeit, bis Button gedrückt
	while((PINB & 0x01) != 0)
	{
		time++;
		_delay_ms(1);
	}
	
	srand(time);
}

void playLaunchGame(void)
{
	playTone(4, 0);
	_delay_ms(100);
	playTone(4, 2);
	_delay_ms(300);
	playTone(4, 0);
	_delay_ms(100);
	playTone(4, 4);
	_delay_ms(300);
	playTone(4, 2);
	_delay_ms(100);
	playTone(4, 7);
	_delay_ms(500);
	toneOff();
}

void playLevelUp5(void)
{
	if((patternPos % 5) == 0)
	{
		for(uint8_t i  = 0; i < 8; i++)
		{
			playTone(4, i);
			_delay_ms(50);
		}
		playTone(5, 0);
		_delay_ms(250);
		toneOff();
	}
}
