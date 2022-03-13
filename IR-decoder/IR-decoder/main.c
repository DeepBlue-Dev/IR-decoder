/*
 * IR-decoder.c
 *
 * Created: 13/03/2022 14:29:06
 * Author : arthu
 */ 


#define F_CPU 3686400L

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
//	THIS PROJECT CONTAINS A MODIFIED LCD.H LIBRARY//
#include "lcd.h"
//	THIS PROJECT CONTAINS A MODIFIED LCD.H LIBRARY//
uint32_t Command;
unsigned char is_locked = 0;
unsigned char command_index = 32;
unsigned char mode = 0;	//	if mode == 0 => choose led via remote, if mode == 1 => show code on portc


int main(void)
{
	DDRC = 0xFF;	//	PORTC all outputs
	DDRA &= ~(1 << DDRC0); //	PORTA1 as input	
    // lcd setup
	lcd_init(LCD_DISP_ON);

	//	timer1 mode 0, no prescaler, no interrupts
	TCCR1B &= ~((1 << WGM13) | (1 << WGM12) | (1 << CS11) | (1 << CS12));
	TCCR1A &= ~((1 << WGM11) | (1 << WGM10));
	TCCR1B |= (1 << CS10);

	//	INT0, trigger on rising and falling flank, interrupt enabled
	EICRA |= (1 << ISC00);
	EICRA &= ~(1 << ISC01);
	EIMSK |= (1 << INT0);

	sei();
	//	buffer for string functions
	 char buffer[32];

    while (1) 
    {
		if(PINA & (1 << PINA0)){	//	switch mode
			mode = 0;
			while(PINA & (1 << PINA0));
		}
		if(PINA & (1 << PINA1)){	//	switch mode
			mode = 1;
			while(PINA & (1 << PINA1));
		}
		if(command_index == 0){	//	if index reached the last bit
			
			lcd_clrscr();	//	clear lcd screen
			ultoa((Command & (uint32_t)0x1FE),buffer,10);	//	mask the NEC frame to get the bits we want, convert to ASCII
			lcd_puts(buffer);	//	send parsed ASCII to lcd
			if(mode){
				PORTC = (Command & (uint32_t)0x1FE);	//	show command on portc if mode is not 0
				Command = 0;	//	reset Command
				command_index = 32;		//	reset index
				continue;
			}
			switch ((Command & (uint32_t)0x1FE))	//	this is a really ugly and easy way to find the led that corresponds with the code
			{
			case 150:
				PORTC = 0x01;
				break;
			case 206:
				PORTC = 0x02;
				break;
			case 230:
				PORTC = 0x04;
				break;
			case 132:
				PORTC = 0x08;
				break;
			case 238:
				PORTC = 16;
				break;
			case 198:
				PORTC = 32;
				break;
			case 164:
				PORTC = 64;
				break;
			case 188:
				PORTC = 128;
				break;
			}
			
			Command = 0;	//	reset Command
			command_index = 32;		//	reset index
			
		}
    }
}

ISR(INT0_vect){
	//	rising flank
	if(command_index == 0){return;}
	if((PIND & (1 << PIND2))){	//	on rising flank, since the output of the sensor is somehow inverted
		TCNT1 = 0;	//	reset counter value
	} else {
		
		uint16_t value = TCNT1;	//	get the value from TCNT1
		
		if(value > 2027 && value < 2183){	//	550 µs / (1/3686400) = 2027.52 => 2027 ticks ¤¤¤ 580 µs / (1/3686400) = 2101.248 => 2183 ticks
			Command &= ~(1 << (--command_index));	//	set corresponding bit to 0
			
		} else if(value < 6257 && value > 6082) {	//	1.6975 ms / (1/3686400) = 6257.644 => 6257 ¤¤¤ 1.6500 ms / (1/3686400) = 6082.56 => 6082
			Command |= (1 << (--command_index));	//	set corresponding bit to 1
			
		} else if((value > 8110 && value < 8478) || value > 7000){
			Command = 0;	//	reset command
			command_index = 32;	//	reset index
		}
	}
}

