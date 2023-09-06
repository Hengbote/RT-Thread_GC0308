#include "lcd.h"
#include "GT911.h"

#define DBG_TAG "GT911"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

extern struct rt_i2c_bus_device *gt911_i2c;    //触摸模块的I2C总线


//GT911配置参数表
//第一个字节为版本号(0X68),必须保证新的版本号大于等于GT911内部
//flash原有版本号,才会更新配置.
const rt_uint8_t GT911_CFG_TBL[]=
{
    0x00,0x20,0x03,0xe0,0x01,0x05,0x3D,0x00,0x01,0x48,
    0x28,0x0D,0x50,0x32,0x03,0x05,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x18,0x1A,0x1E,0x14,0x8A,0x2A,0x0C,
    0x30,0x38,0x31,0x0D,0x00,0x00,0x02,0xB9,0x03,0x2D,
    0x00,0x00,0x00,0x00,0x00,0x03,0x64,0x32,0x00,0x00,
    0x00,0x1D,0x41,0x94,0xC5,0x02,0x07,0x00,0x00,0x04,
    0xA5,0x1F,0x00,0x94,0x25,0x00,0x88,0x2B,0x00,0x7D,
    0x33,0x00,0x74,0x3C,0x00,0x74,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x18,0x16,0x14,0x12,0x10,0x0E,0x0C,0x0A,
    0x08,0x06,0x04,0x02,0xFF,0xFF,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x24,0x22,0x21,0x20,0x1F,0x1E,0x1D,0x1C,
    0x18,0x16,0x13,0x12,0x10,0x0F,0x0A,0x08,0x06,0x04,
    0x02,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,

};




//发送GT911配置参数
//mode:0,参数不保存到flash
//     1,参数保存到flash
rt_uint8_t GT911_Send_Cfg(rt_uint8_t mode)
{
    rt_uint8_t buf[2];
    rt_uint8_t i=0;
    buf[0]=0;
    buf[1]=mode;    //是否写入到GT911 FLASH?  即是否掉电保存
    for(i=0;i<sizeof(GT911_CFG_TBL);i++)
    {
        buf[0]+=GT911_CFG_TBL[i];//计算校验和
    }
    buf[0]=(~buf[0])+1;
    GT911_I2C_Write_Reg(GT_CFGS_REG,sizeof(GT911_CFG_TBL),(rt_uint8_t*)GT911_CFG_TBL);//发送寄存器配置
    GT911_I2C_Write_Reg(GT_CHECK_REG,2,buf);//写入校验和,和配置更新标记
    return 0;
}

