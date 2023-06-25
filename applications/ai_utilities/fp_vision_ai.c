/**
 ******************************************************************************
 * @file    fp_vision_ai.c
 * @author  MCD Application Team
 * @brief   FP VISION AI configuration
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
#include "fp_vision_ai.h"
//#include "layers.h"
#include <stdio.h>
#include <string.h>

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Global variables ----------------------------------------------------------*/

AiContext_TypeDef Ai_Context;

uint8_t pixel_conv_lut[256];

/* Private function prototypes -----------------------------------------------*/
static void Compute_pix_conv_tab(AiContext_TypeDef *Ai_Context_Ptr);
static void Precompute_8FXP(uint8_t *lut, uint32_t q_input_shift);
static void Precompute_8IntU(uint8_t *lut, float scale, int32_t zp, float scale_prepro, int32_t zp_prepro);
static void Precompute_8IntS(uint8_t *lut, float scale, int32_t zp, float scale_prepro, int32_t zp_prepro);
static void Ai_Context_Init(AiContext_TypeDef *Ai_Context_Ptr);

/* Functions Definition ------------------------------------------------------*/
/**
  * @brief Performs pixel conversion from 8-bits integer to 8-bits "Fixed-point Qm,n" quantized format expected by NN input with normalization in the range [0,+1]
  * 
  * @param lut            Look Up Table for storing the precomputed values that will be subsequently used for the pixel conversion at execution time     
  * @param q_input_shift  Shift to be applied to be compatible with the Qm,n format expected by the input layer of the quantized model
  */
static void Precompute_8FXP(uint8_t *lut, uint32_t q_input_shift)
{
  uint32_t index;
  
 for(index=0;index<256;index++)
 {
   *(lut+index)=__USAT((index + (1 << q_input_shift)) >> (1 + q_input_shift), 8);
 }
}

/**
 * @brief  Performs pixel conversion from 8-bits integer to 8-bits "Integer Arithmetic Unsigned" quantized format expected by NN input with normalization in the range [0,+1]
 *
 * @param lut   Look Up Table for storing the precomputed values that will be subsequently used for the pixel conversion at execution time
 * @param scale Scale factor value to be used for the conversion to "Integer Arithmetic Unsigned" quantized format
 * @param zp    Zero-Point value to be used for the conversion to "Integer Arithmetic Unsigned" quantized format
 * @param scale_prepro Preprocessing scale. Specific to normalization type.
 * @param zp_prepro    Preprocessing zero-point. Specific to normalization type.
 */
static void Precompute_8IntU(uint8_t *lut, float scale, int32_t zp, float scale_prepro, int32_t zp_prepro)
{
  for (int32_t i = 0 ; i < 256 ; i++)
  {
    float tmp = (i - zp_prepro) * scale_prepro;
    *(lut + i) = _CLAMP(zp + _ROUND(tmp * scale, int32_t), 0, 255, uint8_t);
  }
}

/**
 * @brief  Performs pixel conversion from 8-bits integer to 8-bits "Integer Arithmetic Signed" quantized format expected by NN input
 *
 * @param lut   Look Up Table for storing the precomputed values that will be subsequently used for the pixel conversion at execution time
 * @param scale Scale factor value to be used for the conversion to "Integer Arithmetic Signed" quantized format
 * @param zp    Zero-Point value to be used for the conversion to "Integer Arithmetic Signed" quantized format
 * @param scale_prepro Preprocessing scale. Specific to normalization type.
 * @param zp_prepro    Preprocessing zero-point. Specific to normalization type.
 */
static void Precompute_8IntS(uint8_t *lut, float scale, int32_t zp, float scale_prepro, int32_t zp_prepro)
{
  for (int32_t i = 0 ; i < 256 ; i++)
  {
    float tmp = (i - zp_prepro) * scale_prepro;
    *(lut + i) = _CLAMP(zp + _ROUND(tmp * scale, int32_t),  -128, 127, int8_t);
  }
}

/**
 * @brief Performs adequate preprocessing on the captured frame image
 * @param Ai_Context_Ptr Pointer to AI context
 * @retval None
 */
static void Compute_pix_conv_tab(AiContext_TypeDef *Ai_Context_Ptr)
{
  uint8_t *lut = Ai_Context_Ptr->lut;
  float prepro_scale = Ai_Context_Ptr->nn_input_norm_scale;
  int32_t prepro_zp = Ai_Context_Ptr->nn_input_norm_zp;

  /*Retrieve the quantization scheme used to quantize the neural network*/
  switch(ai_get_input_quantization_scheme())
  {
  case AI_FXP_Q:
    /*Pixel value convertion and normalisation*/
    Precompute_8FXP(lut, ai_get_input_quantized_format());
    break;

  case AI_UINT_Q:
    /*Pixel value convertion and normalisation*/
    Precompute_8IntU(lut, ai_get_input_scale(), ai_get_input_zero_point(), prepro_scale, prepro_zp);
    break;

  case AI_SINT_Q:
    /*Pixel value convertion and normalisation*/
    Precompute_8IntS(lut, ai_get_input_scale(), ai_get_input_zero_point(), prepro_scale, prepro_zp);
    break;

  default:
    break;
  }
}

