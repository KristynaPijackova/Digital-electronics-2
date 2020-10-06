/***********************************************************************
 * 
 * Blink 2 LEDs alternately when a push button is pressed.
 * ATmega328P (Arduino Uno), 16 MHz, AVR 8-bit Toolchain 3.6.2
 *
 * Copyright (c) 2018-2020 Tomas Fryza
 * Dept. of Radio Electronics, Brno University of Technology, Czechia
 * This work is licensed under the terms of the MIT license.
 * 
 **********************************************************************/

/* Defines -----------------------------------------------------------*/
#define LED_GREEN   PB5     // AVR pin where green LED is connected
#define LED_RED     PC0     // AVR pin where red LED is connected
#define PUSH_BUTTON PD0
#define SHORT_DELAY 250     // Delay in miliseconds
#ifndef F_CPU
#define F_CPU 16000000      // CPU frequency in Hz required for delay func
#endif

/* Includes ----------------------------------------------------------*/
#include <util/delay.h>     // Functions for busy-wait delay loops
#include <avr/io.h>         // AVR device-specific IO definitions


/* Functions ---------------------------------------------------------*/
/**
 * Toggle two LEDs when a push button is pressed and use the function 
 * from the delay library.
 */
int main(void)
{
    // Set pin as output in Data Direction Register
    DDRB = DDRB | (1<<LED_GREEN);
    // Set pin LOW in Data Register (LED off)
    PORTB = PORTB & ~(1<<LED_GREEN);

    // Set pin as output in Data Direction Register
    DDRC = DDRC | (1<<LED_RED);
    // Set pin LOW in Data Register (LED off)
    PORTC = PORTC & ~(1<<LED_RED);

    // Set pin as input in Data Direction Register
    DDRD =  DDRD & ~(1<<PUSH_BUTTON);
    // Set pull-up resistor on 
    PORTD = PORTD | (1<<PUSH_BUTTON);

    // Infinite loop
    while (1)
    {

        // Pause for several milliseconds
        _delay_ms(SHORT_DELAY);

        if (bit_is_clear(PIND, PUSH_BUTTON))
        {

            // Toggle the LEDs
            PORTB = PORTB ^ (1<<LED_GREEN);
            PORTC = PORTC ^ (1<<LED_RED);
        } 
    }
    
    // Will never reach this
    return 0;
}

/* Interrupt routines ------------------------------------------------*/
