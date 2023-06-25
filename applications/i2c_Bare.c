#include <rtthread.h>
#include <rtdevice.h>
#include "i2c_Bare.h"

#ifdef USE_BARE

#include "pin_config.h"
#include "drv_common.h"

#define DBG_TAG "i2c"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>


Camera_Structure camera_device_t;     //摄像头设备
rt_thread_t camera_response_t;      //摄像头任务结构体


//SCCB起始信号
//当时钟为高的时候,数据线的高到低,为SCCB起始信号
//在激活状态下,SDA和SCL均为低电平
void SCCB_Start(void)
{
    rt_pin_write(SDA_PIN, PIN_HIGH);    //数据线高电平
    rt_pin_write(SCL_PIN, PIN_HIGH);        //在时钟线高的时候数据线由高至低
    rt_hw_us_delay(50);
    rt_pin_write(SDA_PIN, PIN_LOW);
    rt_hw_us_delay(50);
    rt_pin_write(SCL_PIN, PIN_LOW);     //数据线恢复低电平，单操作函数必要
}

//SCCB停止信号
//当时钟为高的时候,数据线的低到高,为SCCB停止信号
//空闲状况下,SDA,SCL均为高电平
void SCCB_Stop(void)
{
    rt_pin_write(SDA_PIN, PIN_LOW);
    rt_hw_us_delay(50);
    rt_pin_write(SCL_PIN, PIN_HIGH);
    rt_hw_us_delay(50);
    rt_pin_write(SDA_PIN, PIN_HIGH);
    rt_hw_us_delay(50);
}
//产生NA信号
void SCCB_No_Ack(void)
{
    rt_hw_us_delay(50);
    rt_pin_write(SDA_PIN, PIN_HIGH);
    rt_pin_write(SCL_PIN, PIN_HIGH);
    rt_hw_us_delay(50);
    rt_pin_write(SCL_PIN, PIN_LOW);
    rt_hw_us_delay(50);
    rt_pin_write(SDA_PIN, PIN_LOW);
    rt_hw_us_delay(50);
}
//SCCB,写入一个字节
//返回值:0,成功;1,失败.
rt_uint8_t SCCB_WR_Byte(rt_uint8_t dat)
{
    rt_uint8_t j,res;
    for(j=0;j<8;j++) //循环8次发送数据
    {
        if(dat&0x80)rt_pin_write(SDA_PIN, PIN_HIGH);
        else rt_pin_write(SDA_PIN, PIN_LOW);
        dat<<=1;
        rt_hw_us_delay(50);
        rt_pin_write(SCL_PIN, PIN_HIGH);
        rt_hw_us_delay(50);
        rt_pin_write(SCL_PIN, PIN_LOW);
    }
    rt_pin_mode(SDA_PIN, PIN_MODE_INPUT);       //设置SDA为输入
    rt_hw_us_delay(50);
    rt_pin_write(SCL_PIN, PIN_HIGH);            //接收第九位,以判断是否发送成功
    rt_hw_us_delay(50);
    if(rt_pin_read(SDA_PIN))res=1;  //SDA=1发送失败，返回1
    else res=0;         //SDA=0发送成功，返回0
    rt_pin_write(SCL_PIN, PIN_LOW);
    rt_pin_mode(SDA_PIN, PIN_MODE_OUTPUT_OD);       //设置SDA为输出
    return res;
}
//SCCB 读取一个字节
//在SCL的上升沿,数据锁存
//返回值:读到的数据
rt_uint8_t SCCB_RD_Byte(void)
{
    rt_uint8_t temp=0,j;
    rt_pin_mode(SDA_PIN, PIN_MODE_INPUT);       //设置SDA为输入
    for(j=8;j>0;j--)    //循环8次接收数据
    {
        rt_hw_us_delay(50);
        rt_pin_write(SCL_PIN, PIN_HIGH);
        temp=temp<<1;
        if(rt_pin_read(SDA_PIN))temp++;
        rt_hw_us_delay(50);
        rt_pin_write(SCL_PIN, PIN_LOW);
    }
    rt_pin_mode(SDA_PIN, PIN_MODE_OUTPUT_OD);       //设置SDA为输出
    return temp;
}
//写寄存器
//返回值:0,成功;1,失败.
rt_uint8_t SCCB_WR_Reg(rt_uint8_t reg,rt_uint8_t data)
{
    rt_uint8_t res=0;
    SCCB_Start();                   //启动SCCB传输
    if(SCCB_WR_Byte(OV2640_BUS_ADDR))res=1; //写器件ID
    rt_hw_us_delay(100);
    if(SCCB_WR_Byte(reg))res=1;     //写寄存器地址
    rt_hw_us_delay(100);
    if(SCCB_WR_Byte(data))res=1;    //写数据
    SCCB_Stop();
    return  res;
}
//读寄存器
//返回值:读到的寄存器值
rt_uint8_t SCCB_RD_Reg(rt_uint8_t reg)
{
    rt_uint8_t val=0;
    SCCB_Start();               //启动SCCB传输
    SCCB_WR_Byte(OV2640_BUS_ADDR);      //写器件ID
    rt_hw_us_delay(100);
    SCCB_WR_Byte(reg);          //写寄存器地址
    rt_hw_us_delay(100);
    SCCB_Stop();
    rt_hw_us_delay(100);
    //设置寄存器地址后，才是读
    SCCB_Start();
    SCCB_WR_Byte(OV2640_BUS_ADDR|0X01); //发送读命令
    rt_hw_us_delay(100);
    val=SCCB_RD_Byte();         //读取数据
    SCCB_No_Ack();
    SCCB_Stop();
    return val;
}

void I2C_Reponse_Callback(void *parameter)
{
    rt_uint8_t temp;

    rt_pin_mode(SCL_PIN, PIN_MODE_OUTPUT_OD);
    rt_pin_mode(SDA_PIN, PIN_MODE_OUTPUT_OD);

    camera_device_t.lock    = rt_mutex_create("mutex_camera", RT_IPC_FLAG_FIFO);                    //创建摄像头互斥锁
    if(camera_device_t.lock == RT_NULL)
        LOG_E("Cannot create mutex for camera device on '%s'\r\n", CAMERA_I2C_BUS_NAME);

    while(1)
    {
        if(rt_mutex_take(camera_device_t.lock, RT_WAITING_FOREVER) != RT_EOK)       //上锁
            LOG_E("Failed to obtain the mutex\r\n");
        temp = SCCB_RD_Reg(0x00);
        rt_mutex_release(camera_device_t.lock);                                     //解锁

        rt_kprintf("temp = %x\r\n", temp);
        rt_thread_mdelay(1000);
    }
}

void I2C_Reponse(void)
{
    camera_response_t = rt_thread_create("camera_response_t", I2C_Reponse_Callback, RT_NULL, 2048, 10, 10);
    if(camera_response_t!=RT_NULL)rt_thread_startup(camera_response_t);
    LOG_I("I2C_Reponse Init Success\r\n");
}

#endif  /*USE_BARE*/
