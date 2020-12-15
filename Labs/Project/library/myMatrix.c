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


uint8_t columns[4] = {Col_1,Col_2,Col_3,Col_4};
uint8_t rows[3] = {row_3,row_2,row_1};
const char* buttons_look_up[13]= {"1","2","3","4","5","6","7","8","9","shift","0","onoff"};
const char* shift_buttons_look_up[13]= {"freq","sine","ramp","Hz","kHz","MHz","sqre","tria","9","shift","0","onoff"};


void initMatrix(){
    // set collumn pins as inputs with internal pullups
    for (size_t i = 0; i < 4; i++)
    {
     GPIO_config_input_pullup(&MATRIX_MODE_PORT, columns[i]); 
    }
    

    // set rows as outputs HIGH
    for (size_t i = 0; i < 3; i++)
    {
       GPIO_config_output(&MATRIX_MODE_PORT, rows[i]);
       GPIO_write_high(&MATRIX_PORT, rows[i]);
    }    
}


uint8_t scanMatrix(){ //returns int like 14 --> row 1, column 4
   static uint8_t pos_row;
   static uint8_t pos_column;
   static uint8_t pos = 0;  //row x column 
   static uint8_t n =0; //variable to check if nothing was pressed
    for (size_t i = 0; i < 3; i++)
    {   
        //set row to LOW and scan through collumns
        GPIO_toggle(&MATRIX_PORT, rows[i]); // set row pin LOW
        for (size_t k = 0; k < 4; k++) // scan through all columns
        { //_delay_us(5); // needed for Simul IDE crap
            if (GPIO_read(&PINC, columns[k]) == 0)
            {
                pos_row = i+1;
                pos_column = k+1;
                pos = 10*pos_row+pos_column;
                
            }//if  
            else
            {
                n++;
            }
                 
        }//for
    GPIO_toggle(&MATRIX_PORT, rows[i]);  // set row pin HIGH
    }//for 
    if (n == 12){ // nothing was pressed
        pos = 0;
    }
    
    n = 0;
    return pos;  
     
}//void


const char* posToConstChar(uint8_t pos, bool shift){
    const char* button_name;
    switch (pos)
    {
    case 11:
    button_name = shiftFun(0,shift); //1 or freq
        break;
    case 21:
        button_name = shiftFun(1,shift); //2 or sine
        break;
    case 31:
        button_name = shiftFun(2,shift);//3 or ramp
        break; 
    case 12:
        button_name = shiftFun(3,shift);// 4 or Hz 
        break; 
    case 22:
        button_name = shiftFun(4,shift);// 5 or kHz
        break; 
    case 32:
        button_name = shiftFun(5,shift);// 6 or MHz
        break;         
    case 13:
        button_name = shiftFun(6,shift);// 7 or sqr
        break; 
    case 23:
        button_name = shiftFun(7,shift);
        break; 
    case 33:
        button_name = shiftFun(8,shift);
        break; 
    case 14:
        button_name = shiftFun(9,shift);//shift
        break;
    case 24:
        button_name = shiftFun(10,shift);//0
        break; 
    case 34:
        button_name = shiftFun(11,shift);//enter/decimal
        break;  
    case 0:
        button_name = shiftFun(12,shift);
        break;         

    default:
        button_name = "Init";
        break;
    }
    return button_name;
}

// this function returns correct name according to shift state
const char* shiftFun(uint8_t number, bool shift){
    const char* button_name;
    if (shift == 0)
    {
        button_name = buttons_look_up[number];
    }
    else
    {
        button_name = shift_buttons_look_up[number];
        if (strcmp(button_name , "shift")!=0)
        {
            shift = 0;
        }
        
    }   
    return button_name;
}
