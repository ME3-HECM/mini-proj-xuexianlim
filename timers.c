#include <xc.h>
#include "timers.h"
#include "lights.h"

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
}

/************************************
 * Calendar and clock to keep track of date and time
************************************/
void Timekeeper(unsigned int *pyear, char *pmonth, char *pday, int *phour, int *pminute, char *pdayofweek, char *pDST) {
    //format the date and time
    ++*pminute; //after Timer0 interrupt is triggered due to an overflow after 1 min, increment the minutes
    
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
        *pDST = 1; //now in DST
    }
    
    if (*pmonth == 10 && *pday >=25 && *phour == 2 && *pdayofweek == 7 && *pDST) {
        --*phour; //clocks go back 1 hour at 2 am on last Sunday in Oct
        *pDST = 0; //no longer in DST
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

/************************************
 * Calibrates the time based on the calculated solar noon time
************************************/
void SunPleaseFixTheDamnClock(int *pdawnhour, int *pdawnminute, int *pduskhour, int *pduskminute, int *phour, int *pminute, char DST)
{
    int avghour; //after removing the effect of DST, this midpoint time between dawn and dusk should theoretically always be 12 noon 
    int avgminute; //any deviation will therefore give info about whether the clock is fast or slow and by how much
    
    TimeAvg(*pdawnhour, *pdawnminute, *pduskhour, *pduskminute, &avghour, &avgminute); //calculate solar noon time based on the dusk and dawn times
    int timedifference = TimeDiff(avghour, avgminute, 12, 0); //difference between theoretical and actual solar noon time
            
    if (timedifference > 30 || timedifference < -30) { //±30-min tolerance before the time gets synced to the sun
        *pminute = timedifference % 60; //adjust the minutes
        *pdawnminute = *pdawnminute + timedifference % 60;
        *pduskminute = *pduskminute + timedifference % 60;
        
        if (*pminute < 0) {--*phour; *pminute = *pminute + 60;} //ensure minutes make sense (values from 0-59)
        
        if (*pdawnminute >= 60) {++*pdawnhour; *pdawnminute = *pdawnminute - 60;}
        else if (*pdawnminute < 0) {--*pdawnhour; *pdawnminute = *pdawnminute + 60;}
        
        if (*pduskminute >= 60) {++*pduskhour; *pduskminute = *pduskminute - 60;}
        else if (*pduskminute < 0) {--*pduskhour; *pduskminute = *pduskminute + 60;}
        
        *phour = 12 + timedifference / 60; //adjust the hour
        *pdawnhour = *pdawnhour + timedifference / 60;
        *pduskhour = *pduskhour + timedifference / 60;
    }
}

/************************************
 * Gives the time difference in minutes between 2 times
************************************/
int TimeDiff(int hour1, int minute1, int hour2, int minute2) //returns positive values when time2 > time1 and vice versa
{
    if (hour2 > hour1) {
        while (hour2 > hour1) { //convert hours to minutes
            --hour2;
            minute2 = minute2 + 60;
        }
        
    } else if (hour1 > hour2) {
        while (hour1 > hour2) {
            --hour1;
            minute1 = minute1 + 60;
        }    
    }
    return minute2 - minute1; //difference in minutes
}

/************************************
 * Gives the midpoint of 2 times
************************************/
void TimeAvg(int hour1, int minute1, int hour2, int minute2, int *pavghour, int *pavgminute)
{
    int HalfTimeDiff = TimeDiff(hour1, minute1, hour2, minute2) / 2; //find half of the time interval between the 2 times
    
    *pavghour = hour1 + HalfTimeDiff / 60; //add/subtract half of the time interval to the earlier/later time
    *pavgminute = minute1 + HalfTimeDiff % 60;
    
    if (*pavgminute >= 60) {++*pavghour; *pavgminute = *pavgminute - 60;} //ensure minutes make sense (values from 0-59)
    else if (*pavgminute < 0) {--*pavghour; *pavgminute = *pavgminute + 60;}
}

/************************************
 * Updates the dawn and dusk times
 * These times are always displayed in GMT regardless of whether the UK is currently in DST
************************************/
void UpdateDawnDusk(int *pdawnhour, int *pdawnminute, int *pduskhour, int *pduskminute, int *phour, int *pminute, char DST)
{
    if (*phour < 12 && !LIGHTS) { //dawn is before noon and lights should be off
        if (DST) {
            *pdawnhour = *phour - 1; //remove effect of DST
            *pdawnminute = *pminute;
        } else {
            *pdawnhour = *phour;
            *pdawnminute = *pminute;
        }
    } else if (LIGHTS) { //dusk is after noon and lights should be on
        if (DST) {
            *pduskhour = *phour - 1; //remove effect of DST
            *pduskminute = *pminute;
        } else {
            *pduskhour = *phour;
            *pduskminute = *pminute;
        }
    }
}