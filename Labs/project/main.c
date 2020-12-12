/***********************************************************************
 * 
 * Analog-to-digital conversion with displaying result on LCD and 
 * transmitting via UART.
 * ATmega328P (Arduino Uno), 16 MHz, AVR 8-bit Toolchain 3.6.2
 *
 * Copyright (c) 2018-2020 Tomas Fryza
 * Dept. of Radio Electronics, Brno University of Technology, Czechia
 * This work is licensed under the terms of the MIT license.
 * 
 **********************************************************************/


/* Defines -----------------------------------------------------------*/
#define DAC_0        PB0     // AVR pin where DAC is connected
#define DAC_1        PB1     // AVR pin where DAC is connected
#define DAC_2        PB2     // AVR pin where DAC is connected
#define DAC_3        PB3     // AVR pin where DAC is connected
#define DAC_4        PB4     // AVR pin where DAC is connected
#define DAC_5        PB5     // AVR pin where DAC is connected
#define DAC_6        PB6     // AVR pin where DAC is connected
#define DAC_7        PB7     // AVR pin where DAC is connected

/* Includes ----------------------------------------------------------*/
#include <avr/io.h>         // AVR device-specific IO definitions
#include <avr/interrupt.h>  // Interrupts standard C library for AVR-GCC
#include "timer.h"         // Timer library for AVR-GCC
#include "lcd.h"           // Peter Fleury's LCD library
#include <stdlib.h>        // C library. Needed for conversion function
#include "uart.h"          // Peter Fleury's UART library
#include <myMatrix.h>      // library for matrix scanning
#include "gpio.h"           // GPIO library for AVR-GCC
#include <string.h>

/* Function definitions ----------------------------------------------*/
// returns 1 if odd (1,3.) and 0 if even(0,2...)
uint16_t counter = 0;
uint16_t help_counter = 950;
uint16_t steps = 0;
uint16_t freq = 61;
uint16_t value = 0;
uint16_t help_value = 255;
uint16_t status = 1;
uint8_t wave = 0;
uint16_t frequency = 100;
const char *waveform = "sine";
uint8_t digits[4] = {0, 0, 0, 0};
const char *my_exponent = "Hz";
// bool status = 0;      //status on/off
uint8_t shiftChar[8] = { //adress 0
    0b11011,
    0b10001,
    0b00000,
    0b11011,
    0b11011,
    0b11011,
    0b11011,
    0b11011};

static const uint16_t lookup_sine[950] = {
128,128,129,130,131,132,133,133,134,135,136,137,138,138,139,140,141,142,143,143,
144,145,146,147,148,148,149,150,151,152,153,153,154,155,156,157,158,158,159,160,
161,162,162,163,164,165,166,166,167,168,169,170,170,171,172,173,174,174,175,176,
177,178,178,179,180,181,181,182,183,184,184,185,186,187,187,188,189,190,190,191,
192,193,193,194,195,195,196,197,198,198,199,200,200,201,202,202,203,204,204,205,
206,206,207,208,208,209,210,210,211,212,212,213,214,214,215,215,216,217,217,218,
218,219,220,220,221,221,222,222,223,224,224,225,225,226,226,227,227,228,228,229,
229,230,230,231,231,232,232,233,233,234,234,235,235,236,236,236,237,237,238,238,
239,239,239,240,240,241,241,241,242,242,243,243,243,244,244,244,245,245,245,246,
246,246,247,247,247,247,248,248,248,248,249,249,249,250,250,250,250,250,251,251,
251,251,252,252,252,252,252,252,253,253,253,253,253,253,253,254,254,254,254,254,
254,254,254,254,254,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,254,254,254,254,254,254,254,254,254,
254,253,253,253,253,253,253,253,252,252,252,252,252,252,251,251,251,251,250,250,
250,250,250,249,249,249,248,248,248,248,247,247,247,247,246,246,246,245,245,245,
244,244,244,243,243,243,242,242,241,241,241,240,240,239,239,239,238,238,237,237,
236,236,236,235,235,234,234,233,233,232,232,231,231,230,230,229,229,228,228,227,
227,226,226,225,225,224,224,223,222,222,221,221,220,220,219,218,218,217,217,216,
215,215,214,214,213,212,212,211,210,210,209,208,208,207,206,206,205,204,204,203,
202,202,201,200,200,199,198,198,197,196,195,195,194,193,193,192,191,190,190,189,
188,187,187,186,185,184,184,183,182,181,181,180,179,178,178,177,176,175,174,174,
173,172,171,170,170,169,168,167,166,166,165,164,163,162,162,161,160,159,158,158,
157,156,155,154,153,153,152,151,150,149,148,148,147,146,145,144,143,143,142,141,
140,139,138,138,137,136,135,134,133,133,132,131,130,129,128,128,127,126,125,124,
123,122,122,121,120,119,118,117,117,116,115,114,113,112,112,111,110,109,108,107,
107,106,105,104,103,102,102,101,100,99,98,97,97,96,95,94,93,93,92,91,
90,89,89,88,87,86,85,85,84,83,82,81,81,80,79,78,77,77,76,75,
74,74,73,72,71,71,70,69,68,68,67,66,65,65,64,63,62,62,61,60,
60,59,58,57,57,56,55,55,54,53,53,52,51,51,50,49,49,48,47,47,
46,45,45,44,43,43,42,41,41,40,40,39,38,38,37,37,36,35,35,34,
34,33,33,32,31,31,30,30,29,29,28,28,27,27,26,26,25,25,24,24,
23,23,22,22,21,21,20,20,19,19,19,18,18,17,17,16,16,16,15,15,
14,14,14,13,13,12,12,12,11,11,11,10,10,10,9,9,9,8,8,8,
8,7,7,7,7,6,6,6,5,5,5,5,5,4,4,4,4,3,3,3,
3,3,3,2,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,2,2,2,2,
2,2,2,3,3,3,3,3,3,4,4,4,4,5,5,5,5,5,6,6,
6,7,7,7,7,8,8,8,8,9,9,9,10,10,10,11,11,11,12,12,
12,13,13,14,14,14,15,15,16,16,16,17,17,18,18,19,19,19,20,20,
21,21,22,22,23,23,24,24,25,25,26,26,27,27,28,28,29,29,30,30,
31,31,32,33,33,34,34,35,35,36,37,37,38,38,39,40,40,41,41,42,
43,43,44,45,45,46,47,47,48,49,49,50,51,51,52,53,53,54,55,55,
56,57,57,58,59,60,60,61,62,62,63,64,65,65,66,67,68,68,69,70,
71,71,72,73,74,74,75,76,77,77,78,79,80,81,81,82,83,84,85,85,
86,87,88,89,89,90,91,92,93,93,94,95,96,97,97,98,99,100,101,102,
102,103,104,105,106,107,107,108,109,110,111,112,112,113,114,115,116,117,117,118,
119,120,121,122,122,123,124,125,126,127,128,};

