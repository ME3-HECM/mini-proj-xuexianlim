//CONFIG1L
#pragma config FEXTOSC = HS //external Oscillator mode Selection bits (HS (crystal oscillator) above 8 MHz; PFM set to high power)
#pragma config RSTOSC = EXTOSC_4PLL //power-up default value for COSC bits (EXTOSC with 4x PLL, with EXTOSC operating per FEXTOSC bits)

//CONFIG3L
#pragma config WDTE = OFF //WDT operating mode (WDT enabled regardless of sleep)

#include <xc.h>
#include "lights.h"
#include "timers.h"
#include "interrupts.h"
#include "comparator.h"

#define _XTAL_FREQ 64000000

void main(void) {
    //input date and time for initialisation
    unsigned int year = 2021;
    char month = 11;
    char day = 1;
    char hour = 4; //24-hour time format
    char minute = 59;
    
	//initialisation functions to set up the hardware modules
    Lights_init();
    Timer0_init();
    Comp1_init();
    Interrupts_init();
    
    while (1) {
        Timekeeper(&year, &month, &day, &hour, &minute);
        LEDarray_disp_bin(hour);
        SmallHours(hour, minute);
    }
}