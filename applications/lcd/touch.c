#include "touch.h" 
#include "lcd.h"
//#include "delay.h"
#include "stdlib.h"
#include "math.h"
//#include "24cxx.h"
//#include "key.h"

#define DBG_TAG "touch"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

_m_tp_dev tp_dev=
{
    TP_Init,
    GT911_Scan,
    {0},    // 对 x 进行初始化
    {0},    // 对 y 进行初始化
    0,
    0,
    0,
    0,
    0,
    0,
};

//默认为touchtype=0的数据.
rt_uint8_t CMD_RDX=0XD0;
rt_uint8_t CMD_RDY=0X90;

//5个触控点的颜色(电容触摸屏用)
const rt_uint16_t POINT_COLOR_TBL[5]={RED,GREEN,BLUE,BROWN,GRED};

rt_thread_t touch_response_t;       //触摸模块任务结构体

//////////////////////////////////////////////////////////////////////////////////
//与LCD部分有关的函数
//画一个触摸点
//用来校准用的
//x,y:坐标
//color:颜色
void TP_Drow_Touch_Point(rt_uint16_t x,rt_uint16_t y,rt_uint16_t color)
{
    LCD_DrawLine(x-12,y,x+13,y,color);//横线
    LCD_DrawLine(x,y-12,x,y+13,color);//竖线
    LCD_DrawPoint(x+1,y+1,color);
    LCD_DrawPoint(x-1,y+1,color);
    LCD_DrawPoint(x+1,y-1,color);
    LCD_DrawPoint(x-1,y-1,color);
    Draw_Circle(x,y,6,color);//画中心圈
}
//画一个大点(2*2的点)
//x,y:坐标
//color:颜色
void TP_Draw_Big_Point(rt_uint16_t x,rt_uint16_t y,rt_uint16_t color)
{
    LCD_DrawPoint(x,y,color);//中心点
    LCD_DrawPoint(x+1,y,color);
    LCD_DrawPoint(x,y+1,color);
    LCD_DrawPoint(x+1,y+1,color);
}

//触摸屏初始化
//返回值:0,没有进行校准
//       1,进行过校准
rt_uint8_t TP_Init(void)
{
    GT911_Init();
    tp_dev.touchtype|=0X80; //电容屏
    tp_dev.touchtype|=(~lcddev.dir)&0X01;//横屏还是竖屏
//    tp_dev.touchtype|=lcddev.dir&0X01;//横屏还是竖屏
    return 0;

//    return 1;
}

void Touch_Reponse_Callback(void *parameter)
{

    rt_uint8_t t=0;
    rt_uint16_t lastpos[5][2];      //最后一次的数据

    tp_dev.init();                      //触摸屏初始化
    LCD_Clear(WHITE);//清屏

    while(1)
    {
        tp_dev.GT911_Scan(0);
        for(t=0;t<5;t++)
        {
            if((tp_dev.sta)&(1<<t))
            {
                //printf("X坐标:%d,Y坐标:%d\r\n",tp_dev.x[0],tp_dev.y[0]);
                if(tp_dev.x[t]<lcddev.width&&tp_dev.y[t]<lcddev.height)
                {
                    if(lastpos[t][0]==0XFFFF)
                    {
                        lastpos[t][0] = tp_dev.x[t];
                        lastpos[t][1] = tp_dev.y[t];
                    }

                    LCD_DrawLine(lastpos[t][0],lastpos[t][1],tp_dev.x[t],tp_dev.y[t],POINT_COLOR_TBL[t]);//画线
                    lastpos[t][0]=tp_dev.x[t];
                    lastpos[t][1]=tp_dev.y[t];
                    if(tp_dev.x[t]>(lcddev.width-24)&&tp_dev.y[t]<20)
                        LCD_Clear(WHITE);//清屏
                }
            }else lastpos[t][0]=0XFFFF;
        }
    }
}

void Touch_Reponse(void)
{
    touch_response_t = rt_thread_create("touch_response_t", Touch_Reponse_Callback, RT_NULL, 2048, 10, 10);
    if(touch_response_t!=RT_NULL)rt_thread_startup(touch_response_t);
        LOG_I("Touch_Reponse Init Success\r\n");
}

