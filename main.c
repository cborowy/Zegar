/*
 * main.c


 *
 *  Created on: 26 lis 2015
 *      Author: cborowy
 */

#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdio.h>


#include "HD44780.h"
#include "i2c_twi.h"
#include "DS3231.h"
#include "main.h"

#define BUTTON1_PORT PORTD
#define BUTTON1_PIN PIND
#define BUTTON1_BIT PD0

#define BUTTON2_PORT PORTD
#define BUTTON2_PIN PIND
#define BUTTON2_BIT PD1

#define BUTTON3_PORT PORTD
#define BUTTON3_PIN PIND
#define BUTTON3_BIT PD2

#define BUTTON4_PORT PORTD
#define BUTTON4_PIN PIND
#define BUTTON4_BIT PD3

uint8_t menuFlag = 0;
uint8_t functionFlag = 0;
uint8_t timePosition = 0;
uint8_t datePosition = 0;
uint8_t daysInMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

int main(void){
	DDRD = 0xf0;
	PORTD = 0x0f;

	LCD_Initalize();
	LCD_Clear();
	LCD_Home();

	i2cSetBitrate(100);

	DS3231_init();
	DS3231_setTime(0 ,0 ,0);
	DS3231_setDate(0, 0, 0, 1);

	while(1) {
		button();
		showOnScreen();
	}
	return 0;
}

void button(void){
			if(bit_is_clear(BUTTON1_PIN, BUTTON1_PIN)){
				upButton();
			}else if(bit_is_clear(BUTTON2_PIN, BUTTON2_BIT)){
				downButton();
			}else if(bit_is_clear(BUTTON3_PIN, BUTTON3_BIT)){
				leftButton();
			}else if(bit_is_clear(BUTTON4_PIN, BUTTON4_BIT)){
				rightButton();
			}
			_delay_ms(2000);
}

void rightButton(void){
	if(functionFlag == 0){
		changeMenu(1);
	} else if(functionFlag == 1){
		if(menuFlag == 1){
			if(timePosition != 2){
				timePosition++;
			}else if(timePosition == 2){
				DS3231_setTime(intTime[0], intTime[1], intTime[2]);
				functionFlag = 0;
			}
		}
	}
}

void leftButton(void){
	if(functionFlag == 0){
		changeMenu(0);
	} else if (functionFlag == 1){
		if(menuFlag == 1){
			if(timePosition != 0){
				timePosition--;
			}else if(timePosition == 0){
				functionFlag = 0;
			}
		}
	}
}

void upButton(void){
	if(functionFlag == 0){
		changeFunction();
	} else if (functionFlag == 1){
		if(menuFlag == 1){
			if(timePosition == 0){
				if(intTime[timePosition] != 23){
					intTime[timePosition]++;
				}else if(intTime[timePosition] == 23){
					intTime[timePosition] = 0;
				}
			}else if(timePosition == 1){
				if(intTime[timePosition] != 59){
					intTime[timePosition]++;
				}else if(intTime[timePosition] == 59){
					intTime[timePosition] = 0;
				}
			} else if(timePosition == 2){
				if(intTime[timePosition] != 59){
					intTime[timePosition]++;
				}else if(intTime[timePosition] == 59){
					intTime[timePosition] = 0;
				}
			}
			getHeldTime();//pokazuje ustawiany czas
		}else if(menuFlag == 2){
			if(datePosition == 0){
				if(daysOfMonthCorrect() == 1){
					intDate[datePosition]++;
				}else if (daysOfMonthCorrect() == 0){
					intDate[datePosition] = 0;
				}
			}else if(datePosition == 1){
				if(intDate[datePosition] != 12){
					intDate[datePosition]++;
				}else if(intDate[datePosition] == 12){
					intDate[datePosition] = 1;
				}
			}
			getHeldDate();//pokazuje ustawiana date
		}
	}
}

uint8_t daysOfMonthCorrect(void){
	if (intDate[0] + 1 == daysInMonth[intDate[1]]){
		return 1;
	} else if(intDate[0] + 1 != daysInMonth[intDate[1]]){
		return 0;
	}
	return 2;
}

uint8_t daysOfMonth(void){
	uint8_t buf;
	buf = daysInMonth[intDate[1]];
	return buf;

}

void downButton(void){
	if(functionFlag == 0){

	}else if(functionFlag == 1){
		if(menuFlag == 1){
			if(timePosition == 0){
				if(intTime[0] != 0){
					intTime[0]--;
				}else if(intTime[0] == 0){
					intTime[0] = 23;
				}
			}else if(timePosition == 1){
				if(intTime[timePosition] != 0){
					intTime[timePosition]--;
				}else if(intTime[timePosition] == 0){
					intTime[timePosition] = 60;
				}
			} else if(timePosition == 2){
				if(intTime[timePosition] != 0){
					intTime[timePosition]--;
				}else if(intTime[timePosition] == 0){
					intTime[timePosition] = 60;
				}
			}
			getHeldTime();
		}else if(menuFlag == 2){
			if(datePosition == 0){
				if(daysOfMonthCorrect() == 1){
					intDate[datePosition]++;
				}else if (daysOfMonthCorrect() == 0){
					intDate[datePosition] = 0;
				}
			}else if(datePosition == 1){
				if(intDate[datePosition] != 12){
					intDate[datePosition]++;
				}else if(intDate[datePosition] == 12){
					intDate[datePosition] = 1;
				}
			}
			getHeldDate();//pokazuje ustawiana date
		}
	}
}

void showOnScreen(void){

		if (menuFlag == 0){
			timeAndDateView();
		}else if(menuFlag == 1){
			menuSetTimeView();
			if (functionFlag == 1){
				functionSetTimeView();
			}
		}else if(menuFlag == 2){
			menuSetDateView();
		}else if(menuFlag == 3){
			menuSetAlarmView();
		}else if(menuFlag == 4){
			onOffAlarmView();
		}
}

void timeAndDateView(void){
	DS3231_getDateTime(&dateTime);
	showTime();
	showDate();

}

void showTime(void){
	LCD_Clear();
	show_time(&dateTime);
	LCD_GoTo(0,1);

}

void showDate(void){
	show_date(&dateTime);
}

void menuSetTimeView(void){
	LCD_Clear();
	LCD_WriteText("Ustaw Godzine");
}

void menuSetDateView(){
	LCD_Clear();
	LCD_WriteText("Ustaw Date");
}

void menuSetAlarmView(){
	LCD_Clear();
	LCD_WriteText("Ustaw Alarm");
}

void functionSetTimeView(){
	holdTime(&dateTime);
	getHeldTime();
	while(functionFlag == 1){
		LCD_Clear();
		LCD_WriteText(charTime);
		if(timePosition == 0){
			LCD_GoTo(0,1);
			LCD_WriteText("--");
		}else if(timePosition == 1){
			LCD_GoTo(3,1);
			LCD_WriteText("--");
		}else if(timePosition == 2){
			LCD_GoTo(6,1);
			LCD_WriteText("--");
		}
		button();
	}
}

void onOffAlarmView(void){
	LCD_Clear();
	LCD_WriteText("On / Off");
}

void changeMenu(uint8_t i){
	if(i == 0){
		if(menuFlag == 0){
			menuFlag = 5;
		} else {
			menuFlag--;
		}
	} else if (i == 1){
		if(menuFlag == 5) {
			menuFlag = 0;
		} else {
			menuFlag++;
		}
	}
}

void changeFunction(void){
	if(functionFlag == 0){
		functionFlag = 1;
	}else if (functionFlag == 1){
		functionFlag = 0;
	}
}
