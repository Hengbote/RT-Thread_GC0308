#ifndef __TOUCH_H__
#define __TOUCH_H__

#include "GT911.h"

#define TP_PRES_DOWN 0x80  //触屏被按下
#define TP_CATH_PRES 0x40  //有按键按下了
#define CT_MAX_TOUCH  5    //电容屏支持的点数,固定为5点

//触摸屏控制器
typedef struct
{
    rt_uint8_t (*init)(void);               //初始化触摸屏控制器
    rt_uint8_t (*GT911_Scan)(rt_uint8_t);   //扫描触摸屏.0,屏幕扫描;1,物理坐标;
    rt_uint16_t x[CT_MAX_TOUCH];            //当前坐标
    rt_uint16_t y[CT_MAX_TOUCH];            //电容屏有最多5组坐标,电阻屏则用x[0],y[0]代表:此次扫描时,触屏的坐标,用
                                            //x[4],y[4]存储第一次按下时的坐标.
    rt_uint8_t  sta;                        //笔的状态
                                            //b7:按下1/松开0;
                                            //b6:0,没有按键按下;1,有按键按下.
                                            //b5:保留
                                            //b4~b0:电容触摸屏按下的点数(0,表示未按下,1表示按下)
/////////////////////触摸屏校准参数(电容屏不需要校准)//////////////////////
    float xfac;
    float yfac;
    short xoff;
    short yoff;
    rt_uint8_t touchtype;                   //新增的参数,当触摸屏的左右上下完全颠倒时需要用到.
                                            //b0:0,竖屏(适合左右为X坐标,上下为Y坐标的TP)
                                            //   1,横屏(适合左右为Y坐标,上下为X坐标的TP)
                                            //b1~6:保留.
                                            //b7:0,电阻屏
                                            //   1,电容屏

}_m_tp_dev;

extern _m_tp_dev tp_dev;        //触屏控制器在touch.c里面定义

void TP_Drow_Touch_Point(rt_uint16_t x,rt_uint16_t y,rt_uint16_t color);//画一个坐标校准点
void TP_Draw_Big_Point(rt_uint16_t x,rt_uint16_t y,rt_uint16_t color);  //画一个大点
rt_uint8_t TP_Init(void);                               //初始化

void Touch_Reponse(void);

#endif
