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

/* Includes ----------------------------------------------------------*/
#include <avr/io.h>        // AVR device-specific IO definitions
#include <avr/interrupt.h> // Interrupts standard C library for AVR-GCC
#include "timer.h"         // Timer library for AVR-GCC
#include "lcd.h"           // Peter Fleury's LCD library
#include <stdlib.h>        // C library. Needed for conversion function
#include "uart.h"          // Peter Fleury's UART library
#include <myMatrix.h>      // library for matrix scanning
#include "gpio.h"
#include <util/delay.h> 
#include <stdio.h>
#include <string.h>

#define DAC_0        PB0     // AVR pin where DAC is connected
#define DAC_1        PB1     // AVR pin where DAC is connected
#define DAC_2        PB2     // AVR pin where DAC is connected
#define DAC_3        PB3     // AVR pin where DAC is connected
#define DAC_4        PB4     // AVR pin where DAC is connected
#define DAC_5        PB5     // AVR pin where DAC is connected
#define DAC_6        PB6     // AVR pin where DAC is connected
#define DAC_7        PB7     // AVR pin where DAC is connected

// pre-calculated up table for sine source: https://daycounter.com/Calculators/Sine-Generator-Calculator.phtml
const uint16_t lookup_sine[512] = {
128,129,131,132,134,135,137,138,140,142,143,145,146,148,149,151,152,154,155,157,
159,160,162,163,165,166,168,169,171,172,173,175,176,178,179,181,182,184,185,186,
188,189,190,192,193,195,196,197,198,200,201,202,204,205,206,207,209,210,211,212,
213,214,216,217,218,219,220,221,222,223,224,225,226,227,228,229,230,231,232,233,
234,235,235,236,237,238,239,239,240,241,242,242,243,244,244,245,245,246,247,247,
248,248,249,249,250,250,250,251,251,252,252,252,253,253,253,253,254,254,254,254,
254,255,255,255,255,255,255,255,255,255,255,255,255,255,255,254,254,254,254,254,
254,253,253,253,252,252,252,251,251,251,250,250,249,249,248,248,247,247,246,246,
245,244,244,243,243,242,241,240,240,239,238,237,237,236,235,234,233,232,231,231,
230,229,228,227,226,225,224,223,222,221,219,218,217,216,215,214,213,212,210,209,
208,207,205,204,203,202,200,199,198,196,195,194,192,191,190,188,187,186,184,183,
181,180,179,177,176,174,173,171,170,168,167,165,164,162,161,159,158,156,155,153,
152,150,149,147,145,144,142,141,139,138,136,135,133,131,130,128,127,125,124,122,
120,119,117,116,114,113,111,110,108,106,105,103,102,100,99,97,96,94,93,91,
90,88,87,85,84,82,81,79,78,76,75,74,72,71,69,68,67,65,64,63,
61,60,59,57,56,55,53,52,51,50,48,47,46,45,43,42,41,40,39,38,
37,36,34,33,32,31,30,29,28,27,26,25,24,24,23,22,21,20,19,18,
18,17,16,15,15,14,13,12,12,11,11,10,9,9,8,8,7,7,6,6,
5,5,4,4,4,3,3,3,2,2,2,1,1,1,1,1,1,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,2,2,2,2,
3,3,3,4,4,5,5,5,6,6,7,7,8,8,9,10,10,11,11,12,
13,13,14,15,16,16,17,18,19,20,20,21,22,23,24,25,26,27,28,29,
30,31,32,33,34,35,36,37,38,39,41,42,43,44,45,46,48,49,50,51,
53,54,55,57,58,59,60,62,63,65,66,67,69,70,71,73,74,76,77,79,
80,82,83,84,86,87,89,90,92,93,95,96,98,100,101,103,104,106,107,109,
110,112,113,115,117,118,120,121,123,124,126,128};

uint16_t counter = 0;           // needed to adjust frequency of output signal
// uint16_t help_counter = 512; // not needed without triangle
uint16_t steps = 0;             // needed to adjust frequency of output signal
uint16_t value = 0;             // output value send to PORTB...the output pins for DAC
// uint16_t help_value = 255;   // not needed without triangle

