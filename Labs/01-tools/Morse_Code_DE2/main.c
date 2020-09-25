/***********************************************************************
 * 
 * Blink a LED and use the function from the delay library.
 * ATmega328P (Arduino Uno), 16 MHz, AVR 8-bit Toolchain 3.6.2
 *
 * Copyright (c) 2018-2020 Tomas Fryza
 * Dept. of Radio Electronics, Brno University of Technology, Czechia
 * This work is licensed under the terms of the MIT license.
 * 
 **********************************************************************/

/* Defines -----------------------------------------------------------*/
#define LED_GREEN       PB5     // AVR pin where green LED is connected
#define DOT_DELAY       500      // Delay in miliseconds for dot          =
#define DASH_DELAY      1500     // Delay in miliseconds for dash         ===
#define SYMBOL_DELAY    300      // Delay in miliseconds for symbol space .
#define LETTER_DELAY    900      // Delay in miliseconds for letter space ...
#define WORD_DELAY      2100     // Delay in miliseconds for word space   ....... 
#ifndef F_CPU
#define F_CPU     16000000      // CPU frequency in Hz required for delay func
#endif

/* Includes ----------------------------------------------------------*/
#include <util/delay.h>     // Functions for busy-wait delay loops
#include <avr/io.h>         // AVR device-specific IO definitions

/* Variables ---------------------------------------------------------*/

/* Function prototypes -----------------------------------------------*/

/* Functions ---------------------------------------------------------*/
/**
 * Toggle one LED and use the function from the delay library.
 */
int main(void)
{
    // Set pin as output in Data Direction Register
    // DDRB = DDRB or 0010 0000
    DDRB = DDRB | (1<<LED_GREEN);

    // Set pin LOW in Data Register (LED off)
    // PORTB = PORTB and 1101 1111
    PORTB = PORTB & ~(1<<LED_GREEN);

    // Infinite loop
    // DE2 in Morse Code
    // ===.=.=...=...=.=.===.===.===.......*repeat*
    while (1)
    {
        // Pause several miliseconds
        _delay_ms(WORD_DELAY);

        // Invert LED in Data Register
        // PORTB = PORTB ^ (1<<LED_GREEN); turns the led on and off

        // D ===.=.=...
        PORTB = PORTB ^ (1<<LED_GREEN);     // on - dash
        _delay_ms(DASH_DELAY);        
        PORTB = PORTB ^ (1<<LED_GREEN);     // off 
        _delay_ms(SYMBOL_DELAY);
        PORTB = PORTB ^ (1<<LED_GREEN);     // on - dot
        _delay_ms(DOT_DELAY);
        PORTB = PORTB ^ (1<<LED_GREEN);     // off
        _delay_ms(SYMBOL_DELAY);
        PORTB = PORTB ^ (1<<LED_GREEN);     // on - dot
        _delay_ms(DOT_DELAY);
        PORTB = PORTB ^ (1<<LED_GREEN);     // off - letter space
        _delay_ms(LETTER_DELAY);
        // E =...
        PORTB = PORTB ^ (1<<LED_GREEN);     // on - dot
        _delay_ms(DOT_DELAY);
        PORTB = PORTB ^ (1<<LED_GREEN);     // off - letter space
        _delay_ms(LETTER_DELAY);
        // 2 =.=.===.===.===.......
        PORTB = PORTB ^ (1<<LED_GREEN);     // on - dot
        _delay_ms(DOT_DELAY);
        PORTB = PORTB ^ (1<<LED_GREEN);     // off
        _delay_ms(SYMBOL_DELAY);
        PORTB = PORTB ^ (1<<LED_GREEN);     // on - dot
        _delay_ms(DOT_DELAY);
        PORTB = PORTB ^ (1<<LED_GREEN);     // off
        _delay_ms(SYMBOL_DELAY);
        PORTB = PORTB ^ (1<<LED_GREEN);     // on - dash
        _delay_ms(DASH_DELAY);        
        PORTB = PORTB ^ (1<<LED_GREEN);     // off 
        _delay_ms(SYMBOL_DELAY);
        PORTB = PORTB ^ (1<<LED_GREEN);     // on - dash
        _delay_ms(DASH_DELAY);        
        PORTB = PORTB ^ (1<<LED_GREEN);     // off 
        _delay_ms(SYMBOL_DELAY);
        PORTB = PORTB ^ (1<<LED_GREEN);     // on - dash
        _delay_ms(DASH_DELAY);        
        PORTB = PORTB ^ (1<<LED_GREEN);     // off - word space (at the beginning)
    }

    // Will never reach this
    return 0;
}

/* Interrupt routines ------------------------------------------------*/
