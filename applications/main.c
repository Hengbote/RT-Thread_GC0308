#include <rtthread.h>
#include <rtdevice.h>
#include "pin_config.h"
#include "main.h"
#if bare
    #include "i2c_Bare.h"
#else
    #include "i2c.h"
#endif
#include "GC0308.h"
#include "image_recognition.h"


#define DBG_TAG "main"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

int main(void)
{
    int count = 1;

//    I2C_Reponse();
    GC0308_Reponse();
    while (count++)
    {
        rt_thread_mdelay(1000);
    }

    return RT_EOK;
}
