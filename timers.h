#ifndef _timers_H
#define _timers_H

#include <xc.h>

#define _XTAL_FREQ 64000000

//function prototypes
void Timer0_init(void);
void Timekeeper(unsigned int *pyear, char *pmonth, char *pday, int *phour, int *pminute, char *pdayofweek, char *pDST);
char LeapYear(unsigned int year);
void SunPleaseFixTheDamnClock(int *pdawnhour, int *pdawnminute, int *pduskhour, int *pduskminute, int *phour, int *pminute, char DST);
int TimeDiff(int hour1, int minute1, int hour2, int minute2);
void TimeAvg(int hour1, int minute1, int hour2, int minute2, int *pavghour, int *pavgminute);
void UpdateDawnDusk(int *pdawnhour, int *pdawnminute, int *pduskhour, int *pduskminute, int *phour, int *pminute, char DST);

#endif