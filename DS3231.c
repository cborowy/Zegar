/*
 * DS3231.c

 *
 *  Created on: 14 gru 2015
 *      Author: cborowy
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdio.h>

#include "HD44780.h"
#include "i2c_twi.h"
#include "DS3231.h"

#define DS3231_ADDR 0xD0

char days[7][4] = {
		"pon", "wto", "sro", "czw", "pia", "sob", "nie"
};

 uint8_t intTime[3];
 uint8_t intDate[4];

 char charTime[8];
 char charDate[16];

 TDATETIME dateTime;

void holdTime(TDATETIME * dt){
	intTime[0] = dt -> hh;
	intTime[1] = dt -> mm;
	intTime[2] = dt -> ss;
}

void holdDate(TDATETIME * dt){
	intDate[0] = dt -> day;
	intDate[1] = dt -> month;
	intDate[2] = dt -> year;
	intDate[3] = dt -> dayOfWeek;
}

void getHeldTime(void){
	sprintf(charTime,"%02d:%02d:%02d", intTime[0], intTime[1], intTime[2]);
}

void getHeldDate(void){
	sprintf(charDate, "%02d-%02d-20%02d   %s", intDate[0], intDate[1], intDate[2], days[intDate[3]]);
}

void DS3231_init( void ) {
	uint8_t ctrl = 0;
	TWI_write_buf( DS3231_ADDR, 0x0e, 1, &ctrl );
}

void DS3231_getDateTime( TDATETIME * dt ) {
	uint8_t i;
	uint8_t buf[7];
	TWI_read_buf( DS3231_ADDR, 0x00, 7, buf );
	for( i=0; i<7; i++ ) dt->bytes[i] = bcd2dec( buf[i] );
}

void show_time( TDATETIME * dt ) {
	char time[8];
	sprintf(time, "%02d:%02d:%02d", dt->hh, dt->mm, dt->ss);
	LCD_WriteText(time);
}

void show_date(TDATETIME * dt ) {
	char date[16];
	sprintf(date, "%02d-%02d-20%02d   %s", dt->day, dt->month, dt->year, days[dt -> dayOfWeek- 1]);
	LCD_WriteText(date);
}

void DS3231_setTime( uint8_t hh, uint8_t mm, uint8_t ss ) {
	uint8_t buf[3];
	buf[0]=dec2bcd(ss);
	buf[1]=dec2bcd(mm);
	buf[2]=dec2bcd(hh);
	TWI_write_buf( DS3231_ADDR, 0x00, 3, buf );
}

void DS3231_setDate( uint8_t year, uint8_t month, uint8_t day, uint8_t dayofweek ) {
	uint8_t buf[4];
	buf[0]=dayofweek;
	buf[1]=dec2bcd(day);
	buf[2]=dec2bcd(month);
	buf[3]=dec2bcd(year);
	TWI_write_buf( DS3231_ADDR, 0x03, 4, buf );
}

// konwersja liczby dziesiêtnej na BCD
uint8_t dec2bcd(uint8_t dec) {
	return dec + 6 * (dec / 10);
}

// konwersja liczby BCD na dziesiêtn¹
uint8_t bcd2dec(uint8_t bcd) {
	return bcd - 6 * (bcd >> 4);
}
