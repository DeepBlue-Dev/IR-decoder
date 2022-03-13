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
#include "lcd.h"

uint32_t Command;
unsigned char is_locked = 0;
unsigned char command_index = 32;



int main(void)
{
	
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
	
		if(command_index == 0){
			
			lcd_clrscr();
			ultoa((Command & (uint32_t)0x1FF),buffer,16);
			lcd_puts(buffer);
			Command = 0;
			command_index = 32;	
			
		}
    }
}

ISR(INT0_vect){
	//	rising flank
	
	if((PIND & (1 << PIND2))){
		TCNT1 = 0;
	} else {
		
		uint16_t value = TCNT1;
		
		if(value > 2027 && value < 2183){	//	550 µs / (1/3686400) = 2027.52 => 2027 ticks ¤¤¤ 580 µs / (1/3686400) = 2101.248 => 2183 ticks
			Command &= ~(1 << (--command_index));
			
		} else if(value < 6257 && value > 6082) {	//	1.6975 ms / (1/3686400) = 6257.644 => 6257 ¤¤¤ 1.6500 ms / (1/3686400) = 6082.56 => 6082
			Command |= (1 << (--command_index));
			//PORTC ^= (1 << PORTC7);
		} else if((value > 8110 && value < 8478) || value > 7000){
			Command = 0;
			command_index = 32;
		}
	}
}