/**
 * @brief Initializes the AI context structure
 * @param Ai_Context_Ptr Pointer to AI context
 */
static void Ai_Context_Init(AiContext_TypeDef *Ai_Context_Ptr)
{
 Ai_Context_Ptr->lut=pixel_conv_lut;
}

/**
  * @brief  Initializes the generated C model for a neural network
  * @param  Ai_Context_Ptr Pointer to the AI NN context
  * @retval None
  */
void AI_Init(AiContext_TypeDef* Ai_Context_Ptr)
{
#ifdef AI_NETWORK_INPUTS_IN_ACTIVATIONS
  void *input_data_ptr;
  
  input_data_ptr =ai_init((void*)(Ai_Context_Ptr->activation_buffer));
  
  if(input_data_ptr!= NULL)
    Ai_Context_Ptr->nn_input_buffer=input_data_ptr;
#else
  if (Ai_Context_Ptr->nn_input_buffer == NULL)
  {
    while(1);
  }
  
  /*Initialize the AI library*/
  ai_init((void*)(Ai_Context_Ptr->activation_buffer));
#endif
  
  Ai_Context_Init(Ai_Context_Ptr);
  Compute_pix_conv_tab(Ai_Context_Ptr);
}

/**
* @brief  Initializes the generated C model for a neural network
* @param  Ai_Context_Ptr Pointer to the AI NN context
* @retval None
*/
void AI_Run(AiContext_TypeDef* Ai_Context_Ptr)
{
  ai_run((void*)Ai_Context_Ptr->nn_input_buffer, (void*)Ai_Context_Ptr->nn_output_buffer);
}

/**
* @brief  Performs pixel conversion in format expected by NN input
* @param  Ai_Context_Ptr Pointer to the AI NN context
* @param  pSrc           Pointer to source buffer
* @retval None
*/
void AI_PixelValueConversion(AiContext_TypeDef* Ai_Context_Ptr, void *pSrc)
{
  /**Check format of the input so to call the right fucntion for pixel format conversion**/
  if(ai_get_input_format() == AI_BUFFER_FMT_TYPE_Q)
  {
    AI_PixelValueConversion_QuantizedNN(Ai_Context_Ptr, (uint8_t *)pSrc);
  }
  else if(ai_get_input_format() == AI_BUFFER_FMT_TYPE_FLOAT)
  {
    if(Ai_Context_Ptr->nn_input_norm_scale == 255.0f)
    {
      AI_PixelValueConversion_FloatNN(Ai_Context_Ptr, pSrc, 0);
    }
    else if(Ai_Context_Ptr->nn_input_norm_scale == 127.0f)
    {
      AI_PixelValueConversion_FloatNN(Ai_Context_Ptr, pSrc, 1);
    }
    else
    {
      while(1);
    }
  }
  else
  {
    while(1);
  }
}
    
/**
* @brief  Performs pixel conversion from 8-bits integer to 8-bits quantized format expected by NN input with normalization
* @param  Ai_Context_Ptr Pointer to the AI NN context
* @param  pSrc           Pointer to source buffer
* @retval None
*/
void AI_PixelValueConversion_QuantizedNN(AiContext_TypeDef* Ai_Context_Ptr, uint8_t *pSrc)
{
  const uint32_t nb_pixels = AI_NETWORK_WIDTH * AI_NETWORK_HEIGHT * AI_NETWORK_CHANNEL;
  const uint8_t *lut = Ai_Context_Ptr->lut;
  uint8_t *pDst = (uint8_t *) Ai_Context_Ptr->nn_input_buffer;

  if (pDst > pSrc)
  {
    for (int32_t i = nb_pixels - 1; i >= 0; i--)
    {
      pDst[i] = lut[pSrc[i]];
    }
  }
  else
  {
    for (int32_t i = 0; i < nb_pixels; i++)
    {
      pDst[i] = lut[pSrc[i]];
    }
  }

}

/**
 * @brief  Performs pixel conversion from 8-bits integer to float simple precision with normalization
 * @param  Ai_Context_Ptr Pointer to the AI NN context
 * @param  pSrc           Pointer to source buffer
 * @param  normalization_type  Asymmetric normalization (=0), i.e. in the range [0,+1] or Symmetric normalization (=1), i.e. in the range [-1,+1]
 * @retval None
 */
