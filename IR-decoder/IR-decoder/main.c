/*
 * IR-decoder.c
 *
 * Created: 13/03/2022 14:29:06
 * Author : arthu
 */ 
 #define F_CPU 3686400L
 #define T_CPUCYCLE (1.0 / F_CPU)

#include "lcd.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

uint16_t T_command[32];
unsigned char is_locked = 0;
unsigned char index = 0;



int main(void)
{
	DDRC = 0xFF;	//	PORTC all outputs
    // lcd setup
	lcd_init(LCD_DISP_ON);

	//	timer1 mode 0, no prescaler, no interrupts
	TCCR1B &= ~((1 <<WGM13) | (1 << WGM12) | (1 << CS11) | (1 << CS12));
	TCCR1A &= ~((1 << WGM11) | (1 << WGM10));
	TCCR1B |= (1 << CS10);

	//	INT0, trigger on rising and falling flank, interrupt enabled
	EICRA |= (1 << ISC00);
	EICRA &= ~(1 << ISC01);
	EIMSK |= (1 << INT0);

	//	buffer for string functions
	unsigned char buffer[16];

    while (1) 
    {
		if(index == 32){
			PORTC = 	
		}
    }
}

ISR(INT0_vect){
	//	rising flank
	if(PIND & (1 << PIND2)){
		TCNT1 = 0;
	} else {
		static uint16_t value = TCNT1;
		if(value > 16600){return;}	//	4.5ms / (1/3686400) = 16588.5 => 16600 ticks
			
		T_command[index] = TCNT1;	//	store timing in array
		index++;
	}
}

