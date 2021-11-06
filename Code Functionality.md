# Code Functionality

## Link to Video Demonstration
<https://imperiallondon-my.sharepoint.com/:v:/g/personal/xxl19_ic_ac_uk/EUemO3x4a7lNjZPtA1X1iSMBc3mWB1ju6D33rXhNxYGfLA>

## Introduction
The user initialises the device by inputting values for a suite of variables, namely the current date (*year*, *month*, *day*) and time (*hour*, *minute*), the day of the week (*dayofweek*), whether the UK is currently in DST (*DST*) and the dawn and dusk times (*dawnhour*, *dawnminute*, *duskhour*, *duskminute*). After doing so, the device should run fully automatically with no further need for maintenance.

## Requirement 1
The device monitors light level with the LDR and turns on an LED in low light conditions (i.e. night-time) and off in bright conditions (i.e. daytime).

Relevant functions:
```
    void DAC_init(void);
    void Comp1_init(void);
    void Interrupts_init(void);
    void __interrupt() ISR();
```
The output of an LDR and a DAC are fed to Comparator 1. Interrupts are triggered for both a rising and falling edge. Whenever an interrupt occurs, after the interrupt flag is checked, *CM1CON0bits.OUT* is checked to differentiate between a rising or falling edge and therefore, determine whether to switch on or turn off the lights.

## Requirement 2
The device displays the current hour of day on the LED array in binary.

Relevant functions:
```
    void Timekeeper(unsigned int *pyear, char *pmonth, char *pday, int *phour, int *pminute, char *pdayofweek, char *pDST);
    char LeapYear(unsigned int year);
    void LEDarray_disp_bin(int number);
    void Lights_init(void);
    void Timer0_init(void);
    void __interrupt() ISR();
```
Timer0 is used to keep track of time. It overflows every minute. The calculations are shown below:

    Prescaler = 1:16384
    Time interval = 4 \* 16384/ (64 \* 10^6) = 1.024 ms
    60 s / 1.024 ms = 58593.75 ≈ 58594
    2^16 - 1 - 58594 = 6941 or 0b0001101100011101
	
While the clock is technically slow by 256 μs every minute, it was deemed to be a negligible error given that the clock will be synchronised to the sun daily. Every time Timer0 overflows, a flag bit *minutehand* is set. When *minutehand* is checked and confirmed to be 1 in the *main* function, the *Timekeeper* function runs. This function increments the *minute* and implements any cascade changes to the *hour*, *day*, *month* and *year*, thereby acting as a calendar and clock. The *LeapYear* function also passes information to the *Timekeeper* function on whether a year is a leap year.

The hour of the day is then displayed via *LEDarray_disp_bin*.

## Requirement 3
The device turns the light off between approx. 1 am and 5 am.

Relevant functions:
```
    void SmallHours(int hour, int minute, int dawnhour);
```
Between 1 am and 5 am, *LATHbits.LATH3* is set to 0, turning off the lights. At 5 am, the state of *CM1CON0bits.OUT* is checked to determine whether it is bright or dark and therefore, whether to switch on the lights or leave it switched off.

## Requirement 4
The device adjusts for daylight savings time.

Relevant functions:
```
    void Timekeeper(unsigned int *pyear, char *pmonth, char *pday, int *phour, int *pminute, char *pdayofweek, char *pDST);
```
The last Sunday of March or October can only fall on the 25th to the 31st. Therefore, during these days, *dayofweek* is checked and if it is a Sunday, the clock is adjusted accordingly. *DST* is also set to 1 or 0 and crucially, prevents the clock from getting stuck in a loop between 1 am and 2 am on the last Sunday in October as the clock goes back 1 hour at 2 am.

## Requirement 5
The device maintains synchronicity with the sun indefinitely.

Relevant functions:
```
    void SunPleaseFixTheDamnClock(int *pdawnhour, int *pdawnminute, int *pduskhour, int *pduskminute, int *phour, int *pminute, char DST);
    void UpdateDawnDusk(int *pdawnhour, int *pdawnminute, int *pduskhour, int *pduskminute, int *phour, int *pminute, char DST);
    int TimeDiff(int hour1, int minute1, int hour2, int minute2);
    void TimeAvg(int hour1, int minute1, int hour2, int minute2, int *pavghour, int *pavgminute);
    void __interrupt() ISR();
```
Every time an interrupt occurs due to Comparator 1, it means that the sun has either risen or set so the flag bit *dawndusk* is set to 1 for dawn and 2 for dusk. *dawnhour*, *dawnminute*, *duskhour* and *duskminute* are then updated accordingly via *UpdateDawnDusk*. Note that these times are always stored in the device in GMT, regardless of whether the UK is in DST or not. Additionally, when the user initialises the device, they should technically also input the dawn and dusk times in GMT only. Nevertheless, even if they input these times in BST or if they input inaccurate dawn and dusk times, the device will update the dawn and dusk times by the next day.

From the point-of-view of the sun, solar noon always occurs at 12 noon in GMT, midway between dusk and dawn. The sun does not care whether we have fiddled with our clocks for DST. Therefore, it makes sense for the device to store the dawn and dusk times in GMT. If the midpoint between the recorded dawn and dusk times as calculated by *TimeAvg* is earlier than 12 noon, the clock is slow and vice versa. The relevant adjustments can then be made using the time difference calculated by *TimeDiff*. A half-an-hour tolerance is provided for some variation in dawn and dusk times due to say, cloud cover.

There is a rare but possible situation where the clock remains permanently inaccurate. This occurs when the clock is somehow 12 hours out from the actual time due to say, incorrect initialisation or a solar eclipse. *TimeAvg* will still calculate the midpoint of dusk and dawn to be 12 noon even if dusk and dawn are flipped and therefore the clock never gets adjusted. To preclude this, this condition is checked in *SunPleaseFixTheDamnClock* and if confirmed, the clock will be shifted by 12 hours.

## Requirement 6
The device should be fully automatic (requires zero maintenance after installation).

The amalgamation of the above functions enables the device to be fully automatic after initialisation. It keeps track of the date and time, caters for leap years, and synchronises itself to the sun.

## Bug fixes
This section details the bugs found during testing and the fixes:

- On initialisation, *duskhour* would immediately be modified to the initialisation hour due to the comparator voltage momentarily exceeding the DAC voltage on power up, triggering the *UpdateDawnDusk* function.

>> A small delay of 10 μs was incorporated into *Comp1_init* to cater for the voltage settling time. Following that, the *duskdawn* flag bit was also set to 0.