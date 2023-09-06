#include "ctiic.h"

#define DBG_TAG "ctiic"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

struct rt_i2c_bus_device *gt911_i2c;    //触摸模块的I2C总线
rt_thread_t gt911_i2c_response_t;       //触摸模块任务结构体

//向GT911写入一次数据
//reg:起始寄存器地址
//len:写数据长度
//buf:数据缓缓存区
rt_err_t GT911_I2C_Write_Reg(rt_uint16_t reg, rt_uint8_t len, rt_uint8_t *buf)      //写 寄存器地址 数据长度 数据
{
    rt_err_t ret = -RT_ERROR;
    struct rt_i2c_msg msgs[2];
    rt_uint8_t reg_[2] = {reg>>8, reg&0XFF};

    do
    {
        //设备地址 -- 寄存器号
        msgs[0].addr    = GT911_SCCB_ADDR>>1;
//        msgs[0].flags = RT_I2C_WR | RT_I2C_NO_STOP;
        msgs[0].flags   = RT_I2C_WR;
        msgs[0].buf     = reg_;
        msgs[0].len     = 2;
        //打算往寄存器里写什么
        msgs[1].addr    = GT911_SCCB_ADDR>>1;
        msgs[1].flags   = RT_I2C_WR | RT_I2C_NO_START;
        msgs[1].buf     = buf;
        msgs[1].len     = len;

        ret = rt_i2c_transfer(gt911_i2c, msgs, 2);

        if(ret != 2)
            rt_thread_mdelay(1);
//        {
//            rt_thread_mdelay(1);
//            LOG_E("写1位      失败  '%ld'   寄存器'%02x' 数据'%02x' ", ret, reg, *buf);
//        }
//        else
//            LOG_I("写1位      成功  '%ld'  寄存器'%02x' 数据'%02x' ", ret, reg, *buf);
    }while(ret != 2);

    return RT_EOK;
}

rt_err_t GT911_I2C_Write_Reg_1data(rt_uint16_t reg, rt_uint8_t buf)      //写 寄存器地址 数据
{
    return GT911_I2C_Write_Reg(reg, 1, &buf);
}

//从GT911读出一次数据
//reg:起始寄存器地址
//len:读数据长度
//buf:数据缓缓存区
rt_err_t GT911_I2C_Read_Reg(rt_uint16_t reg, rt_uint8_t len, rt_uint8_t *buf)       //读 寄存器地址 数据长度 数据
{
    rt_err_t ret = -RT_ERROR;
    struct rt_i2c_msg msgs[2];
    rt_uint8_t reg_[2] = {reg>>8, reg&0XFF};

    do
    {

        msgs[0].addr    = GT911_SCCB_ADDR>>1;
        msgs[0].flags   = RT_I2C_WR;
        msgs[0].buf     = reg_;
        msgs[0].len     = 2;

        msgs[1].addr    = GT911_SCCB_ADDR>>1;
        msgs[1].flags   = RT_I2C_RD;
        msgs[1].buf     = buf;
        msgs[1].len     = len;

        ret = rt_i2c_transfer(gt911_i2c, msgs, 2);

        if(ret != 2)
            rt_thread_mdelay(1);
//        {
//            LOG_E("读1位      失败       '%ld'   寄存器'%02x' 数据'%02x' ", ret, reg, *buf);
//            rt_thread_mdelay(1);
//        }
//        else
//            LOG_I("读1位      成功       '%ld'   寄存器'%02x' 数据'%02x' ", ret, reg, *buf);
    }while(ret != 2);

    return RT_EOK;
}

rt_err_t GT911_I2C_Read_Reg_1data(rt_uint16_t reg, rt_uint8_t *buf)       //读 寄存器地址 数据
{
    return GT911_I2C_Read_Reg(reg, 1, buf);
}

void GT911_I2C_Reponse_Callback(void *parameter)
{
//    rt_uint8_t temp;

    /* 查找I2C总线设备，获取I2C总线设备句柄 */
    gt911_i2c     = (struct rt_i2c_bus_device *)rt_device_find(TOUCH_I2C_BUS_NAME);      //寻找触摸模块IIC总线设备
    if(gt911_i2c  == RT_NULL)
        LOG_E("Cannot find gt911 device on '%s'", TOUCH_I2C_BUS_NAME);

    while(1)
    {
//        rt_kprintf("temp = %x\r\n", temp);
        rt_thread_mdelay(1000);
    }
}

void GT911_I2C_Reponse(void)
{
    gt911_i2c_response_t = rt_thread_create("gt911_i2c_response_t", GT911_I2C_Reponse_Callback, RT_NULL, 2048, 10, 10);
    if(gt911_i2c_response_t!=RT_NULL)rt_thread_startup(gt911_i2c_response_t);
        LOG_I("GT911_I2C_Reponse Init Success\r\n");
}


void GT911_I2C_Write_Register(int argc, rt_uint8_t *argv[])
{
    rt_uint8_t reg_addr = strtoul((const char *)argv[1], NULL, 16);// 从十六进制字符串转换为整数
    rt_uint8_t buf = strtoul((const char *)argv[2], NULL, 16);// 从十六进制字符串转换为整数

    if(3 != argc)
        rt_kprintf("Write Register Command format:<GT911_I2C_Write_Register <reg_addr> <data>>");
    else
        GT911_I2C_Write_Reg_1data(reg_addr, buf);
}


void GT911_I2C_Read_Register(int argc, rt_uint8_t *argv[])
{
    rt_uint8_t buf;
    rt_uint8_t reg_addr = strtoul((const char *)argv[1], NULL, 16);// 从十六进制字符串转换为整数

    if(2 != argc)
        rt_kprintf("Read Register Command format:<GT911_I2C_Read_Register <reg_addr>>");
    else
        GT911_I2C_Read_Reg_1data(reg_addr, &buf);
}

/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(GT911_I2C_Write_Register, Write Register Command format:<GT911_I2C_Write_Register <reg_addr> <data>>);
MSH_CMD_EXPORT(GT911_I2C_Read_Register, Read Register Command format:<GT911_I2C_Read_Register <reg_addr>>);
