/**
 ******************************************************************************
 * @file    fp_vision_utils.h
 * @author  MCD Application Team
 * @brief   Header for fp_vision_utils.c module
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
#ifndef __FP_VISION_UTILS_H
#define __FP_VISION_UTILS_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "fp_vision_global.h"
//#include "camera.h"

typedef enum
{
  INVALIDATE           = 0x01,
  CLEAN                = 0x02
}DCache_Coherency_TypeDef;

//typedef struct
//{
//  uint32_t operation_exec_time[APP_FRAMEOPERATION_NUM]; //存储每种帧操作的执行时间
//  uint32_t Tfps;            //记录帧率
//  uint32_t tcapturestart1;  //存储捕获操作开始的时间
//  uint32_t tcapturestart2;  //
//  uint32_t tcapturestart;   //
//  uint32_t tcapturestop;    //记录帧捕获的结束时间
//}ExecTimingContext_TypeDef; //桢操作执行时间的度量和统计信息

//typedef struct
//{
//  JOYPin_TypeDef joyState;  //表示遥感(操纵杆)的状态
//  int32_t camContrast;   /* Camera brightness. */   //表示相机的对比度
//  int32_t camBrightness;/* Camera contrast. */      //表示相机的亮度
//  int32_t lcdBrightness; /* LCD brightness. */      //表示液晶显示器（LCD）的亮度
//} JoystickContext_TypeDef;  //与遥感(操纵杆)和各种亮度设置相关的统计信息

//typedef struct
//{
//  ExecTimingContext_TypeDef ExecTimingContext;  //桢操作执行时间的度量和统计信息
//  JoystickContext_TypeDef   JoystickContext;    //与遥感(操纵杆)和各种亮度设置相关的统计信息
//  void* AppCtxPtr;      //这是一个指向应用程序上下文的指针，这个上下文可能包含了应用程序的其他信息
//} UtilsContext_TypeDef; //桢操作执行时间 遥感(操纵杆)信息 相机和lcd亮度设置

/* Exported constants --------------------------------------------------------*/
#include "fp_vision_app.h"

/* External variables --------------------------------------------------------*/
//extern UtilsContext_TypeDef UtilsContext;

/* Exported functions ------------------------------------------------------- */
//void UTILS_Init(UtilsContext_TypeDef *);
//void UTILS_Joystick_Check(UtilsContext_TypeDef *);
void UTILS_Dma2d_Memcpy(uint32_t *pSrc, uint32_t *pDst, uint16_t x, uint16_t y, uint16_t xsize, uint16_t ysize,
                     uint32_t rowStride, uint32_t input_color_format, uint32_t output_color_format, int pfc,
                     int red_blue_swap);
void UTILS_DCache_Coherency_Maintenance(uint32_t *, int32_t , DCache_Coherency_TypeDef );
void UTILS_Bubblesort(float *, int *, int );
//void UTILS_Compute_ExecutionTiming(UtilsContext_TypeDef *);
//uint32_t UTILS_GetTimeStamp(UtilsContext_TypeDef *);

#ifdef __cplusplus
}
#endif

#endif /*__FP_VISION_UTILS_H */

