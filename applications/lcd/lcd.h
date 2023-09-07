#ifndef __LCD_H
#define __LCD_H
#include <rtthread.h>
#include <rtdevice.h>
#include <stdlib.h>
#include "image_recognition.h"
#include "pin_config.h"
#include "stdlib.h"
#include "GC0308.h"
#include "main.h"

#define USE_HORIZONTAL 0       //设置横屏或者竖屏显示 0或1为竖屏 2或3为横屏

#define DFT_SCAN_DIR  U2D_R2L  //默认的扫描方向
#define LCD_USE8BIT_MODEL   0  //8位16位切换，1：8位
                               //             0:16位

//扫描方向定义
#define L2R_U2D  0x00 //从左到右,从上到下（正向）
#define L2R_D2U  0x80 //从左到右,从下到上
#define R2L_U2D  0x40 //从右到左,从上到下
#define R2L_D2U  0xc0 //从右到左,从下到上（旋转180度）

#define U2D_L2R  0x20 //从上到下,从左到右
#define U2D_R2L  0x60 //从上到下,从右到左（旋转90度）
#define D2U_L2R  0xa0 //从下到上,从左到右（旋转270度）
#define D2U_R2L  0xe0 //从下到上,从右到左

//LCD重要参数集
typedef struct
{
    rt_uint16_t width;  //LCD 宽度
    rt_uint16_t height; //LCD 高度
    rt_uint16_t id;     //LCD ID
    rt_uint8_t  dir;    //横屏还是竖屏控制：0，竖屏；1，横屏。
}_lcd_dev;

//LCD参数
extern _lcd_dev lcddev; //管理LCD重要参数

extern rt_uint16_t JpegBuffer[INPUT_HEIGHT][INPUT_WIDTH];               //原始图片缓冲

//////////////////////////////////////////////////////////////////////////////////
//-----------------LCD端口定义----------------
//#define LCD_LED PAout(5) //LCD背光  PA5

#define LCD_CS_SET  GPIOE->BSRR=1<<0    //片选端口  PA0
#define LCD_RS_SET  GPIOE->BSRR=1<<1    //数据/命令 PA1
#define LCD_WR_SET  GPIOE->BSRR=1<<2    //写数据    PA2
#define LCD_RD_SET  GPIOE->BSRR=1<<3    //读数据    PA3
#define LCD_RST_SET GPIOE->BSRR=1<<4    //复位      PA4

#define LCD_CS_CLR  GPIOE->BRR=1<<0     //片选端口  PA0
#define LCD_RS_CLR  GPIOE->BRR=1<<1     //数据/命令 PA1
#define LCD_WR_CLR  GPIOE->BRR=1<<2     //写数据    PA2
#define LCD_RD_CLR  GPIOE->BRR=1<<3     //读数据    PA3
#define LCD_RST_CLR GPIOE->BRR=1<<4     //复位      PA4

