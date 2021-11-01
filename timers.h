#ifndef _timers_H
#define _timers_H

#include <xc.h>

#define _XTAL_FREQ 64000000

#define MINUTEHAND LATAbits.LATA3 //pin A3 is used to extract the information that 1 min has passed from the Timer0 interrupt

//function prototypes
void Timer0_init(void);
void Timekeeper(unsigned int *pyear, char *pmonth, char *pday, char *phour, char *pminute, char *pdayofweek, char *DST);
char LeapYear(unsigned int year);

#endif