/**
 ******************************************************************************
 * @file    fp_vision_global.h
 * @author  MCD Application Team
 * @brief   Header contianing global define used across different modules
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2019 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FP_VISION_GLOBAL_H
#define __FP_VISION_GLOBAL_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
//#include "stm32h747i_discovery.h"
#include "ai_interface.h"

#define FULL_EXTERNAL 1             /*完全外部(如SD卡)*/
#define SPLIT_INT_EXT 2             /*内部与外部存储的混合使用*/
#define FULL_INTERNAL_FPS_OPT 3     /*完全内部的，并且优化了内存使用*/
#define FULL_INTERNAL_MEM_OPT 4     /*完全内部的，并且优化了帧率*/


//#define NN_OUTPUT_CLASS_LIST output_labels
#define NN_TOP_N_DISPLAY 1
#define NN_TOP_N_DISPLAY_STRING_1 "Top-"
#define NN_TOP_N_DISPLAY_STRING_2 xstr(NN_TOP_N_DISPLAY)
#define NN_TOP_N_DISPLAY_MSG NN_TOP_N_DISPLAY_STRING_1 NN_TOP_N_DISPLAY_STRING_2
#define NN_OUTPUT_DISPLAY_REFRESH_RATE 1

#define NN_OUTPUT_CLASS_NUMBER AI_NET_OUTPUT_SIZE

typedef enum
{
  NOMINAL              = 0x01,    /* Nominal  */                //正常操作模式
  VALID                = 0x02,    /* Validation  */             //验证模式
  DUMP                 = 0x03,    /* Memory buffer dumping  */  //应用程序的内存缓冲区转储模式
  CAPTURE              = 0x04     /* Frame capture  */          //应用程序的帧捕获模式
}AppOperatingMode_TypeDef;      //表示应用程序的操作模式

typedef enum
{
  FRAME_CAPTURE    = 0x00,      //表示捕获帧的操作
  FRAME_RESIZE     = 0x01,      //表示调整帧大小的操作
  FRAME_PFC        = 0x02,      //
  FRAME_PVC        = 0x03,      //
  FRAME_INFERENCE  = 0x04,      //表示推理帧的操作

  APP_FRAMEOPERATION_NUM        //这个枚举成员可能用于记录枚举中的成员数量。当添加新的帧操作时，这个值将自动更新
} AppFrameOperation_TypeDef;    //用来描述应用程序中进行的不同帧操作

typedef enum
{
  CAM_BUFF        = 0x00,
  PFC_OUT_BUFF    = 0x01,
  RESIZE_OUT_BUFF = 0x02,
  NN_INPUT_BUFF   = 0x03,
  NN_OUTPUT_BUFF  = 0x04,
  
  APP_BUFF_NUM
} AppBuffer_TypeDef;

extern void Error_Handler(void);

#ifdef __cplusplus
}
#endif

#endif /*__FP_VISION_GLOBAL_H */

