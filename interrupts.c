#include <xc.h>
#include "interrupts.h"
#include "flags.h"

/************************************
 * Set up interrupts
************************************/
void Interrupts_init(void)
{
	//turn on global interrupts, peripheral interrupts and the interrupt sources
    PIE0bits.TMR0IE = 1; //enable interrupts from Timer0
    PIE2bits.C1IE = 1; //enable interrupts from Comparator 1
    INTCONbits.PEIE = 1; //peripheral interrupts
    INTCONbits.GIE = 1; //global interrupts
}

/************************************
 * Interrupt service routine
************************************/
void __interrupt() ISR()
{
    if (PIR0bits.TMR0IF == 1) { //Timer0
        minutehand = 1;
        TMR0H=0b00011011; //reset Timer0 to 6942
        TMR0L=0b00011110;
        PIR0bits.TMR0IF = 0;
    }

    if (PIR2bits.C1IF == 1) { //Comparator 1
        if (CM1CON0bits.OUT) {LIGHTS = 1; dawndusk = 2;} //rising edge (dusk)
        else {LIGHTS = 0; dawndusk = 1;} //falling edge (dawn)
        PIR2bits.C1IF = 0;
    }
}