int main(void)
{
  // Define pins as output
  GPIO_config_output(&DDRB, DAC_0);
  GPIO_config_output(&DDRB, DAC_1);
  GPIO_config_output(&DDRB, DAC_2);
  GPIO_config_output(&DDRB, DAC_3);
  GPIO_config_output(&DDRB, DAC_4);
  GPIO_config_output(&DDRB, DAC_5);
  GPIO_config_output(&DDRB, DAC_6);
  GPIO_config_output(&DDRB, DAC_7);


  // init matrix
  initMatrix();
  // Initialize LCD display
  lcd_init(LCD_DISP_ON);

  lcd_gotoxy(0, 0);
  lcd_puts("               ");
  lcd_gotoxy(0, 1);
  lcd_puts("               ");

  lcd_gotoxy(1, 0);
  lcd_puts("wave:");
  lcd_gotoxy(8, 0);
  lcd_puts("sine");
  lcd_gotoxy(13, 0);
  lcd_puts("off");

  lcd_gotoxy(1, 1);
  lcd_puts("Freq:");
  lcd_gotoxy(8, 1);
  lcd_puts("100 Hz");

  //custom character
  lcd_command(1 << LCD_CGRAM);
  for (uint8_t i = 0; i < 8; i++)
  {
    // Store all new chars to memory line by line
    lcd_data(shiftChar[i]);
  }
  // Set DDRAM address
  lcd_command(1 << LCD_DDRAM);

  // Initialize UART to asynchronous, 8N1, 9600
  uart_init(UART_BAUD_SELECT(9600, F_CPU));


  // set timer for generator
  TIM0_overflow_16us();
  TIM0_overflow_interrupt_enable();

  // set timer for scanning
  TIM1_overflow_33ms();
  TIM1_overflow_interrupt_enable();

  // Enables interrupts by setting the global interrupt mask
  sei();

  // Infinite loop
  while (1)
  {
  }

  // Will never reach this
  return 0;
}

