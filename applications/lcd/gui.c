//#include "lcd.h"
//#include "string.h"
//#include "delay.h"
//#include "gui.h"
//
///******************************************************************************
//      函数说明：在指定位置画点
//      入口数据：x,y 画点坐标
//                color 点的颜色
//      返回值：  无
//******************************************************************************/
//void GUI_DrawPoint(rt_uint16_t x,rt_uint16_t y,rt_uint16_t color)
//{
//    LCD_Address_Set(x,y,x,y);//设置光标位置
//    LCD_WR_DATA(color);
//}
///******************************************************************************
//      函数说明：8对称性画圆算法(内部调用)
//      入口数据：(xc,yc) :圆中心坐标
//                (x,y):光标相对于圆心的坐标
//                c 点的颜色
//
//      返回值：  无
//******************************************************************************/
//void _draw_circle_8(int xc, int yc, int x, int y, rt_uint16_t c)
//{
//    GUI_DrawPoint(xc + x, yc + y, c);
//
//    GUI_DrawPoint(xc - x, yc + y, c);
//
//    GUI_DrawPoint(xc + x, yc - y, c);
//
//    GUI_DrawPoint(xc - x, yc - y, c);
//
//    GUI_DrawPoint(xc + y, yc + x, c);
//
//    GUI_DrawPoint(xc - y, yc + x, c);
//
//    GUI_DrawPoint(xc + y, yc - x, c);
//
//    GUI_DrawPoint(xc - y, yc - x, c);
//}
//
///******************************************************************************
//      函数说明：在指定位置画一个指定大小的圆
//      入口数据：(xc,yc) :圆中心坐标
//                c:填充的颜色
//                r:圆半径
//                fill:填充判断标志，1-填充，0-不填充
//      返回值：  无
//******************************************************************************/
//void gui_circle(int xc, int yc,rt_uint16_t c,int r, int fill)
//{
//    int x = 0, y = r, yi, d;
//
//    d = 3 - 2 * r;
//
//
//    if (fill)
//    {
//        // 如果填充（画实心圆）
//        while (x <= y) {
//            for (yi = x; yi <= y; yi++)
//                _draw_circle_8(xc, yc, x, yi, c);
//
//            if (d < 0) {
//                d = d + 4 * x + 6;
//            } else {
//                d = d + 4 * (x - y) + 10;
//                y--;
//            }
//            x++;
//        }
//    } else
//    {
//        // 如果不填充（画空心圆）
//        while (x <= y) {
//            _draw_circle_8(xc, yc, x, y, c);
//            if (d < 0) {
//                d = d + 4 * x + 6;
//            } else {
//                d = d + 4 * (x - y) + 10;
//                y--;
//            }
//            x++;
//        }
//    }
//}
//
///******************************************************************************
//      函数说明：显示一个字符串,包含中英文显示
//      入口数据：x,y :起点坐标
//                c:填充的颜色
//                                fc:前置画笔颜色
//                                bc:背景颜色
//                                str :字符串
//                                size:字体大小
//                                mode:模式 0,填充模式;1,叠加模式
//      返回值：  无
//******************************************************************************/
//void Show_Str(rt_uint16_t x, rt_uint16_t y,rt_uint8_t *str,rt_uint16_t fc, rt_uint16_t bc,rt_uint8_t sizey,rt_uint8_t mode)
//{
//    rt_uint16_t x0=x;
//  rt_uint8_t bHz=0;     //字符或者中文
//    while(*str!=0)//数据未结束
//    {
//        if(!bHz)
//        {
//            if(x>(lcddev.width-sizey/2)||y>(lcddev.height-sizey)) return;
//            if(*str>0x80)bHz=1;//中文
//            else              //字符
//            {
//                if(*str==0x0D)//换行符号
//                {
//                    y+=sizey;
//                    x=x0;
//                    str++;
//                }else
//                {
//                    LCD_ShowChar(x,y,*str,fc,bc,sizey,mode);
//                    x+=sizey/2; //字符,为全字的一半
//                }
//              str++;
//            }
//        }else//中文
//        {
//            if(x>(lcddev.width-sizey)||y>(lcddev.height-sizey)) return;
//            bHz=0;//有汉字库
//            if(sizey==32)
//                LCD_ShowChinese32x32(x,y,str,fc,bc,sizey,mode);
//            else if(sizey==24)
//                LCD_ShowChinese24x24(x,y,str,fc,bc,sizey,mode);
//            else if(sizey==16)
//              LCD_ShowChinese16x16(x,y,str,fc,bc,sizey,mode);
//            else
//              LCD_ShowChinese12x12(x,y,str,fc,bc,sizey,mode);
//                str+=2;
//                x+=sizey;//下一个汉字偏移
//            }
//    }
//}
//
///******************************************************************************
//      函数说明：居中显示一个字符串,包含中英文显示
//      入口数据：x,y :起点坐标
//                c:填充的颜色
//                                fc:前置画笔颜色
//                                bc:背景颜色
//                                str :字符串
//                                size:字体大小
//                                mode:模式 0,填充模式;1,叠加模式
//      返回值：  无
//******************************************************************************/
//void Gui_StrCenter(rt_uint16_t x, rt_uint16_t y,rt_uint8_t *str,rt_uint16_t fc,rt_uint16_t bc,rt_uint8_t sizey,rt_uint8_t mode)
//{
//    rt_uint16_t len=strlen((const char *)str);
//    rt_uint16_t x1=(lcddev.width-len*8)/2;
//    Show_Str(x+x1,y,str,fc,bc,sizey,mode);
//}
