/*
 * DS3231.h
 *
 *  Created on: 14 gru 2015
 *      Author: cborowy
 */

#ifndef DS3231_H_
#define DS3231_H_

typedef union {
	uint8_t bytes[7];
	struct {
		uint8_t ss;
		uint8_t mm;
		uint8_t hh;
		uint8_t dayOfWeek;
		uint8_t day;
		uint8_t month;
		uint8_t year;
	};
} TDATETIME;

extern TDATETIME dateTime;

extern uint8_t intTime[3];
extern uint8_t intDate[4];

extern char charTime[8];
extern char charDate[16];

void DS3231_init( void );
uint8_t dec2bcd(uint8_t dec);
uint8_t bcd2dec(uint8_t bcd);
void DS3231_getDateTime( TDATETIME * dt );
void show_time( TDATETIME * dt );
void show_date( TDATETIME * dt );
void DS3231_setTime( uint8_t hh, uint8_t mm, uint8_t ss );
void DS3231_setDate( uint8_t year, uint8_t month, uint8_t day, uint8_t dayofweek );

void holdTime(TDATETIME * dt);
void holdDate(TDATETIME * dt);
void getHeldTime(void);
void getHeldDate(void);
#endif /* DS3231_H_ */
