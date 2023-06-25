/**
 ******************************************************************************
 * @file    fp_vision_preproc.h
 * @author  MCD Application Team
 * @brief   Header for fp_vision_preproc.c module
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
#ifndef __FP_VISION_PREPROC_H
#define __FP_VISION_PREPROC_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "fp_vision_global.h"
#include "stm32ipl.h"

/* Exported types ------------------------------------------------------------*/
typedef struct
{
uint16_t x;                 //这个字段可能表示图像或图像块的宽度
uint16_t y;                 //这个字段可能表示图像或图像块的高度
uint32_t rowStride;         //这个字段表示图像的行跨度（row stride）。行跨度是指在内存中从一行的开始到下一行的开始之间的字节距离。它通常等于或大于图像的宽度乘以每个像素的字节数
}Dma2dCfg_TypeDef;

typedef struct
{
 Dma2dCfg_TypeDef Dma2dcfg; //DMA2D(直接内存访问2D)硬件配置信息。DMA2D是一个可以执行2D图像块传输和操作的硬件单元。
 uint32_t red_blue_swap;    //表示是否需要交换红蓝通道的颜色
 image_t Pfc_Src_Img;       //表示需要进行像素格式转换（Pixel Format Conversion，PFC）的源图像(转换前)
 image_t Pfc_Dst_Img;       //表示像素格式转换（Pixel Format Conversion，PFC）的目标图像(转换后)
 image_t Resize_Src_Img;    //表示需要进行尺寸调整的源图像(转换前)
 image_t Resize_Dst_Img;    //表示尺寸调整的目标图像(转换后)
 void*    AppCtxPtr;        //这是一个指向应用程序上下文的指针
}PreprocContext_TypeDef;    //预处理上下文结构体,用于存储与图像预处理操作有关的状态和信息


/* Exported constants --------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
extern PreprocContext_TypeDef Preproc_Context;

/*******************/
/****PFC defines****/
/*******************/
/*The Pixel Format Conversion method, PIXEL_FMT_CONV, is configured in the preprocessor project's option:
* 1: HW_PFC : PFC performed by mean of HW engine like DMA2D
* 2: SW_PFC : PFC is performed by mean of SW routine and LUT
* 像素格式转换方法，PIXEL_FMT_CONV，在预处理器项目的选项中配置:
* 1: HW_PFC:通过DMA2D等HW引擎执行的PFC
* 2: SW_PFC:通过SW例程和LUT进行PFC
*/
#define HW_PFC 1
#define SW_PFC 2

#ifndef PIXEL_FMT_CONV      //指定图片的像素格式转换方法
 #define PIXEL_FMT_CONV 1   /*使用了但是不知道在那里添加的,目前只能加在这里*/
#endif

/* Exported functions ------------------------------------------------------- */
void PREPROC_ImageResize(PreprocContext_TypeDef*);
void PREPROC_PixelFormatConversion(PreprocContext_TypeDef*);
void PREPROC_Pixel_RB_Swap(void *, void *, uint32_t );
void PREPROC_Init(PreprocContext_TypeDef * );

#ifdef __cplusplus
}
#endif

#endif /*__FP_VISION_PREPROC_H */

