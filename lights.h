#ifndef _lights_H
#define _lights_H

#include <xc.h>

#define _XTAL_FREQ 64000000

#define LED1 LATGbits.LATG0 //define pins used for LED array
#define LED2 LATGbits.LATG1
#define LED3 LATAbits.LATA2
#define LED4 LATFbits.LATF6
#define LED5 LATAbits.LATA4
#define LIGHTS LATHbits.LATH3 //LED on RH3 simulates street lights

//function prototypes
void Lights_init(void);
void LEDarray_disp_bin(char number);

#endif
