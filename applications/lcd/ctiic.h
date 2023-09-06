#ifndef __MYCT_IIC_H
#define __MYCT_IIC_H
#include <rtthread.h>
#include <rtdevice.h>
#include <stdlib.h>
#include "pin_config.h"

#define TOUCH_I2C_BUS_NAME     "i2c2"   /* 传感器连接的I2C总线设备名称 */

#define GT911_SCCB_ADDR 0xBA   //GT911设备总线地址1
//#define GT911_SCCB_ADDR 0x28   //GT911设备总线地址2

//IIC所有操作函数
void GT911_I2C_Reponse(void);
rt_err_t GT911_I2C_Write_Reg(rt_uint16_t reg, rt_uint8_t len, rt_uint8_t *buf);    //写 寄存器地址 数据长度 数据
rt_err_t GT911_I2C_Read_Reg(rt_uint16_t reg, rt_uint8_t len, rt_uint8_t *buf);     //读 寄存器地址 数据长度 数据
rt_err_t GT911_I2C_Write_Reg_1data(rt_uint16_t reg, rt_uint8_t buf);               //写 寄存器地址 数据
rt_err_t GT911_I2C_Read_Reg_1data(rt_uint16_t reg, rt_uint8_t *buf);               //读 寄存器地址 数据

#endif
