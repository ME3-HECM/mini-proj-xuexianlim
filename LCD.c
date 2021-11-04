#include <xc.h>
#include <stdio.h>
#include "LCD.h"

/************************************
 * Function to toggle LCD enable bit on then off
 * when this function is called the LCD screen reads the data lines
************************************/
void LCD_E_TOG(void)
{
	LCD_E = 1;//turn the LCD enable bit on
	__delay_us(2); //wait a short delay
	LCD_E = 0;//turn the LCD enable bit off again
}

/************************************
 * Function to set the 4-bit data line levels for the LCD
************************************/
void LCD_sendnibble(unsigned char number)
{
	//set the data lines here (think back to LED array output)
    if (number & 0b0001) {LCD_DB4 = 1;} else {LCD_DB4 = 0;}
    if (number & 0b0010) {LCD_DB5 = 1;} else {LCD_DB5 = 0;}
    if (number & 0b0100) {LCD_DB6 = 1;} else {LCD_DB6 = 0;}
    if (number & 0b1000) {LCD_DB7 = 1;} else {LCD_DB7 = 0;}
    
    LCD_E_TOG();			//toggle the enable bit to instruct the LCD to read the data lines
    __delay_us(5);      //Delay 5uS
}


/************************************
 * Function to send full 8-bit commands/data over the 4-bit interface
 * high nibble (4 most significant bits) are sent first, then low nibble sent
************************************/
void LCD_sendbyte(unsigned char Byte, char type)
{
    if (type) {LCD_RS = 1;} else {LCD_RS = 0;} // set RS pin whether it is a Command (0) or Data/Char (1) using type argument
    LCD_sendnibble((Byte >> 4) & 0b1111); // send high bits of Byte using LCD_sendnibble function
    LCD_sendnibble(Byte & 0b1111); // send low bits of Byte using LCD_sendnibble function
	
    __delay_us(50);               //delay 50uS (minimum for command to execute)
}

/************************************
 * Function to initialise the LCD after power on
************************************/
void LCD_Init(void)
{
    //Define LCD Pins as Outputs and
    //set all pins low (might be random values on start up, fixes lots of issues)
    TRISCbits.TRISC6 = 0;
    TRISCbits.TRISC2 = 0;
    TRISBbits.TRISB3 = 0;
    TRISBbits.TRISB2 = 0;
    TRISEbits.TRISE3 = 0;
    TRISEbits.TRISE1 = 0;
    LCD_RS = 0;
    LCD_E = 0;
    LCD_DB4 = 0;
    LCD_DB5 = 0;
    LCD_DB6 = 0;
    LCD_DB7 = 0;

    //Initialisation sequence code
	// follow the sequence in the GitHub Readme picture for 4-bit interface.
	// first Function set should be sent with LCD_sendnibble (the LCD is in 8 bit mode at start up)
	// after this use LCD_sendbyte to operate in 4 bit mode
    LCD_sendnibble(0b0011); //0011
    __delay_us(50); //>39us
    LCD_sendbyte(0b00101000,0); //0010NF**
    __delay_us(50); //>39us
    LCD_sendbyte(0b00101000,0); //0010NF**
    __delay_us(50); //>37us
    LCD_sendbyte(0b00001100,0); //00001DCB
    __delay_us(50); //>37us
    LCD_sendbyte(0b00000001,0); //00000001
    __delay_us(1600); //>1.53ms
    LCD_sendbyte(0b00000110,0); //000001(I/D)(SH)
	//remember to turn the LCD display back on at the end of the initialisation (not in the data sheet)
}

/************************************
 * Function to set the cursor to beginning of line 1 or 2
************************************/
void LCD_setline (char line)
{
    if (line == 1) {LCD_sendbyte(0x80,0);} //Send 0x80 to set line to 1 (0x00 ddram address)
    if (line == 2) {LCD_sendbyte(0xC0,0);} //Send 0xC0 to set line to 2 (0x40 ddram address)
}

/************************************
 * Function to send string to LCD screen
************************************/
void LCD_sendstring(char *string)
{
	//code here to send a string to LCD using pointers and LCD_sendbyte function
    while(*string != 0){  // While the data pointed to isn?t a 0x00 do below (strings in C must end with a NULL byte) 
		LCD_sendbyte(*string++,1); 	//Send out the current byte pointed to and increment the pointer
	}
}

/************************************
 * Display date and time
************************************/
void Display(unsigned int year, char month, char day, int hour, int minute, char dayofweek, char DST, int dawnhour, int dawnminute, int duskhour, int duskminute)
{
    char string[16];
    char string2[15];
    int iyear = year;
    sprintf(string,"%02d/%02d/%d %02d:%02d",day,month,iyear,hour,minute);
    LCD_setline(1);
    LCD_sendstring(string);
    sprintf(string2,"%02d:%02d %02d:%02d %d %d",dawnhour,dawnminute,duskhour,duskminute,dayofweek,DST);
    LCD_setline(2);
    LCD_sendstring(string2);
}