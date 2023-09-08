#ifndef APPLICATIONS_PIN_CONFIG_H_
#define APPLICATIONS_PIN_CONFIG_H_

#include <board.h>

//IO
#define GREEN_PIN 39
#define RED_PIN 98
#define BLUE_PIN 23

#define SCL_PIN 43
#define SDA_PIN 42


//屏幕
#define DB0         GET_PIN(D, 0)
#define DB1         GET_PIN(D, 1)
#define DB2         GET_PIN(D, 2)
#define DB3         GET_PIN(D, 3)
#define DB4         GET_PIN(D, 4)
#define DB5         GET_PIN(D, 5)
#define DB6         GET_PIN(D, 6)
#define DB7         GET_PIN(D, 7)
#define DB8         GET_PIN(D, 8)
#define DB9         GET_PIN(D, 9)
#define DB10        GET_PIN(D, 10)
#define DB11        GET_PIN(D, 11)
#define DB12        GET_PIN(D, 12)
#define DB13        GET_PIN(D, 13)
#define DB14        GET_PIN(D, 14)
#define DB15        GET_PIN(D, 15)
#define LCD_RD      GET_PIN(E, 0)
#define LCD_WR      GET_PIN(E, 1)
#define LCD_RS      GET_PIN(E, 2)
#define LCD_CS      GET_PIN(E, 3)
#define LCD_RST     GET_PIN(E, 4)

//触摸
#define GT911_RST   GET_PIN(C, 12)
#define GT911_INT   GET_PIN(C, 10)
//#define GT911_SDA   GET_PIN(F, 6)
//#define GT911_SCL   GET_PIN(F, 7)

/**/
//#define USE_BARE

#endif /* APPLICATIONS_PIN_CONFIG_H_ */