//初始化GT911触摸屏
//返回值:0,初始化成功;1,初始化失败
rt_uint8_t GT911_Init(void)
{
    rt_uint8_t temp[5]={0};

    //配置INT 推挽输出
    rt_pin_mode(GT911_INT, PIN_MODE_OUTPUT);
    //配置RST 推挽输出
    rt_pin_mode(GT911_RST, PIN_MODE_OUTPUT);

    /* 查找I2C总线设备，获取I2C总线设备句柄 */
    gt911_i2c     = (struct rt_i2c_bus_device *)rt_device_find(TOUCH_I2C_BUS_NAME);      //寻找触摸模块IIC总线设备
    if(gt911_i2c  == RT_NULL)
        LOG_E("Cannot find gt911 device on '%s'", TOUCH_I2C_BUS_NAME);

    rt_pin_write(GT911_RST, PIN_LOW);
//    rt_pin_write(GT911_INT, PIN_LOW);
    rt_thread_mdelay(10);
    rt_pin_write(GT911_RST, PIN_HIGH);
    rt_thread_mdelay(10);
    //配置INT  浮空输入
    rt_pin_mode(GT911_INT, PIN_MODE_INPUT);
    rt_thread_mdelay(100);

    GT911_I2C_Read_Reg(GT_PID_REG,4,temp);//读取产品ID
    LOG_I("触摸芯片型号为%s", temp);

    temp[4]=0;
    if(strcmp((char*)temp,"911")==0)//ID==911
    {
        temp[0]=0X02;
        GT911_I2C_Write_Reg(GT_CTRL_REG,1,temp);//软复位GT911
        GT911_I2C_Read_Reg(GT_CFGS_REG,1,temp);//读取GT_CFGS_REG寄存器
//      if(temp[0]!=0X5D)//默认版本比较低,需要更新flash配置
//      {
            GT911_Send_Cfg(1);//更新并保存配置
//      }
        rt_thread_mdelay(100);
        temp[0]=0X00;
        GT911_I2C_Write_Reg(GT_CTRL_REG,1,temp);//结束复位
        return 0;
    }
    return 1;
}
const rt_uint16_t GT911_TPX_TBL[5]={GT_TP1_REG,GT_TP2_REG,GT_TP3_REG,GT_TP4_REG,GT_TP5_REG};
//扫描触摸屏(采用查询方式)
//mode:0,正常扫描.
//返回值:当前触屏状态.
//0,触屏无触摸;1,触屏有触摸
rt_uint8_t GT911_Scan(rt_uint8_t mode)
{
    rt_uint8_t buf[4];
//    rt_uint8_t printf_buffer[50];
    rt_uint8_t i=0;
    rt_uint8_t res=0;
    rt_uint8_t temp;
    rt_uint8_t tempsta;
    static rt_uint8_t t=0;//控制查询间隔,从而降低CPU占用率
    t++;
    if((t%10)==0||t<10)//空闲时,每进入10次CTP_Scan函数才检测1次,从而节省CPU使用率
    {
        GT911_I2C_Read_Reg(GT_GSTID_REG,1,&mode); //读取触摸点的状态
        if(mode&0X80&&((mode&0XF)<6))
        {
            temp=0;
            GT911_I2C_Write_Reg(GT_GSTID_REG,1,&temp);//清标志
        }
        if((mode&0XF)&&((mode&0XF)<6))
        {
            temp=0XFF<<(mode&0XF);      //将点的个数转换为1的位数,匹配tp_dev.sta定义
            tempsta=tp_dev.sta;         //保存当前的tp_dev.sta值
            tp_dev.sta=(~temp)|TP_PRES_DOWN|TP_CATH_PRES;
            tp_dev.x[4]=tp_dev.x[0];    //保存触点0的数据
            tp_dev.y[4]=tp_dev.y[0];
            for(i=0;i<5;i++)
            {
                if(tp_dev.sta&(1<<i))   //触摸有效?
                {
                    GT911_I2C_Read_Reg(GT911_TPX_TBL[i],4,buf);   //读取XY坐标值
                    if(tp_dev.touchtype&0X01)//横屏
                    {
                        tp_dev.y[i]=((rt_uint16_t)buf[1]<<8)+buf[0];
                        tp_dev.x[i]=800-(((rt_uint16_t)buf[3]<<8)+buf[2]);
                    }else
                    {
                        tp_dev.x[i]=((rt_uint16_t)buf[1]<<8)+buf[0];
                        tp_dev.y[i]=((rt_uint16_t)buf[3]<<8)+buf[2];
                    }
                    LOG_I("x[%d]:%03d,y[%d]:%03d",i,tp_dev.x[i],i,tp_dev.y[i]);
//                    ESP8266SendData((char*)printf_buffer);//用串口助手，时间戳观察 延时时间是否正确
                }
            }
            res=1;
            if(tp_dev.x[0]>lcddev.width||tp_dev.y[0]>lcddev.height)//非法数据(坐标超出了)
            {
                if((mode&0XF)>1)        //有其他点有数据,则复第二个触点的数据到第一个触点.
                {
                    tp_dev.x[0]=tp_dev.x[1];
                    tp_dev.y[0]=tp_dev.y[1];
                    t=0;                //触发一次,则会最少连续监测10次,从而提高命中率
                }else                   //非法数据,则忽略此次数据(还原原来的)
                {
                    tp_dev.x[0]=tp_dev.x[4];
                    tp_dev.y[0]=tp_dev.y[4];
                    mode=0X80;
                    tp_dev.sta=tempsta; //恢复tp_dev.sta
                }
            }else t=0;                  //触发一次,则会最少连续监测10次,从而提高命中率
        }
    }
    if((mode&0X8F)==0X80)//无触摸点按下
    {
        if(tp_dev.sta&TP_PRES_DOWN) //之前是被按下的
        {
            tp_dev.sta&=~(1<<7);    //标记按键松开
        }else                       //之前就没有被按下
        {
            tp_dev.x[0]=0xffff;
            tp_dev.y[0]=0xffff;
            tp_dev.sta&=0XE0;   //清除点有效标记
        }
    }
    if(t>240)t=10;//重新从10开始计数
    return res;
}
