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
    //user inputs date, time and DST status for initialisation
    unsigned int year = 2021;
    char month = 11;
    char day = 1;
    char hour = 20; //24-hour time format
    char minute = 00;
    char dayofweek = 1; //1 = Mon, 2 = Tue, 3 = Wed, 4 = Thu, 5 = Fri, 6 = Sat, 7 = Sun
    char DST = 0; //input whether the UK is currently in DST (1 = BST/DST, 0 = GMT)
    
	//initialisation functions to set up the hardware modules
    Lights_init();
    Timer0_init();
    Comp1_init();
    Interrupts_init();
    
    while (1) {
        Timekeeper(&year, &month, &day, &hour, &minute, &dayofweek, &DST);
        LEDarray_disp_bin(hour);
        SmallHours(hour, minute);
    }
}