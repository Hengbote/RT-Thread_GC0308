/**
 ******************************************************************************
 * @file    fp_vision_preproc.c
 * @author  MCD Application Team
 * @brief   FP VISION Pre-Processing configuration
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

/* Includes ------------------------------------------------------------------*/
#include "fp_vision_preproc.h"
#include "fp_vision_utils.h"
#include "ai_utilities.h"
#include <stdio.h>
#include <string.h>

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Global variables ----------------------------------------------------------*/
PreprocContext_TypeDef Preproc_Context;

/* Private function prototypes -----------------------------------------------*/
static void Preproc_Context_Init(PreprocContext_TypeDef *);

/* Functions Definition ------------------------------------------------------*/

/**
 * @brief Initializes the PreProcessing context structure
 * @param Preproc_Context_Ptr Pointer to PREPROC context
 */
static void Preproc_Context_Init(PreprocContext_TypeDef * Preproc_Context_Ptr)
{
  ;
}

/**
 * @brief PREPROC Init
 * @param Preproc_Context_Ptr Pointer to PREPROC context
 */
void PREPROC_Init(PreprocContext_TypeDef * Preproc_Context_Ptr)
{
  Preproc_Context_Init(Preproc_Context_Ptr);
}

/**
 * @brief Performs image (or selected Region Of Interest) resizing
 * @param Preproc_Context_Ptr Pointer to PREPROC context
 * 采用最近邻的方法 进行缩放
 */
void PREPROC_ImageResize(PreprocContext_TypeDef* Preproc_Context_Ptr)	//采用最近邻的方法 进行缩放
{
  image_t *src_img = &Preproc_Context_Ptr->Resize_Src_Img;				//表示需要进行尺寸调整的源图像(转换前)
  image_t *dst_img = &Preproc_Context_Ptr->Resize_Dst_Img;				//表示尺寸调整的目标图像(转换后)

  bool reverse = Image_CheckResizeMemoryLayout(src_img, dst_img);		//图像处理_判断内存中的布局是否需要进行反向处理

  if (STM32Ipl_Downscale(src_img, dst_img, reverse) != stm32ipl_err_Ok)	//采用最近邻的方法 进行缩放
  {
    while (1);
  }
}

/**
 * @brief Performs a pixel format conversion either by HW or SW
 * @param Preproc_Context_Ptr Pointer to PREPROC context
 * 通过硬件或软件执行像素格式转换
 */
void PREPROC_PixelFormatConversion(PreprocContext_TypeDef* Preproc_Context_Ptr)
{
  image_t *src_img = &Preproc_Context_Ptr->Pfc_Src_Img;
  image_t *dst_img = &Preproc_Context_Ptr->Pfc_Dst_Img;
  uint32_t rb_swap = Preproc_Context_Ptr->red_blue_swap;

#if PIXEL_FMT_CONV == HW_PFC
  if((src_img->bpp == IMAGE_BPP_RGB565) && (dst_img->bpp == IMAGE_BPP_RGB888))
  {
    /* DMA2D transfer with PFC */
    UTILS_Dma2d_Memcpy((uint32_t *)(src_img->data),
                       (uint32_t *)(dst_img->data),
                       0,
                       0,
                       src_img->w,
                       src_img->h,
                       dst_img->w,
                       DMA2D_INPUT_RGB565,
                       DMA2D_OUTPUT_RGB888,
                       1,
                       rb_swap);
  }
  else
  {
    while (1);
  }
#elif PIXEL_FMT_CONV == SW_PFC
  if (rb_swap != 0)
  {
    uint32_t nb_pixels = dst_img->w * dst_img->h;
    PREPROC_Pixel_RB_Swap(src_img->data, dst_img->data, nb_pixels);
  }

  bool reverse = Image_CheckResizeMemoryLayout(src_img, dst_img);

  if (STM32Ipl_ConvertRev(src_img, dst_img, reverse) != stm32ipl_err_Ok)
  {
    while (1);
  }
#else
 #error PFC method not valid!
#endif
}

/**
 * @brief  Performs pixel R & B swapping
 * @param  pSrc            Pointer to source buffer
 * @param  pDst            Pointer to destination buffer
 * @param  pixels          Number of pixels
 */
void PREPROC_Pixel_RB_Swap(void *pSrc, void *pDst, uint32_t pixels)
{
  struct rgb_Src
  {
    uint8_t r, g, b;
  };
  
  struct rgb_Dst
  {
    uint8_t r, g, b;
  };
  
  uint8_t tmp_r;
  
  struct rgb_Src *pivot = (struct rgb_Src *) pSrc;
  struct rgb_Dst *dest = (struct rgb_Dst *) pDst;
  
  for (int i = pixels-1; i >= 0; i--)
  {
    tmp_r=pivot[i].r;
    
    dest[i].r = pivot[i].b;
    dest[i].b = tmp_r;
    dest[i].g = pivot[i].g;
  }
}

