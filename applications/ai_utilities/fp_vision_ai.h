/**
 ******************************************************************************
 * @file    fp_vision_ai.h
 * @author  MCD Application Team
 * @brief   Header for fp_vision_ai.c module
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
#ifndef __FP_VISION_AI_H
#define __FP_VISION_AI_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "fp_vision_global.h"
#include "ai_interface.h"

#ifdef OBJECTDETECT
#include "peopledetect_network_pp_if.h"
#endif

/* Private macros ------------------------------------------------------------*/
#define _MIN(x_, y_) \
  ( ((x_)<(y_)) ? (x_) : (y_) )

#define _MAX(x_, y_) \
    ( ((x_)>(y_)) ? (x_) : (y_) )

#define _CLAMP(x_, min_, max_, type_) \
      (type_) (_MIN(_MAX(x_, min_), max_))

#define _ROUND(v_, type_) \
        (type_) ( ((v_)<0) ? ((v_)-0.5f) : ((v_)+0.5f) )


/* Exported types ------------------------------------------------------------*/
typedef struct
{
  void* nn_output_buffer;       //指向神经网络输出缓冲区的指针
  void* nn_input_buffer;        //指向神经网络输入缓冲区的指针
  void* activation_buffer;      //指向激活缓冲区的指针        激活缓冲区通常用于存储中间层的激活值
  uint8_t* lut;                 //指向查找表（Look-Up Table）的指针。用于优化计算过程
  float    nn_input_norm_scale; //代表神经网络输入的比例因子。在神经网络中，输入数据通常会进行标准化处理，以使得数据落在一定的范围内，有利于网络的训练和推断。
  int32_t  nn_input_norm_zp;    //代表神经网络输入的零点。它也是进行数据标准化的一部分，和nn_input_norm_scale一起，决定了标准化的具体方式。
  void*    AppCtxPtr;           //指向应用上下文的指针，可能被用于存储和管理特定应用相关的数据或状态。
} AiContext_TypeDef;    //存储和管理神经网络的输入,输出,激活,查找表,标准化尺度因子,标准化零点

#include "fp_vision_app.h"

#ifndef OBJECTDETECT
/* Exported constants --------------------------------------------------------*/
extern AiContext_TypeDef Ai_Context;

/* Exported functions ------------------------------------------------------- */
void AI_Deinit(void);
void AI_Run(AiContext_TypeDef* );
void AI_Init(AiContext_TypeDef*);
void AI_PixelValueConversion_QuantizedNN(AiContext_TypeDef* , uint8_t *);
void AI_PixelValueConversion_FloatNN(AiContext_TypeDef* , uint8_t *, uint32_t );
void AI_Output_Dequantize(AiContext_TypeDef* );
void AI_Softmax(AiContext_TypeDef* Ai_Context_Ptr);
void AI_PixelValueConversion(AiContext_TypeDef* , void *);
#endif /* !OBJECTDETECT */

#ifdef __cplusplus
}
#endif

#endif /*__FP_VISION_AI_H */

