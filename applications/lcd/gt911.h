#ifndef __GT911_H
#define __GT911_H
#include "ctiic.h"
#include "touch.h"

//IO��������
#define GT_RST          PCout(7)    //GT911��λ����
#define GT_INT          PAin(9)     //GT911�ж�����


//GT911 ���ּĴ�������
#define GT_CTRL_REG     0X8040      //GT911���ƼĴ���
#define GT_CFGS_REG     0X8047      //GT911������ʼ��ַ�Ĵ���
#define GT_CHECK_REG    0X80FF      //GT911У��ͼĴ���
#define GT_PID_REG      0X8140      //GT911��ƷID�Ĵ���

#define GT_PID_REG0     0X8140      //GT911��ƷID�Ĵ���
#define GT_PID_REG1     0X8141      //GT911��ƷID�Ĵ���
#define GT_PID_REG2     0X8142      //GT911��ƷID�Ĵ���
#define GT_PID_REG3     0X8143      //GT911��ƷID�Ĵ���


#define GT_GSTID_REG    0X814E      //GT911��ǰ��⵽�Ĵ������
#define GT_TP1_REG      0X8150      //��һ�����������ݵ�ַ
#define GT_TP2_REG      0X8158      //�ڶ������������ݵ�ַ
#define GT_TP3_REG      0X8160      //���������������ݵ�ַ
#define GT_TP4_REG      0X8168      //���ĸ����������ݵ�ַ
#define GT_TP5_REG      0X8170      //��������������ݵ�ַ


rt_uint8_t GT911_Send_Cfg(rt_uint8_t mode);
rt_uint8_t GT911_Init(void);
rt_uint8_t GT911_Scan(rt_uint8_t mode);
#endif
