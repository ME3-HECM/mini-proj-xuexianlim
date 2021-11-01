#include <xc.h>
#include "interrupts.h"

/************************************
 * Set up interrupts
************************************/
void Interrupts_init(void)
{
	//turn on global interrupts, peripheral interrupts and the interrupt sources
    PIE2bits.C1IE = 1; //enable interrupts from Comparator 1
    INTCONbits.PEIE = 1; //peripheral interrupts
    INTCONbits.GIE = 1; //global interrupts
}

/************************************
 * Interrupt service routine
************************************/
void __interrupt() ISR()
{
    if (PIR2bits.C1IF == 1) { //Comparator 1
        if (CM1CON0bits.OUT) {LIGHTS = 1;} //rising edge (dark outside)
        else {LIGHTS = 0;} //falling edge (bright outside)
        PIR2bits.C1IF = 0;
    }
}

