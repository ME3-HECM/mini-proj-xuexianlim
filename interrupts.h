#ifndef _interrupts_H
#define _interrupts_H

#include <xc.h>

#define _XTAL_FREQ 64000000

#define LIGHTS LATHbits.LATH3 //LED on RH3 simulates street lights

//function prototypes
void Interrupts_init(void);
void __interrupt() ISR();

#endif
