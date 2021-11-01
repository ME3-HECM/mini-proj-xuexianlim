#ifndef _interrupts_H
#define _interrupts_H

#include <xc.h>

#define _XTAL_FREQ 64000000

#define LIGHTS LATHbits.LATH3 //LED on RH3 simulates street lights
#define MINUTEHAND LATAbits.LATA3 //pin A3 is used to extract the information that 1 min has passed from the Timer0 interrupt

//function prototypes
void Interrupts_init(void);
void __interrupt() ISR();

#endif