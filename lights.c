#include <xc.h>
#include "lights.h"

/************************************
 * Initialise pins to drive the LED array and street lights
************************************/
void Lights_init(void)
{
    //set initial output LAT values
    LED1 = 0;
    LED2 = 0;
    LED3 = 0;
    LED4 = 0;
    LED5 = 0;
    LIGHTS = 0;
	
    //set up TRIS registers for pin outputs
    TRISGbits.TRISG0 = 0;
    TRISGbits.TRISG1 = 0;
    TRISAbits.TRISA2 = 0;
    TRISFbits.TRISF6 = 0;
    TRISAbits.TRISA4 = 0;
    TRISHbits.TRISH3 = 0;
}

/************************************
 * Displays a number on the LED array in binary
************************************/
void LEDarray_disp_bin(char number)
{
    //turn on/off the pins connected to the LED array
    if (number & 0b00001) {LED1 = 1;} else {LED1 = 0;}
    if (number & 0b00010) {LED2 = 1;} else {LED2 = 0;}
    if (number & 0b00100) {LED3 = 1;} else {LED3 = 0;}
    if (number & 0b01000) {LED4 = 1;} else {LED4 = 0;}
    if (number & 0b10000) {LED5 = 1;} else {LED5 = 0;}
}