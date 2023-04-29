#include <rtthread.h>
#include <rtdevice.h>
#include <stdlib.h>
#include "pin_config.h"

#ifdef bare
#include "i2c.h"
#include "GC0308.h"

#define DBG_TAG "i2c"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>


Camera_Structure camera_device_t;     //摄像头设备
rt_thread_t i2c_response_t;      //摄像头任务结构体

rt_err_t write_reg(rt_uint8_t reg, rt_uint8_t len, rt_uint8_t *buf)      //写 寄存器地址 数据长度 数据
{
    rt_err_t ret = -RT_ERROR;
    struct rt_i2c_msg msgs[2];

    do
    {
        //设备地址 -- 寄存器号
        msgs[0].addr = GC0308_SCCB_ADDR>>1;
//        msgs[0].flags = RT_I2C_WR | RT_I2C_NO_STOP;
        msgs[0].flags = RT_I2C_WR;
        msgs[0].buf = &reg;
        msgs[0].len = 1;
        //打算往寄存器里写什么
        msgs[1].addr = GC0308_SCCB_ADDR>>1;
        msgs[1].flags = RT_I2C_WR | RT_I2C_NO_START;
        msgs[1].buf = buf;
        msgs[1].len = len;

        ret = rt_i2c_transfer(camera_device_t.i2c, msgs, 2);

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

rt_err_t write_reg_1data(rt_uint8_t reg, rt_uint8_t buf)      //写 寄存器地址 数据
{
    return write_reg(reg, 1, &buf);
}

rt_err_t write_regs(const rt_uint8_t regs[][2], size_t regs_size) //写多个命令
{
    int i = 0;
    rt_err_t ret = -RT_ERROR;
    struct rt_i2c_msg msgs;

    while (i < regs_size)
    {
        //寄存器号 数据
        msgs.addr = GC0308_SCCB_ADDR>>1;
        msgs.flags = RT_I2C_WR;
        msgs.buf = (rt_uint8_t *)&regs[i];
        msgs.len = 2;

        ret = rt_i2c_transfer(camera_device_t.i2c, &msgs, 1);

        if(ret != 1)
//            rt_thread_mdelay(1);
        {
            i--;
            LOG_E("写多位     失败       '%ld'    寄存器'%02x' 数据'%02x' ", ret, regs[i][0], regs[i][1]);
            rt_thread_mdelay(1);
        }
//        else
//            LOG_I("写多位     成功       '%ld'    寄存器'%02x' 数据'%02x' ", ret, regs[i][0], regs[i][1]);

//        if((regs[i][0] == 0xfe) && ret == 1)
//            LOG_I("写多位     成功       '%ld'    寄存器'%02x' 数据'%02x' ", ret, regs[i][0], regs[i][1]);

        i++;
    }
    return RT_EOK;
}

/*
 * *设置寄存器值
 * reg目标寄存器
 * offset移位值
 * mask掩码   表示只设置掩码的位置 清零
 * value输入值
 * */
rt_err_t set_reg_bits(rt_uint8_t reg, rt_uint8_t offset, rt_uint8_t mask, rt_uint8_t value)  //设置寄存器值
{
    rt_uint8_t c_value, new_value;
    struct rt_i2c_msg msgs[2];
    struct rt_i2c_msg msgs1[2];
    rt_err_t ret = -RT_ERROR;

    do
    {
        // 读取寄存器原来的值
        msgs1[0].addr = GC0308_SCCB_ADDR>>1;
        msgs1[0].flags = RT_I2C_WR;
        msgs1[0].buf = &reg;
        msgs1[0].len = 1;
        msgs1[1].addr = GC0308_SCCB_ADDR>>1;
        msgs1[1].flags = RT_I2C_RD;
        msgs1[1].buf = &c_value;
        msgs1[1].len = 1;
        // 使用 rt_i2c_transfer 进行 I2C 通信
        ret = rt_i2c_transfer(camera_device_t.i2c, msgs1, 2);

        if(ret != 2)
//            rt_thread_mdelay(1);
        {
            LOG_E("读寄存器   失败   '%ld'       寄存器'%02x' 数据'%02x' ", ret, reg, c_value);
            rt_thread_mdelay(1);
        }
//        else
//            LOG_I("读寄存器      成功   '%ld'   寄存器'%02x' 数据'%02x' ", ret, reg, c_value);

    }while(ret != 2);

    do
    {
        // 更新寄存器的值，只改变mask和offset指定的位，保留其他位的值
        new_value = (c_value & ~(mask << offset)) | ((value & mask) << offset);
        // 设备地址 -- 寄存器号
        msgs[0].addr = GC0308_SCCB_ADDR >> 1;
        msgs[0].flags = RT_I2C_WR;
        msgs[0].buf = &reg;
        msgs[0].len = 1;
        // 打算往寄存器里写什么
        msgs[1].addr = GC0308_SCCB_ADDR >> 1;
        msgs[1].flags = RT_I2C_WR | RT_I2C_NO_START;
        msgs[1].buf = &new_value;
        msgs[1].len = 1;
        // 使用 rt_i2c_transfer 进行 I2C 通信
        ret = rt_i2c_transfer(camera_device_t.i2c, msgs, 2);

        if(ret != 2)
        {
            LOG_E("写寄存器   失败      '%ld'   寄存器'%02x' 数据'%02x' ", ret, reg, new_value);
            rt_thread_mdelay(1);
        }
//            rt_thread_mdelay(1);
//        else
//            LOG_I("写寄存器      成功      '%ld'   寄存器'%02x' 数据'%02x' ", ret, reg, new_value);
    }while(ret != 2);

    return RT_EOK;
}

rt_err_t read_reg(rt_uint8_t reg, rt_uint8_t len, rt_uint8_t *buf)       //读 寄存器地址 数据长度 数据
{
    rt_err_t ret = -RT_ERROR;
    struct rt_i2c_msg msgs[2];

    do
    {
        msgs[0].addr = GC0308_SCCB_ADDR>>1;
        msgs[0].flags = RT_I2C_WR;
        msgs[0].buf = &reg;
        msgs[0].len = 1;

        msgs[1].addr = GC0308_SCCB_ADDR>>1;
        msgs[1].flags = RT_I2C_RD;
        msgs[1].buf = buf;
        msgs[1].len = len;

        ret = rt_i2c_transfer(camera_device_t.i2c, msgs, 2);

        if(ret != 2)
//            rt_thread_mdelay(1);
        {
            LOG_E("读1位      失败       '%ld'   寄存器'%02x' 数据'%02x' ", ret, reg, *buf);
            rt_thread_mdelay(1);
        }
//        else
//            LOG_I("读1位      成功       '%ld'   寄存器'%02x' 数据'%02x' ", ret, reg, *buf);
    }while(ret != 2);

    return RT_EOK;
}

rt_err_t read_reg_1data(rt_uint8_t reg, rt_uint8_t *buf)       //读 寄存器地址 数据
{
    return read_reg(reg, 1, buf);
}

void I2C_Reponse_Callback(void *parameter)
{
//    rt_uint8_t temp;

    /* 查找I2C总线设备，获取I2C总线设备句柄 */
    camera_device_t.i2c     = (struct rt_i2c_bus_device *)rt_device_find(CAMERA_I2C_BUS_NAME);      //寻找摄像头IIC总线设备
    if(camera_device_t.i2c  == RT_NULL)
        LOG_E("Cannot find camera device on '%s'", CAMERA_I2C_BUS_NAME);
    camera_device_t.lock    = rt_mutex_create("mutex_camera", RT_IPC_FLAG_FIFO);                    //创建摄像头互斥锁
    if(camera_device_t.lock == RT_NULL)
        LOG_E("Cannot create mutex for camera device on '%s'", CAMERA_I2C_BUS_NAME);

    while(1)
    {
        if(rt_mutex_take(camera_device_t.lock, RT_WAITING_FOREVER) != RT_EOK)       //上锁
            LOG_E("Failed to obtain the mutex\r\n");
//        write_reg();
//        read_reg(0x00, 1, &temp);
        rt_mutex_release(camera_device_t.lock);                                     //解锁

//        rt_kprintf("temp = %x\r\n", temp);
        rt_thread_mdelay(1000);
    }
}

//void I2C_Reponse(void)
//{
//    i2c_response_t = rt_thread_create("i2c_response_t", I2C_Reponse_Callback, RT_NULL, 2048, 10, 10);
//    if(i2c_response_t!=RT_NULL)rt_thread_startup(i2c_response_t);
//        LOG_I("I2C_Reponse Init Success\r\n");
//}


void Write_Register(int argc, rt_uint8_t *argv[])
{
    rt_uint8_t reg_addr = strtoul((const char *)argv[1], NULL, 16);// 从十六进制字符串转换为整数
    rt_uint8_t buf = strtoul((const char *)argv[2], NULL, 16);// 从十六进制字符串转换为整数

    if(3 != argc)
        rt_kprintf("Write Register Command format:<Write_Register <reg_addr> <data>>");
    else
        write_reg_1data(reg_addr, buf);
}


void Read_Register(int argc, rt_uint8_t *argv[])
{
    rt_uint8_t buf;
    rt_uint8_t reg_addr = strtoul((const char *)argv[1], NULL, 16);// 从十六进制字符串转换为整数

    if(2 != argc)
        rt_kprintf("Read Register Command format:<Read_Register <reg_addr>>");
    else
        read_reg_1data(reg_addr, &buf);
}

/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(Write_Register, Write Register Command format:<Write_Register <reg_addr> <data>>);
MSH_CMD_EXPORT(Read_Register, Read Register Command format:<Read_Register <reg_addr>>);

#endif
