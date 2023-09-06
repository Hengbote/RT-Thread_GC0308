#ifndef __GT911_H
#define __GT911_H
#include "ctiic.h"
#include "touch.h"

//IO操作函数
#define GT_RST          PCout(7)    //GT911复位引脚
#define GT_INT          PAin(9)     //GT911中断引脚


//GT911 部分寄存器定义
#define GT_CTRL_REG     0X8040      //GT911控制寄存器
#define GT_CFGS_REG     0X8047      //GT911配置起始地址寄存器
#define GT_CHECK_REG    0X80FF      //GT911校验和寄存器
#define GT_PID_REG      0X8140      //GT911产品ID寄存器

#define GT_PID_REG0     0X8140      //GT911产品ID寄存器
#define GT_PID_REG1     0X8141      //GT911产品ID寄存器
#define GT_PID_REG2     0X8142      //GT911产品ID寄存器
#define GT_PID_REG3     0X8143      //GT911产品ID寄存器


#define GT_GSTID_REG    0X814E      //GT911当前检测到的触摸情况
#define GT_TP1_REG      0X8150      //第一个触摸点数据地址
#define GT_TP2_REG      0X8158      //第二个触摸点数据地址
#define GT_TP3_REG      0X8160      //第三个触摸点数据地址
#define GT_TP4_REG      0X8168      //第四个触摸点数据地址
#define GT_TP5_REG      0X8170      //第五个触摸点数据地址


rt_uint8_t GT911_Send_Cfg(rt_uint8_t mode);
rt_uint8_t GT911_Init(void);
rt_uint8_t GT911_Scan(rt_uint8_t mode);
#endif