// timer 1 is used to handle user interface (input matrix, display and UART)
ISR(TIMER1_OVF_vect)
{
  static uint8_t pos;     // position of pressed button
  static uint8_t pre_pos; // previous position
  static uint8_t shift = 0;
  static const char *button_name = ""; // button Name
  static uint8_t input_counter = 0;
  // char StringToUpadate[2] = ""; //string to update UART
  static uint8_t im_in_freq = 0;

  pos = scanMatrix();                       // get position in form of int ex:23 row 2, column 3
  button_name = posToConstChar(pos, shift); //grt name of button in form of const char* from position


  if ((pos == 0) || (pos == pre_pos))
  { 
  } // do nothing
  else
  {
    // if change is detected:
    if (pos == 14)
    { //if shift was pressed
      shift = !shift;
      if (shift == 1)
      {
        lcd_gotoxy(0, 1);
        lcd_putc(0);
      }
      if (shift == 0)
      {
        lcd_gotoxy(0, 1);
        lcd_puts(" ");
      }
    }
    else if ((pos != 14) && (shift == 1)) // switch off  shift after shift + button combination
    {
      shift = 0;
      lcd_gotoxy(0, 1);
      lcd_puts(" ");
    }

    if (strcmp(button_name,"freq")==0) // freq is pressed
    {
      im_in_freq = 1; // variable to detect, that frequency isn't already selected
    }
    else if(strcmp(button_name,"onoff")==0)
    {
      status = !status;
      if (status == 1)
      {
        lcd_gotoxy(13, 0);
        lcd_puts("   ");
        lcd_gotoxy(13, 0);
        lcd_puts("on");
      }
      if (status == 0)
      {
        lcd_gotoxy(13, 0);
        lcd_puts("off");
      }
      
    }

    else if(strcmp(button_name,"sine")==0)
    {
      waveform = button_name;
      wave = 1;
        lcd_gotoxy(8, 0);
        lcd_puts(waveform);
    }

        else if(strcmp(button_name,"ramp")==0)
    {
      waveform = button_name;
      wave = 2;
        lcd_gotoxy(8, 0);
        lcd_puts(waveform);
    }
    

    if (im_in_freq == 1)
    {
      if (input_counter == 0) // wait for input ("freq" on display)
      {
        lcd_gotoxy(8, 1);
        lcd_puts("        ");
        lcd_gotoxy(8, 1);
        lcd_puts(button_name);
        //_delay_us(2);
        input_counter++;
      }
      else if (input_counter == 1) // first digit (MSB)
      {
        lcd_gotoxy(8, 1);
        lcd_puts("        ");
        lcd_gotoxy(8, 1);
        lcd_puts(button_name);
        digits[0] = atoi(button_name);
        input_counter++;
      }

      else if (input_counter < 5) // 2nd - 4th(LSB) digit of frequency
      {
        lcd_gotoxy(7 + input_counter, 1);
        lcd_puts(button_name);
        digits[input_counter - 1] = atoi(button_name);
        input_counter++;
      }

      else if (input_counter == 5) // wait for enter frequency with exponent (Hz, kHz, MHz)
      {
        switch (pos)
        {
        case 12:
          my_exponent = " Hz";
          frequency = 1000 * digits[0] + 100 * digits[1] + 10 * digits[2] + digits[3];
          lcd_gotoxy(7 + input_counter, 1);
          lcd_puts(" Hz");
          uart_puts("frequency: ");
          input_counter = 0;
          im_in_freq = 0;
          break;

        case 22:
          my_exponent = "kHz";
          frequency = 1000 * digits[0] + 100 * digits[1] + 10 * digits[2] + digits[3];
          lcd_gotoxy(7 + input_counter, 1);
          lcd_puts(" kHz");
          input_counter = 0;
          im_in_freq = 0;
          break;

        case 32:
          my_exponent = "MHz";
          frequency = 1000 * digits[0] + 100 * digits[1] + 10 * digits[2] + digits[3];
          lcd_gotoxy(7 + input_counter, 1);
          lcd_puts(" MHz");
          input_counter = 0;
          im_in_freq = 0;
          break;

        default: // do nothing untill Hz, kHz or MHz is pressed
          break;
        } //switch
      }   //else if
    }     //if
    else  //send position of matrix input, frequency and exponent to UART
    {
    } //else
  }
  pre_pos = pos; // to avoid multiple press
} //ISR

ISR(TIMER0_OVF_vect)
{
  // calculate increment's steps for counter to reach chosen frequency
  steps = freq/66; 
  counter = counter + steps;
  help_counter = counter - steps;

  if(status==1)
  {
    if(wave==1)
    {
      value = lookup_sine[counter];
      if(counter<=950)
      {
        PORTB = (value & 0b11111111);
      }
      else if(counter > 950)
      {
        counter = 0;
      }
    }

    if(wave==2)
  {
      if(counter<=475)
      {
        PORTB = 0b11111111;
      }
      else if(counter > 475 && counter < 950) 
      {
        PORTB = 0b00000000; 
      }
      else if (counter >= 950)
      {
        counter = 0;
      }            
  }

    if(wave==3)
    {
      if(counter<=950)
      {
        value = counter/4;
        if(value<256)
        {
            PORTB = (value & 0b11111111);
        }
        else    
        {
            value=0;                
        }
      }
    }
    else if (counter > 950)
    {
      counter = 0;
    }

    if(wave==4)
    {
      if(counter<950)
      {
        value = counter/4;
        help_value = help_counter/4;
        if(value<128)
        {
          PORTB = (2*value & 0b11111111);
        }
        else if(value>=128)
        {
          PORTB = (2*help_value & 0b11111111);                
        }
        else if(value>=255)
        {
          value=0;
          help_value=255;   
        }
      }
      else if (counter >= 950)
      {
        help_counter = 950;
        counter = 0;
      }
    }
}
}