//#define LCD_DATA_OUT() {GPIOB->CRH=0X33333333;GPIOB->CRL=0X33333333;}
//#define LCD_DATA_IN()  {GPIOB->CRH=0X88888888;GPIOB->CRL=0X88888888;}
/*
#define LCD_DATA_OUT() {    rt_pin_mode(DB0 , PIN_MODE_OUTPUT); \
                            rt_pin_mode(DB1 , PIN_MODE_OUTPUT); \
                            rt_pin_mode(DB2 , PIN_MODE_OUTPUT); \
                            rt_pin_mode(DB3 , PIN_MODE_OUTPUT); \
                            rt_pin_mode(DB4 , PIN_MODE_OUTPUT); \
                            rt_pin_mode(DB5 , PIN_MODE_OUTPUT); \
                            rt_pin_mode(DB6 , PIN_MODE_OUTPUT); \
                            rt_pin_mode(DB7 , PIN_MODE_OUTPUT); \
                            rt_pin_mode(DB8 , PIN_MODE_OUTPUT); \
                            rt_pin_mode(DB9 , PIN_MODE_OUTPUT); \
                            rt_pin_mode(DB10, PIN_MODE_OUTPUT); \
                            rt_pin_mode(DB11, PIN_MODE_OUTPUT); \
                            rt_pin_mode(DB12, PIN_MODE_OUTPUT); \
                            rt_pin_mode(DB13, PIN_MODE_OUTPUT); \
                            rt_pin_mode(DB14, PIN_MODE_OUTPUT); \
                            rt_pin_mode(DB15, PIN_MODE_OUTPUT); }


#define LCD_DATA_IN()  {    rt_pin_mode(DB0 , PIN_MODE_INPUT);  \
                            rt_pin_mode(DB1 , PIN_MODE_INPUT);  \
                            rt_pin_mode(DB2 , PIN_MODE_INPUT);  \
                            rt_pin_mode(DB3 , PIN_MODE_INPUT);  \
                            rt_pin_mode(DB4 , PIN_MODE_INPUT);  \
                            rt_pin_mode(DB5 , PIN_MODE_INPUT);  \
                            rt_pin_mode(DB6 , PIN_MODE_INPUT);  \
                            rt_pin_mode(DB7 , PIN_MODE_INPUT);  \
                            rt_pin_mode(DB8 , PIN_MODE_INPUT);  \
                            rt_pin_mode(DB9 , PIN_MODE_INPUT);  \
                            rt_pin_mode(DB10, PIN_MODE_INPUT);  \
                            rt_pin_mode(DB11, PIN_MODE_INPUT);  \
                            rt_pin_mode(DB12, PIN_MODE_INPUT);  \
                            rt_pin_mode(DB13, PIN_MODE_INPUT);  \
                            rt_pin_mode(DB14, PIN_MODE_INPUT);  \
                            rt_pin_mode(DB15, PIN_MODE_INPUT);  }
*/

#define LCD_DATA_OUT() {GPIOD->MODER=0X55555555;GPIOD->OTYPER=0X00000000;GPIOD->OSPEEDR=0X55555555;GPIOD->PUPDR =0X00000000;}
#define LCD_DATA_IN()  {GPIOD->MODER=0X00000000;GPIOD->PUPDR =0X00000000;}

#define DATAOUT(x)  GPIOD->ODR=x;       //数据输出
#define DATAIN      GPIOD->IDR;         //数据输入

//////////////////////////////////////////////////////////////////////////////////

//画笔颜色
#define WHITE            0xFFFF
#define BLACK            0x0000
#define BLUE             0x001F
#define BRED             0XF81F
#define GRED             0XFFE0
#define GBLUE            0X07FF
#define RED              0xF800
#define MAGENTA          0xF81F
#define GREEN            0x07E0
#define CYAN             0x7FFF
#define YELLOW           0xFFE0
#define BROWN            0XBC40 //棕色
#define BRRED            0XFC07 //棕红色
#define GRAY             0X8430 //灰色
//GUI颜色

#define DARKBLUE         0X01CF //深蓝色
#define LIGHTBLUE        0X7D7C //浅蓝色
#define GRAYBLUE         0X5458 //灰蓝色
//以上三色为PANEL的颜色

#define LIGHTGREEN       0X841F //浅绿色
//#define LIGHTGRAY        0XEF5B //浅灰色(PANNEL)
#define LGRAY            0XC618 //浅灰色(PANNEL),窗体背景色

#define LGRAYBLUE        0XA651 //浅灰蓝色(中间层颜色)
#define LBBLUE           0X2B12 //浅棕蓝色(选择条目的反色)


void LCD_WR_REG(rt_uint16_t reg);                                                   //写寄存器
void LCD_WR_data(rt_uint16_t data);                                                         //写数据
void LCD_WR_DATA(rt_uint16_t data);                                                 //写数据
rt_uint16_t LCD_RD_DATA(void);                                                      //读点
rt_uint16_t LCD_ReadReg(rt_uint16_t LCD_Reg);                                       //读寄存器
void LCD_SetCursor(rt_uint16_t x,rt_uint16_t y);                                    //设置光标位置
void LCD_Address_Set(rt_uint16_t x1,rt_uint16_t y1,rt_uint16_t x2,rt_uint16_t y2);  //设置坐标函数
void LCD_DisplayOn(void);                                                           //开显示
void LCD_DisplayOff(void);                                                          //关显示
rt_uint16_t LCD_ReadPoint(rt_uint16_t x,rt_uint16_t y);                             //读点函数
void LCD_Clear(rt_uint16_t color);                                                  //清屏函数