void AI_PixelValueConversion_FloatNN(AiContext_TypeDef* Ai_Context_Ptr, uint8_t *pSrc, uint32_t normalization_type)
{
  const uint32_t nb_pixels = AI_NETWORK_WIDTH * AI_NETWORK_HEIGHT * AI_NETWORK_CHANNEL;
  float *pDst = (float *) Ai_Context_Ptr->nn_input_buffer;
  float div;
  float sub;

  if (normalization_type == 0)
  {/*NN input data in the range [0 , +1]*/
    div=255.0F;
    sub=0.0F;
  }
  else if(normalization_type == 1)
  {/*NN input data in the range [-1 , +1]*/
    div=127.5F;
    sub=1.0F;
  }
  else
  {
    while(1);
  }

  for (int32_t i = 0; i < nb_pixels; i++)
  {
    *pDst++ = (((float) *pSrc++) / div) - sub;
  }

}

/**
* @brief  Performs the dequantization of a quantized NN output
* @param  Ai_Context_Ptr Pointer to the AI NN context
* @retval None
*/
void AI_Output_Dequantize(AiContext_TypeDef* Ai_Context_Ptr)
{
  /**Check format of the output and convert to float if required**/
  if(ai_get_output_format() == AI_BUFFER_FMT_TYPE_Q)
  {
    float scale;
    int32_t zero_point;
    ai_i8 *nn_output_i8;
    ai_u8 *nn_output_u8;
    float *nn_output_f32;
    
    /*Check what type of quantization scheme is used for the output*/
    switch(ai_get_output_quantization_scheme())
    {
    case AI_FXP_Q:
      
      scale=ai_get_output_fxp_scale();
      
      /* Dequantize NN output - in-place 8-bit to float conversion */
      nn_output_i8 = (ai_i8 *) Ai_Context_Ptr->nn_output_buffer;
      nn_output_f32 = (float *) Ai_Context_Ptr->nn_output_buffer;
      for(int32_t i = AI_NET_OUTPUT_SIZE - 1; i >= 0; i--)
      {
        float q_value = (float) *(nn_output_i8 + i);
        *(nn_output_f32 + i) = scale * q_value;
      }
      break;
      
    case AI_UINT_Q:
      
      scale = ai_get_output_scale();
      zero_point = ai_get_output_zero_point();
      
      /* Dequantize NN output - in-place 8-bit to float conversion */
      nn_output_u8 = (ai_u8 *) Ai_Context_Ptr->nn_output_buffer;
      nn_output_f32 = (float *) Ai_Context_Ptr->nn_output_buffer;
      for(int32_t i = AI_NET_OUTPUT_SIZE - 1; i >= 0; i--)
      {
        int32_t q_value = (int32_t) *(nn_output_u8 + i);
        *(nn_output_f32 + i) = scale * (q_value - zero_point);
      }
      break;
      
    case AI_SINT_Q:
      
      scale = ai_get_output_scale();
      zero_point = ai_get_output_zero_point();
      
      /* Dequantize NN output - in-place 8-bit to float conversion */
      nn_output_i8 = (ai_i8 *) Ai_Context_Ptr->nn_output_buffer;
      nn_output_f32 = (float *) Ai_Context_Ptr->nn_output_buffer;
      for(int32_t i = AI_NET_OUTPUT_SIZE - 1; i >= 0; i--)
      {
        int32_t q_value = (int32_t) *(nn_output_i8 + i);
        *(nn_output_f32 + i) = scale * (q_value - zero_point);
      }
      break;
      
    default:
      break;
    }  
  }
}

/**
 * @brief Normalize output into probability distribution
 * @param Ai_Context_Ptr Pointer to the AI NN context
 */
void AI_Softmax(AiContext_TypeDef* Ai_Context_Ptr)
{
  float *nn_buff = Ai_Context_Ptr->nn_output_buffer;
  float max = nn_buff[0];
  float sum = 0.0f;

  /* Compute maximum */
  for (uint32_t i = 0; i < AI_NET_OUTPUT_SIZE; i++)
  {
    max = (max > nn_buff[i]) ? max : nn_buff[i];
  }

  /* subtract maximum to avoid softmax saturation and calculate sum */
  for (uint32_t i = 0; i < AI_NET_OUTPUT_SIZE; i++)
  {
    nn_buff[i] = expf(nn_buff[i] - max);
    sum += nn_buff[i];
  }

  /* normalize probabilities multiplying by the inverse of sum */
  sum = 1.0f / sum;
  for (uint32_t i = 0; i < AI_NET_OUTPUT_SIZE; i++)
  {
    nn_buff[i] *= sum;
  }
}

/**
 * @brief De-initializes the generated C model for a neural network
 */
void AI_Deinit(void) 
{ 
  ai_deinit(); 
}

