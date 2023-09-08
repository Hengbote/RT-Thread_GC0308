#include <rtthread.h>
#include <rtdevice.h>
#include "pin_config.h"
#include "main.h"
#ifdef USE_BARE
    #include "i2c_Bare.h"
#else
    #include "i2c.h"
#endif
#include "GC0308.h"
#include "bc28.h"
#ifdef USE_STM32IPL
    #include "fp_vision_app.h"
#else
    #include "image_recognition.h"
#endif
#include "lcd.h"
#include "touch.h"



#define DBG_TAG "main"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

int main(void)
{
    int count = 1;

//    AI_Reponse();           //AI推理
    LCD_Reponse();          //屏幕
//    Touch_Reponse();        //触摸屏
    GC0308_Reponse();       //摄像头
//    BC28_Reponse();         //4G模块
#ifdef USE_STM32IPL
    Frame_Reponse();
#endif
    while (count++)
    {
//        LOG_D("Hello");
        rt_thread_mdelay(1000);
    }

    return RT_EOK;
}
