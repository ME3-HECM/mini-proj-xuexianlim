#include <xc.h>
#include "timers.h"

/************************************
 * Set up Timer0
************************************/
void Timer0_init(void)
{
    T0CON1bits.T0CS=0b010; //Fosc/4
    T0CON1bits.T0ASYNC=1; //input to Timer0 counter not synced to sys clocks (needed to ensure correct operation with Fosc/4 as clock source)
    T0CON1bits.T0CKPS=0b1110; //1:16384 prescaler (0b1110 - modify this to speed up time for testing)
    T0CON0bits.T016BIT=1; //16-bit mode
    //4*16384/(64*10^6) = 1.024*10^-3 s
    //60/(1.024*10^-3) = 58593
    //2^16-1-58593 = 6942 = 0b0001101100011110
    TMR0H=0b00011011; //initialise Timer0 with 6942 so its overflows after 1 min
    TMR0L=0b00011110;
    T0CON0bits.T0EN=1; //start Timer0
    MINUTEHAND = 0; //MINUTEHAND (pin A3) is used to extract the information that 1 min has passed from the Timer0 interrupt
    TRISAbits.TRISA3 = 0;
}

/************************************
 * Calendar and clock to keep track of date and time
************************************/
void Timekeeper(unsigned int *pyear, char *pmonth, char *pday, char *phour, char *pminute, char *pdayofweek, char *DST) {
    //format the date and time
    if (MINUTEHAND) {++*pminute; MINUTEHAND = 0;} //after Timer0 interrupt is triggered due to an overflow after 1 min, increment the minutes
    
    if (*pminute == 60) {*pminute = 0; ++*phour;} //reset the minutes and increment the hour when the minutes overflow
    
    if (*phour == 24) {*phour = 0; ++*pday; ++*pdayofweek;} //similar logic for all subsequent lines
    
    if (*pdayofweek == 8) {*pdayofweek = 1;}
    
    if (*pday == 29 && *pmonth == 2 && !LeapYear(*pyear)) { //Feb has either 28/29 days depending on whether it is a leap year
        *pday = 1;
        ++*pmonth;
    }
    
    if (*pday == 30 && *pmonth == 2 && LeapYear(*pyear)) {
        *pday = 1;
        ++*pmonth;
    }
    
    if (*pday == 31 && (*pmonth == 4 || *pmonth == 6 || *pmonth == 9 || *pmonth == 11)) { //Apr, Jun, Sep, Nov have 30 days
        *pday = 1;
        ++*pmonth;
    }
    
    if (*pday == 32 && (*pmonth == 1 || *pmonth == 3 || *pmonth == 5 || *pmonth == 7 || *pmonth == 8 || *pmonth == 10 || *pmonth == 12)) { //Jan, Mar, May, Jul, Aug, Oct, Dec have 31 days
        *pday = 1;
        ++*pmonth;
    }
    
    if (*pmonth == 13) {*pmonth = 1; ++*pyear;}
    
    //Daylight Savings Time
    if (*pmonth == 3 && *pday >= 25 && *phour == 1 && *pdayofweek == 7) { //the last Sunday of Mar/Oct can only range from the 25th to the 31st
        ++*phour; //clocks go forward 1 hour at 1 am on last Sunday in Mar
        *DST = 1; //now in DST
    }
    
    if (*pmonth == 10 && *pday >=25 && *phour == 2 && *pdayofweek == 7 && *DST) {
        --*phour; //clocks go back 1 hour at 2 am on last Sunday in Oct
        *DST = 0; //no longer in DST
    }
}

/************************************
 * Determines whether a year is a leap year
************************************/
char LeapYear(unsigned int year)
{
    char leapyear;
    if (year % 4 == 0 && year % 100 != 0) { //leap years are divisible by 4 except for those that are also divisible by 100
        leapyear = 1;
    } else if (year % 400 == 0) { //however, leap years divisible by 400 are also leap years even if they are divisible by 100
        leapyear = 1;
    } else {
        leapyear = 0;
    }
    return leapyear;
}