uint16_t frequency = 100;
const char *waveform = "sine";
uint8_t digits[4] = {0, 0, 0, 0};
const char *my_exponent = "Hz";
bool status = 0;      //status on/off
uint8_t shiftChar[8] = { //adress 0
    0b11011,
    0b10001,
    0b00000,
    0b11011,
    0b11011,
    0b11011,
    0b11011,
    0b11011};

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
  _delay_ms(2);
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
  // set timer for scanning
  TIM1_overflow_33ms();
  TIM1_overflow_interrupt_enable();

    // set timer for generator
  TIM0_overflow_128us();
  TIM0_overflow_interrupt_enable();

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
  static bool shift = 0;
  static const char *button_name = ""; // button Name
  static uint8_t input_counter = 0;
  static bool im_in_freq = 0;

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

    if (strcmp(button_name , "freq")==0) // freq is pressed
    {
      im_in_freq = 1; // variable to detect, that frequency isn't already selected
    }
    else if(strcmp(button_name , "onoff")==0)
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

    else if((strcmp(button_name , "sine")==0) || (strcmp(button_name , "ramp")==0)|| (strcmp(button_name , "sqre")==0) || (strcmp(button_name , "tria")==0)){
      waveform = button_name;
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
        //_delay_us(2); needed for Simul IDE crap
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


// timer 0 is used as frequency counter for the signal generator

ISR(TIMER0_OVF_vect)
{
    // calculate steps for counter to reach chosen frequency
    // steps = frequency * 512 (size of look up table) / 7812 (1/128us - timer0)
    steps = frequency/15; 
    // Counts up with increrement of steps - this allows to vary the frequency of the signal
    counter = counter + steps;
    // help_counter = counter - steps;

    if(status==1) // the "on/off" button is pressed and the actual status is "on", the signal appears on the output
    {
        if((strcmp(waveform , "sine")==0) )         // if sine waveform was chosen; strcmp helps to compare string values
        {   
            value = lookup_sine[counter];           // use look up table with pre-calculated values for sine wave, the x-th value is chosen each time, given by the counter value
            if(counter<=512)                        // if the counter is smaller than 512 (look up table size), the DAC outputs are activated
            {                                       // to PORTB, where the output pins for DAC are connected receives the actual value to output at the moment 
                PORTB = (value & 0b11111111);       // this value is compared with 0b11111111 / e.g. if value = 4 in dec, then PORTB = 0b00000100 & 0b11111111 = 0b00000100   
            }                                       // and only the pin PB5 for DAC_5 is active
            else if(counter > 512)                  // if the counter value is bigger than 512, this part sets the counter to 0 and the whole process repeats
            {
                counter = 0;
            }
        }

        if((strcmp(waveform , "sqre")==0) )         // if square waveform was chosen; strcmp helps to compare string values
        {
            if(counter<=256)                        // if the counter value is smaller than 256 (half the size), the PORTB has all output pins active
            {
                PORTB = 0b11111111;                 
            }
            else if(counter > 256 && counter < 512) // for the counter value bigger than 256 but smaller than 512, the PORTB has all output pins set to 0
            {
                PORTB = 0b00000000; 
            }
            else if (counter >= 512)                // if the counter value is bigger than 512, this part sets the counter to 0 and the whole process repeats
            {
                counter = 0;
            }            
        }
    
        if((strcmp(waveform , "ramp")==0) )         // if ramp waveform was chosen; strcmp helps to compare string values
        {
            if(counter<512)                         // while the counter value is smaller than 512
            {
                value = counter/2;                  // the value passed to the output is calculated as half the value of the counter - 0 to 512/2 (256)
                if(value<256)                       // if value smaller than 256 - actually always true, the PORTB receives the current value and with the help 
                {                                   // of logical AND activates the right output pins
                    PORTB = (value & 0b11111111);
                }
                else    
                {
                    value=0;                        // resets the the value back to 0
                }
            }
            else if (counter >= 512)                 // if the counter value is bigger than 512, this part sets the counter to 0 and the whole process repeats
            {
                counter = 0;
            }
        }
        
//      This part was dedicated for triangle, however it did not work ideally... 
//      in ideal case, we would use another pre-calculated look up table for triangle and use same method as with sine
//      but the program would't let us compile and would not create the .HEX file, so this waveform was left out.
//      For the look up table version: (https://daycounter.com/Calculators/Triangle-Wave-Generator-Calculator.phtml to generate the look up table, which is not included)
//         if((strcmp(waveform , "tria")==0))
//         {
//             value = lookup_triangle[counter];       
//             if(counter<=512)                        
//             {                                       
//                 PORTB = (value & 0b11111111);       
//             }                                       
//             else if(counter > 512)                  
//             {
//                 counter = 0;
//             }
//         }
//      The rather not working version:
//         if((strcmp(waveform , "tria")==0) )       
//         {
//             if(counter<512)
//             {
//                 value = counter/2;
//                 help_value = help_counter/2;
//                 if(value<128)
//                 {
//                     PORTB = (2*value & 0b11111111);
//                 }
//                 else if(value>=128)
//                 {
//                     PORTB = (2*help_value & 0b11111111);                
//                 }
//                 else if(value>=255)
//                 {
//                     value=0;
//                     help_value=128;   
//                 }
//             }
//             else if (counter >= 512)
//             {
//                 help_counter = 512;
//                 counter = 0;
//             }
        }
    }
}