void LCD_Fill(rt_uint16_t xsta,rt_uint16_t ysta,rt_uint16_t xend,rt_uint16_t yend,rt_uint16_t color);       //指定区域填充颜色
void LCD_DrawPoint(rt_uint16_t x,rt_uint16_t y,rt_uint16_t color);                                          //在指定位置画一个点
void LCD_DrawLine(rt_uint16_t x1,rt_uint16_t y1,rt_uint16_t x2,rt_uint16_t y2,rt_uint16_t color);           //在指定位置画一条线
void LCD_DrawRectangle(rt_uint16_t x1, rt_uint16_t y1, rt_uint16_t x2, rt_uint16_t y2,rt_uint16_t color);   //在指定位置画一个矩形
void Draw_Circle(rt_uint16_t x0,rt_uint16_t y0,rt_uint8_t r,rt_uint16_t color);                             //在指定位置画一个圆

void LCD_ShowChinese(rt_uint16_t x,rt_uint16_t y,rt_uint8_t *s,rt_uint16_t fc,rt_uint16_t bc,rt_uint8_t sizey,rt_uint8_t mode);         //显示汉字串
void LCD_ShowChinese12x12(rt_uint16_t x,rt_uint16_t y,rt_uint8_t *s,rt_uint16_t fc,rt_uint16_t bc,rt_uint8_t sizey,rt_uint8_t mode);    //显示单个12x12汉字
void LCD_ShowChinese16x16(rt_uint16_t x,rt_uint16_t y,rt_uint8_t *s,rt_uint16_t fc,rt_uint16_t bc,rt_uint8_t sizey,rt_uint8_t mode);    //显示单个16x16汉字
void LCD_ShowChinese24x24(rt_uint16_t x,rt_uint16_t y,rt_uint8_t *s,rt_uint16_t fc,rt_uint16_t bc,rt_uint8_t sizey,rt_uint8_t mode);    //显示单个24x24汉字
void LCD_ShowChinese32x32(rt_uint16_t x,rt_uint16_t y,rt_uint8_t *s,rt_uint16_t fc,rt_uint16_t bc,rt_uint8_t sizey,rt_uint8_t mode);    //显示单个32x32汉字

void LCD_ShowChar(rt_uint16_t x,rt_uint16_t y,rt_uint8_t num,rt_uint16_t fc,rt_uint16_t bc,rt_uint8_t sizey,rt_uint8_t mode);           //显示一个字符
void LCD_ShowString(rt_uint16_t x,rt_uint16_t y,const rt_uint8_t *p,rt_uint16_t fc,rt_uint16_t bc,rt_uint8_t sizey,rt_uint8_t mode);    //显示字符串
rt_uint32_t mypow(rt_uint8_t m,rt_uint8_t n);                                                                                           //求幂
void LCD_ShowIntNum(rt_uint16_t x,rt_uint16_t y,rt_uint16_t num,rt_uint8_t len,rt_uint16_t fc,rt_uint16_t bc,rt_uint8_t sizey);         //显示整数变量
void LCD_ShowFloatNum1(rt_uint16_t x,rt_uint16_t y,float num,rt_uint8_t len,rt_uint16_t fc,rt_uint16_t bc,rt_uint8_t sizey);            //显示两位小数变量

void LCD_ShowPicture(rt_uint16_t x,rt_uint16_t y,rt_uint16_t length,rt_uint16_t width,const rt_uint16_t pic[][INPUT_WIDTH]);            //显示图片
void LCD_Show_Target_Subimages(rt_uint16_t x,rt_uint16_t y,rt_uint16_t length,rt_uint16_t width,const rt_uint16_t pic[SUBIMAGE_HEIGHT][SUBIMAGE_WIDTH]);    //显示目标子图片

void Set_Dir(rt_uint8_t dir);
void LCD_Init(void);                                                                                                                    //初始化

void LCD_Reponse(void);        //LCD响应函数

#endif
