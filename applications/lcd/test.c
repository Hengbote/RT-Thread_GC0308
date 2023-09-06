//#include "lcd.h"
//#include "gui.h"
//#include "test.h"
//#include "touch.h"
//#include "pic.h"
//
////========================variable==========================//
//rt_uint16_t ColorTab[5]={BRED,YELLOW,RED,GREEN,BLUE};//定义颜色数组
////=====================end of variable======================//
//
///******************************************************************************
//功能：    绘制测试界面
//输入参数：str :字符串指针
//返回值：  无
//******************************************************************************/
//void DrawTestPage(rt_uint8_t *str)
//{
////绘制固定栏up
//LCD_Fill(0,0,lcddev.width,20,BLUE);
////绘制固定栏down
//LCD_Fill(0,lcddev.height-20,lcddev.width,lcddev.height,BLUE);
//Gui_StrCenter(0,2,str,WHITE,BLUE,16,1);//居中显示
//Gui_StrCenter(0,lcddev.height-18,"中景园版权所有 ",WHITE,BLUE,16,1);//居中显示
////绘制测试区域
//LCD_Fill(0,20,lcddev.width,lcddev.height-20,BLACK);
//}
//
////******************************************************************
////函数名：  main_test
////功能：    绘制全动电子综合测试程序主界面
////输入参数：无
////返回值：  无
////修改记录：
////******************************************************************
//void main_test(void)
//{
//    DrawTestPage("中景园电子综合测试程序");
//
//    Gui_StrCenter(0,30,"中景园电子",RED,BLUE,16,1);//居中显示
//    Gui_StrCenter(0,60,"综合测试程序",RED,BLUE,16,1);//居中显示
//    Gui_StrCenter(0,90,"ILI9341 240X320",RED,BLUE,16,1);//居中显示
//    Gui_StrCenter(0,120,"中景园电子 2020-05-05",RED,BLUE,16,1);//居中显示
//    delay_ms(1000);
//    delay_ms(1000);
//}
//
////******************************************************************
////函数名：  Test_Color
////功能：    颜色填充测试，依次填充白色、黑色、红色、绿色、蓝色
////输入参数：无
////返回值：  无
////修改记录：
////******************************************************************
//void Test_Color(void)
//{
//    DrawTestPage("测试1:纯色填充测试");
//    LCD_Fill(0,20,lcddev.width,lcddev.height-20,WHITE);
//    Show_Str(lcddev.width-50,30,"White",BLUE,YELLOW,16,1);delay_ms(500);
//    LCD_Fill(0,20,lcddev.width,lcddev.height-20,BLACK);
//    Show_Str(lcddev.width-50,30,"Black",BLUE,YELLOW,16,1);delay_ms(500);
//    LCD_Fill(0,20,lcddev.width,lcddev.height-20,RED);
//    Show_Str(lcddev.width-50,30,"Red",BLUE,YELLOW,16,1); delay_ms(500);
//    LCD_Fill(0,20,lcddev.width,lcddev.height-20,GREEN);
//    Show_Str(lcddev.width-50,30,"Green",BLUE,YELLOW,16,1);delay_ms(500);
//    LCD_Fill(0,20,lcddev.width,lcddev.height-20,BLUE);
//    Show_Str(lcddev.width-50,30,"Blue",BLUE,YELLOW,16,1);delay_ms(500);
//
//}
//
////******************************************************************
////函数名：  Test_FillRec
////功能：    矩形框显示和填充测试，依次显示粉红色、黄色、红色、绿色、蓝色矩形框，
////          延时1500毫秒后，依次按照粉红色、黄色、红色、绿色、蓝色填充矩形框
////输入参数：无
////返回值：  无
////修改记录：
////******************************************************************
//void Test_FillRec(void)
//{
//    rt_uint8_t i=0;
//    DrawTestPage("测试2:GUI矩形填充测试");
//    LCD_Fill(0,20,lcddev.width,lcddev.height-20,WHITE);
//    for (i=0; i<5; i++)
//    {
//        LCD_DrawRectangle(lcddev.width/2-80+(i*15),lcddev.height/2-80+(i*15),lcddev.width/2-80+(i*15)+60,lcddev.height/2-80+(i*15)+60,ColorTab[i]);
//    }
//    delay_ms(1500);
//    LCD_Fill(0,20,lcddev.width,lcddev.height-20,WHITE);
//    for (i=0; i<5; i++)
//    {
//        LCD_Fill(lcddev.width/2-80+(i*15),lcddev.height/2-80+(i*15),lcddev.width/2-80+(i*15)+60,lcddev.height/2-80+(i*15)+60,ColorTab[i]);
//    }
//    delay_ms(1500);
//}
//
////******************************************************************
////函数名：  Test_FillRec
////功能：    圆形框显示和填充测试，依次显示粉红色、黄色、红色、绿色、蓝色圆形框，
////          延时1500毫秒后，依次按照粉红色、黄色、红色、绿色、蓝色填充圆形框
////输入参数：无
////返回值：  无
////修改记录：
////******************************************************************
//void Test_Circle(void)
//{
//    rt_uint8_t i=0;
//    DrawTestPage("测试3:GUI画圆填充测试");
//    LCD_Fill(0,20,lcddev.width,lcddev.height-20,WHITE);
//    for (i=0; i<5; i++)
//        gui_circle(lcddev.width/2-80+(i*25),lcddev.height/2-50+(i*25),ColorTab[i],30,0);
//    delay_ms(1500);
//    LCD_Fill(0,20,lcddev.width,lcddev.height-20,WHITE);
//    for (i=0; i<5; i++)
//        gui_circle(lcddev.width/2-80+(i*25),lcddev.height/2-50+(i*25),ColorTab[i],30,1);
//    delay_ms(1500);
//}
//
////******************************************************************
////函数名：  English_Font_test
////功能：    英文显示测试
////输入参数：无
////返回值：  无
////修改记录：
////******************************************************************
//void English_Font_test(void)
//{
//    DrawTestPage("测试4:英文显示测试");
//    Show_Str(10,40,"8X16:abcdefghijklmnopqrstuvwxyz0123456789",RED,BLUE,16,0);
//    Show_Str(10,70,"8X16:ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789",RED,BLUE,16,1);
//    Show_Str(10,100,"8X16:~!@#$%^&*()_+{}:<>?/|-+.",RED,BLUE,16,0);
//    delay_ms(1200);
//}
//
////******************************************************************
////函数名：  Chinese_Font_test
////功能：    中文显示测试
////输入参数：无
////返回值：  无
////修改记录：
////******************************************************************
//void Chinese_Font_test(void)
//{
//    DrawTestPage("测试5:中文显示测试");
//    Show_Str(10,30,"16X16:中景园电子技术有限公司欢迎您",BLUE,YELLOW,16,0);
//    Show_Str(10,50,"16X16:Welcome中景园",BLUE,YELLOW,16,1);
//    Show_Str(10,70,"24X24:Welcome中景园",BLUE,YELLOW,24,1);
//    Show_Str(10,100,"32X32:Welcome中景园",BLUE,YELLOW,32,1);
//    delay_ms(1200);
//}
//
////******************************************************************
////函数名：  Pic_test
////功能：    图片显示测试，依次显示三幅40X40 QQ图像
////输入参数：无
////返回值：  无
////修改记录：
////******************************************************************
//void Pic_test(void)
//{
//    DrawTestPage("测试6:图片显示测试");
//    LCD_Fill(0,20,lcddev.width,lcddev.height-20,WHITE);
//    LCD_ShowPicture(30,30,40,40,gImage_1);
//    Show_Str(30+12,75,"QQ",BLUE,YELLOW,16,1);
//    LCD_ShowPicture(90,30,40,40,gImage_1);
//    Show_Str(90+12,75,"QQ",BLUE,YELLOW,16,1);
//    LCD_ShowPicture(150,30,40,40,gImage_1);
//    Show_Str(150+12,75,"QQ",BLUE,YELLOW,16,1);
//    delay_ms(1200);
//}
//
////******************************************************************
////函数名：  Touch_Test
////功能：    触摸手写测试
////输入参数：无
////返回值：  无
////修改记录：
////******************************************************************
//void Touch_Test(void)
//{
//    rt_uint16_t lastpos[2];//最后一次的数据
//    lastpos[0]=0xffff;
//    TP_Init();//触摸初始化
//    LCD_Clear(WHITE);
//    DrawTestPage("测试7:Touch测试");
//    while(1)
//    {
//        tp_dev.scan(0);//扫描
//        if(tp_dev.sta&TP_PRES_DOWN)//有按键被按下
//        {
//            delay_ms(1);//必要的延时,否则老认为有按键按下.
//            if(tp_dev.x[0]<lcddev.width&&tp_dev.y[0]<lcddev.height)
//            {
//                if(lastpos[0]==0XFFFF)
//                {
//                    lastpos[0]=tp_dev.x[0];
//                    lastpos[1]=tp_dev.y[0];
//                }
//                LCD_DrawRoughLine(lastpos[0],lastpos[1],tp_dev.x[0],tp_dev.y[0],RED);//画线
//                lastpos[0]=tp_dev.x[0];
//                lastpos[1]=tp_dev.y[0];
//                LCD_ShowString(10,lcddev.height-60,"X:",RED,BLACK,16,0);
//                LCD_ShowIntNum(26,lcddev.height-60,tp_dev.x[0],3,RED,BLACK,16);
//                LCD_ShowString(10,lcddev.height-40,"Y:",RED,BLACK,16,0);
//                LCD_ShowIntNum(26,lcddev.height-40,tp_dev.y[0],3,RED,BLACK,16);
//            }
//        }
//  }
//}
