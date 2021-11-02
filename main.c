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
#include "flags.h"

#define _XTAL_FREQ 64000000

void main(void) {
    //user inputs date, time and DST status for initialisation
    unsigned int year = 2021;
    char month = 11;
    char day = 2;
    int hour = 12; //24-hour time format
    int minute = 0;
    char dayofweek = 2; //1 = Mon, 2 = Tue, 3 = Wed, 4 = Thu, 5 = Fri, 6 = Sat, 7 = Sun
    char DST = 0; //input whether the UK is currently in DST (1 = BST/DST, 0 = GMT)
    int dawnhour = 7; //approximate dawn and dusk times 
    int dawnminute = 0; //not too important to get it exactly as it will be synced to the sun anyway
    int duskhour = 17;
    int duskminute = 0;
    
	//initialisation functions to set up the hardware modules
    Lights_init();
    Timer0_init();
    Comp1_init();
    Interrupts_init();
    
    while (1) {
        if (minutehand) {
            Timekeeper(&year, &month, &day, &hour, &minute, &dayofweek, &DST); //keep track of date, time and DST
            minutehand = 0;
        }
        
        LEDarray_disp_bin(hour); //display hour of the day on LED array in binary
        
        SmallHours(hour, minute); //turn the lights off between 1 am and 5 am
        
        if (dawndusk) {
            UpdateDawnDusk(&dawnhour, &dawnminute, &duskhour, &duskminute, &hour, &minute); //update the dawn and dusk times
            dawndusk = 0;
        }
        
        if (hour == 12 && minute == 0) { //sync the clock to the sun at noon
            SunPleaseFixTheDamnClock(&dawnhour, &dawnminute, &duskhour, &duskminute, &hour, &minute, DST);
        }      
    }
}