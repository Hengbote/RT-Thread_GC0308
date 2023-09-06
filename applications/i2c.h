#ifndef APPLICATIONS_I2C_H_
#define APPLICATIONS_I2C_H_

#include "pin_config.h"
#include "main.h"
#ifndef USE_BARE

#define CAMERA_I2C_BUS_NAME     "i2c1"  /* 相机连接的I2C总线设备名称 */
#define CAMERA_UART_NAME        "uart2" /* 相机连接的串口设备名称 */

typedef struct
{
    rt_mutex_t lock;                    //互斥量
    rt_device_t uart;                   //摄像头图像输出串口
    struct rt_i2c_bus_device *i2c;      //挂载的I2C总线
    DCMI_HandleTypeDef *dcmi;           //DCMI设备结构体地址
}Camera_Structure;

rt_err_t GC0308_I2C_Set_Reg_Bits(rt_uint8_t reg, rt_uint8_t offset, rt_uint8_t mask, rt_uint8_t value);  //设置寄存器值
rt_err_t GC0308_I2C_Write_Regs(const rt_uint8_t regs[][2], size_t regs_size);      //写多个命令
rt_err_t GC0308_I2C_Write_Reg(rt_uint8_t reg, rt_uint8_t len, rt_uint8_t *buf);    //写 寄存器地址 数据长度 数据
rt_err_t GC0308_I2C_Read_Reg(rt_uint8_t reg, rt_uint8_t len, rt_uint8_t *buf);     //读 寄存器地址 数据长度 数据
rt_err_t GC0308_I2C_Write_Reg_1data(rt_uint8_t reg, rt_uint8_t buf);               //写 寄存器地址 数据
rt_err_t GC0308_I2C_Read_Reg_1data(rt_uint8_t reg, rt_uint8_t *buf);               //读 寄存器地址 数据

#endif

#endif /* APPLICATIONS_I2C_H_ */
