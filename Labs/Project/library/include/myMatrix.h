#ifndef myMatrix_H
#define myMatrix_H

#include <inttypes.h> //include int data types like uint8_t
#include <avr/pgmspace.h>

// connections
#define MATRIX_PORT       PORTC
#define MATRIX_MODE_PORT  DDRC
#define MATRIX_READ_PORT  PINC
#define Col_1             PC0
#define Col_2             PC1
#define Col_3             PC2
#define Col_4             PC3
#define row_1             PC4
#define row_2             PC5
#define row_3             PC6

//functions
void initMatrix(); // set pins as inputs with pullups and etc.
uint8_t scanMatrix(); //returns number like 12 --> row 1, column 2
const char* posToConstChar(uint8_t pos, bool shift); // returns button name from position
const char* shiftFun(uint8_t number, bool shift); // to handle shift inputs
void callbacksToButtons(const char* button_name); //this function is called inside shiftFun()

#endif
