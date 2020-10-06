/***********************************************************************
 * 
 * Knight Rider style LEDs program.
 * ATmega328P (Arduino Uno), 16 MHz, AVR 8-bit Toolchain 3.6.2
 *
 * Copyright (c) 2018-2020 Tomas Fryza
 * Dept. of Radio Electronics, Brno University of Technology, Czechia
 * This work is licensed under the terms of the MIT license.
 * 
 **********************************************************************/

/* Defines -----------------------------------------------------------*/
#define PUSH_BUTTON  PD0			
#define BLINK_DELAY  250
#ifndef F_CPU
#define F_CPU 16000000      // CPU frequency in Hz required for delay
#endif

/* Includes ----------------------------------------------------------*/
#include <util/delay.h>     // Functions for busy-wait delay loops
#include <avr/io.h>         // AVR device-specific IO definitions

/* Functions ---------------------------------------------------------*/

int main(void)
{	
	// PUSH BUTTON 

	// Set pin as input in Data Direction Register
    DDRD =  DDRD & ~(1<<PUSH_BUTTON);
    // Set pull-up resistor on 
    PORTD = PORTD | (1<<PUSH_BUTTON);
	
	// LEDs

    // Set pins as output
	DDRB = DDRB | (0b00011111); 
	//Turning off LEDs
	PORTB = PORTB | (0b11100000);
	
	
    // Infinite loop
    while (1)
    {
        // When button is pressed (0) start the Knight Rider lights
		while (bit_is_clear(PIND,PUSH_BUTTON))
		{
			for (int i = 0; i < 5; i++)
			{
				_delay_ms(BLINK_DELAY);
                // Moves LED left
				PORTB = ((0b00000001)<<i); 
			}
			for (int i = 0; i < 5; i++)
			{
				_delay_ms(BLINK_DELAY);
				// Moves LED right
                PORTB = ((0b00010000)>>i);
            }			
		}
    }

    // Will never reach this
    return 0;
}
