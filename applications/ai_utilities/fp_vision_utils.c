/**
 ******************************************************************************
 * @file    fp_vision_utils.c
 * @author  MCD Application Team
 * @brief   FP VISION utilities
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
#include "fp_vision_utils.h"

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Global variables ----------------------------------------------------------*/
extern DMA2D_HandleTypeDef hdma2d;
//UtilsContext_TypeDef UtilsContext;

/* Private function prototypes -----------------------------------------------*/
static uint32_t GetBytesPerPixel(uint32_t );
//static void Utils_Context_Init(UtilsContext_TypeDef *);

/* Functions Definition ------------------------------------------------------*/

/**
  * @brief  这个函数处理dma2d全局中断
  * @param  None
  * @retval None
  */
void DMA2D_IRQHandler(void)
{
  HAL_DMA2D_IRQHandler(&hdma2d);
}


/**
 * @brief Helper function to get the bytes per pixels according to the DMA2D color mode
 *
 * @param dma2d_color DMA2D color mode
 * @return uint32_t bytes per pixels for the input mode, either 4,3,2 or 0 if unknown input mode
 *
 * 根据DMA2D的颜色模式计算每像素所对应的字节数
 *
 * uint32_t dma2d_color:DMA2D颜色模式。这是一个输入参数，表示将根据此颜色模式来决定每像素的字节数
 * return uint32_t:		返回值是每像素的字节数，可以是4、3、2或0。如果输入模式未知，则返回0
 */
static uint32_t GetBytesPerPixel(uint32_t dma2d_color)
{
  switch (dma2d_color)
  {
    case DMA2D_OUTPUT_ARGB8888:
      return 4;
    case DMA2D_OUTPUT_RGB888:
      return 3;
    case DMA2D_OUTPUT_RGB565:
    case DMA2D_OUTPUT_ARGB1555:
    case DMA2D_OUTPUT_ARGB4444:
      return 2;
    default:
      return 0;
  }
}

///**
// * @brief Initializes the utilities context structure
// * @param Utils_Context_Ptr Pointer to Utilities context
// */
//static void Utils_Context_Init(UtilsContext_TypeDef *Utils_Context_Ptr)
//{
//  Utils_Context_Ptr->JoystickContext.joyState=JOY_NONE;
//  Utils_Context_Ptr->JoystickContext.camContrast=0;/* Mid-level camera contrast. */
//  Utils_Context_Ptr->JoystickContext.camBrightness=0;/* Mid-level camera brightness. */
//  Utils_Context_Ptr->JoystickContext.lcdBrightness=LCD_BRIGHTNESS_MID;
//
//  Utils_Context_Ptr->ExecTimingContext.tcapturestart1= 0;
//  Utils_Context_Ptr->ExecTimingContext.tcapturestart2= 0;
//  Utils_Context_Ptr->ExecTimingContext.tcapturestart= 0;
//  Utils_Context_Ptr->ExecTimingContext.tcapturestop=0;
//}
//
///**
//* @brief  UTILS Initialization
//* @param  Utils_Context_Ptr  Pointer to Utilities context
//* @retval None
//*/
//void UTILS_Init(UtilsContext_TypeDef *Utils_Context_Ptr)
//{
//  Utils_Context_Init(Utils_Context_Ptr);
//
//  /*LEDs Init*/
//  BSP_LED_Init(LED_GREEN);
//  BSP_LED_Init(LED_ORANGE);
//  BSP_LED_Init(LED_RED);
//  BSP_LED_Init(LED_BLUE);
//
//  /* Activate joystick. */
//  BSP_JOY_Init(JOY1, JOY_MODE_GPIO,JOY_ALL);
//
//  /* Configure the Wakeup push-button in GPIO Mode */
//  BSP_PB_Init(BUTTON_WAKEUP, BUTTON_MODE_GPIO);
//}
//
///**
//* @brief  Get timestamp
//* @param  Utils_Context_Ptr  Pointer to Utilities context
//* @retval Time stamp
//*/
///*获取时间戳*/
//uint32_t UTILS_GetTimeStamp(UtilsContext_TypeDef *Utils_Context_Ptr)
//{
//  return HAL_GetTick();
//}

/**
 * @brief Performs a DMA transfer from an arbitrary address to an arbitrary address
 *
 * @param pSrc address of the source
 * @param pDst address of the destination
 * @param x x position in the destination
 * @param y y position in the destination
 * @param xsize width of the source
 * @param ysize height of the source
 * @param rowStride width of the destination
 * @param input_color_format input color format (e.g DMA2D_INPUT_RGB888)
 * @param output_color_format output color format (e.g DMA2D_OUTPUT_ARGB888)
 * @param pfc boolean flag for pixel format conversion (set to 1 if input and output format are different, else 0)
 * @param red_blue_swap boolean flag for red-blue channel swap, 0 if no swap, else 1
 *
 * @brief 执行DMA（直接内存访问）2D传输，从任意源地址复制数据到任意目标地址
 *
 * @param *pSrc:		源地址，指示数据应从何处复制
 * @param *pDst:		目标地址，指示应将数据复制到何处
 * @param x:			目标位置的x坐标，即在目标数据的二维空间中，数据应该被复制到的列的位置。
 * @param y:			目标位置的y坐标，即在目标数据的二维空间中，数据应该被复制到的行的位置。
 * @param xsize:		源数据的宽度，即应复制的数据的列数。
 * @param ysize:		源数据的高度，即应复制的数据的行数。
 * @param rowStride:	目标数据的宽度，通常这是一行数据中的像素数或一行数据的字节数。
 * @param input_color_format:	输入数据的颜色格式，例如DMA2D_INPUT_RGB888表示输入数据的颜色格式是RGB888。
 * @param output_color_format:	输出数据的颜色格式，例如DMA2D_OUTPUT_ARGB888表示输出数据的颜色格式是ARGB888。
 * @param pfc: 					指示是否需要进行像素格式转换。如果输入和输出格式不同，该参数设为1，否则设为0。
 * @param red_blue_swap:		指示是否需要交换红色和蓝色通道。如果不需要交换，该参数设为0，否则设为1。
*/
void UTILS_Dma2d_Memcpy(uint32_t *pSrc, uint32_t *pDst, uint16_t x, uint16_t y, uint16_t xsize, uint16_t ysize,
                        uint32_t rowStride, uint32_t input_color_format, uint32_t output_color_format, int pfc,
                        int red_blue_swap)
{
  uint32_t bytepp = GetBytesPerPixel(output_color_format);				//根据DMA2D的颜色模式计算每像素所对应的字节数

  uint32_t destination = (uint32_t)pDst + (y * rowStride + x) * bytepp;	//计算DMA传输目标地址
  uint32_t source = (uint32_t)pSrc;										//源地址（即数据的起始位置）由函数参数pSrc指定

  HAL_DMA2D_DeInit(&hdma2d);								    //DMA2D取消初始化重置默认值

  /*##-1- Configure the DMA2D Mode, Color Mode and output offset #############*/
  /*##-1- 配置DMA2D模式，颜色模式和输出偏移量 ###########################################*/
  hdma2d.Init.Mode = pfc ? DMA2D_M2M_PFC : DMA2D_M2M;		//判断是否进行像素格式转换
  hdma2d.Init.ColorMode = output_color_format;				//设置DMA2D输出图像的色彩格式

  /* Output offset in pixels == nb of pixels to be added at end of line to come to the  */
  /* first pixel of the next line : on the output side of the DMA2D computation         */
  /*像素输出偏移量等于每行结束时需要增加的像素数量，以到达下一行的第一个像素：这发生在DMA2D运算的输出端*/
  hdma2d.Init.OutputOffset = rowStride - xsize;

  /*##-2- DMA2D Callbacks Configuration ######################################*/
  /*##-2- 配置DMA2D回调 #########################################################*/
  hdma2d.XferCpltCallback = NULL;

  /*##-3- Foreground Configuration ###########################################*/
  /*##-3- 前景配置 ##############################################################*/
  hdma2d.LayerCfg[1].AlphaMode = DMA2D_REPLACE_ALPHA;		//设置DMA2D前景的alpha模式为替换模式，即替换前景的alpha通道
  hdma2d.LayerCfg[1].InputAlpha = 0xFF;						//设置DMA2D前景的alpha值为0xFF（即255，表示完全不透明）
  hdma2d.LayerCfg[1].InputColorMode = input_color_format;	//设置DMA2D前景的色彩格式
  hdma2d.LayerCfg[1].InputOffset = 0;						//设置DMA2D前景的偏移为0，即不跳过任何像素
  hdma2d.LayerCfg[1].RedBlueSwap = red_blue_swap ? DMA2D_RB_SWAP : DMA2D_RB_REGULAR;	//判断是否使用红蓝交换模式

  /* DMA2D Initialization */
  if (HAL_DMA2D_Init(&hdma2d) == HAL_OK)	//DMA2D初始化
  {
    if (HAL_DMA2D_ConfigLayer(&hdma2d, 1) == HAL_OK)	//配置DMA2D前景图层参数
    {
    	/*
    	 * @brief 开始DMA2D传输
    	 * @param hdma2d: 这是指向DMA2D_HandleTypeDef结构的指针，该结构包含DMA2D的配置信息。
		 * @param pdata: 根据所选择的模式，这个参数可以是源内存缓冲区的地址（如果选择的是内存到内存或内存到内存的像素格式转换模式），或者是颜色值（如果选择的是寄存器到内存模式）。
		 * @param DstAddress: 这是目标内存缓冲区的地址。
		 * @param Width: 这是要从源传输到目标的数据的宽度（以每行的像素数表示）。
		 * @param Height: 这是要从源传输到目标的数据的高度（以行数表示）。
		 * @retval HAL status: 这是函数的返回值，表示函数的执行状态，例如HAL_OK表示操作成功，其他值可能表示出现错误。
    	 * */
      if (HAL_DMA2D_Start(&hdma2d, source, destination, xsize, ysize) == HAL_OK)
      {
        /* Polling For DMA transfer */
        HAL_DMA2D_PollForTransfer(&hdma2d, 30);			//轮询DMA2D以查看传输是否完成或颜色查找表是否已加载 超时时间30
      }
    }
  }
}

///**
// * @brief  Get the joystick's status
// * @param  Utils_Context_Ptr pointer to utilities context
// * @retval None
// */
//void UTILS_Joystick_Check(UtilsContext_TypeDef *Utils_Context_Ptr)
//{
//  JoystickContext_TypeDef* Joystick_Ctx_Ptr=&Utils_Context_Ptr->JoystickContext;
//
//  /* Get the joystick state. */
//  Joystick_Ctx_Ptr->joyState = (JOYPin_TypeDef) BSP_JOY_GetState(JOY1, 0);
//
//  switch (Joystick_Ctx_Ptr->joyState)
//  {
//    case JOY_UP:
//      if (Joystick_Ctx_Ptr->lcdBrightness < LCD_BRIGHTNESS_MAX)
//      {
//        Joystick_Ctx_Ptr->lcdBrightness += LCD_BRIGHTNESS_STEP;
//      }
//      else
//      {
//        Joystick_Ctx_Ptr->joyState = JOY_NONE; /* Cancel LCD brightness change. */
//      }
//      break;
//
//    case JOY_DOWN:
//      if (Joystick_Ctx_Ptr->lcdBrightness > LCD_BRIGHTNESS_MIN)
//      {
//        Joystick_Ctx_Ptr->lcdBrightness -= LCD_BRIGHTNESS_STEP;
//      }
//      else
//      {
//        Joystick_Ctx_Ptr->joyState = JOY_NONE; /* Cancel LCD brightness change. */
//      }
//      break;
//
//    case JOY_LEFT:
//      if (Joystick_Ctx_Ptr->camContrast > CAMERA_CONTRAST_MIN)
//      {
//        Joystick_Ctx_Ptr->camContrast--;
//      }
//      else
//      {
//        Joystick_Ctx_Ptr->joyState = JOY_NONE; /* Cancel camera contrast change. */
//      }
//      break;
//
//    case JOY_RIGHT:
//      if (Joystick_Ctx_Ptr->camContrast < CAMERA_CONTRAST_MAX)
//      {
//        Joystick_Ctx_Ptr->camContrast++;
//      }
//      else
//      {
//        Joystick_Ctx_Ptr->joyState = JOY_NONE; /* Cancel camera contrast change. */
//      }
//      break;
//
//    default:
//      break;
//  }
//
//  if ((Joystick_Ctx_Ptr->joyState == JOY_UP) || (Joystick_Ctx_Ptr->joyState == JOY_DOWN) || (Joystick_Ctx_Ptr->joyState == JOY_LEFT) || (Joystick_Ctx_Ptr->joyState == JOY_RIGHT))
//  {
//    /* Execute the change. */
//    BSP_CAMERA_SetBrightness(0, Joystick_Ctx_Ptr->camBrightness);
//    BSP_CAMERA_SetContrast(0, Joystick_Ctx_Ptr->camContrast);
//    BSP_LCD_SetBrightness(0, Joystick_Ctx_Ptr->lcdBrightness);
//  }
//  else if (Joystick_Ctx_Ptr->joyState == JOY_SEL)
//  {
//    Joystick_Ctx_Ptr->camContrast = 0;     /* Mid-level camera contrast. */
//    Joystick_Ctx_Ptr->camBrightness = 0; /* Mid-level camera brightness. */
//    Joystick_Ctx_Ptr->lcdBrightness = LCD_BRIGHTNESS_MID;       /* Mid-level LCD brightness. */
//
//    /* Restore the default values. */
//    BSP_CAMERA_SetBrightness(0, Joystick_Ctx_Ptr->camBrightness);
//    BSP_CAMERA_SetContrast(0, Joystick_Ctx_Ptr->camContrast);
//    BSP_LCD_SetBrightness(0, Joystick_Ctx_Ptr->lcdBrightness);
//  }
//}
//
///**
// * @brief Performs Data Cache maintenance for coherency purpose
// * @param mem_addr Pointer to memory block address (aligned to 32-byte boundary)
// * @param mem_size Size of memory block (in number of bytes)
// * @param Maintenance_operation type of maintenance: CLEAN or INVALIDATE
// * @retval None
// */
//void UTILS_DCache_Coherency_Maintenance(uint32_t *mem_addr, int32_t mem_size, DCache_Coherency_TypeDef Maintenance_operation)
//{
//  /*Check alignement on 32-bytes for the memory adress and check that the memory size is multiple of 32-bytes*/
//  if(((uint32_t)mem_addr%32 != 0) || (mem_size%32 != 0))
//    while(1);
//
//  if(Maintenance_operation == INVALIDATE)
//  {
//    SCB_InvalidateDCache_by_Addr((void*)mem_addr, mem_size);
//  }
//  else if(Maintenance_operation == CLEAN)
//  {
//    SCB_CleanDCache_by_Addr((void *)mem_addr, mem_size);
//  }
//}
//
///**
// * @brief Bubble sorting algorithm on probabilities
// * @param prob pointer to probabilities buffer
// * @param classes pointer to classes buffer
// * @param size numer of values
// */
//void UTILS_Bubblesort(float *prob, int *classes, int size)
//{
//  float p;
//  int c;
//
//  for (int i = 0; i < size; i++)
//  {
//    for (int ii = 0; ii < size - i - 1; ii++)
//    {
//      if (prob[ii] < prob[ii + 1])
//      {
//        p = prob[ii];
//        prob[ii] = prob[ii + 1];
//        prob[ii + 1] = p;
//        c = classes[ii];
//        classes[ii] = classes[ii + 1];
//        classes[ii + 1] = c;
//      }
//    }
//  }
//}
//
///**
// * @brief Compute the execution timing  of the various operation
// * @param UtilsContext_Ptr pointer to utilities context
// */
//void UTILS_Compute_ExecutionTiming(UtilsContext_TypeDef *UtilsContext_Ptr)
//{
//  AppContext_TypeDef *App_Cxt_Ptr=UtilsContext_Ptr->AppCtxPtr;
//
//  UtilsContext_Ptr->ExecTimingContext.operation_exec_time[FRAME_CAPTURE] = UtilsContext_Ptr->ExecTimingContext.tcapturestop - UtilsContext_Ptr->ExecTimingContext.tcapturestart;
//
//#if MEMORY_SCHEME == FULL_INTERNAL_MEM_OPT
//  UtilsContext_Ptr->ExecTimingContext.Tfps =UtilsContext_Ptr->ExecTimingContext.operation_exec_time[FRAME_PFC] + UtilsContext_Ptr->ExecTimingContext.operation_exec_time[FRAME_PVC] + UtilsContext_Ptr->ExecTimingContext.operation_exec_time[FRAME_RESIZE] + UtilsContext_Ptr->ExecTimingContext.operation_exec_time[FRAME_INFERENCE] + UtilsContext_Ptr->ExecTimingContext.operation_exec_time[FRAME_CAPTURE];
//#elif MEMORY_SCHEME != FULL_INTERNAL_MEM_OPT
//  if((UtilsContext_Ptr->ExecTimingContext.operation_exec_time[FRAME_PFC] + UtilsContext_Ptr->ExecTimingContext.operation_exec_time[FRAME_INFERENCE]) > UtilsContext_Ptr->ExecTimingContext.operation_exec_time[FRAME_CAPTURE])
//    UtilsContext_Ptr->ExecTimingContext.Tfps = UtilsContext_Ptr->ExecTimingContext.operation_exec_time[FRAME_PFC] + UtilsContext_Ptr->ExecTimingContext.operation_exec_time[FRAME_PVC] + UtilsContext_Ptr->ExecTimingContext.operation_exec_time[FRAME_RESIZE] + UtilsContext_Ptr->ExecTimingContext.operation_exec_time[FRAME_INFERENCE];
//  else
//    UtilsContext_Ptr->ExecTimingContext.Tfps =  UtilsContext_Ptr->ExecTimingContext.operation_exec_time[FRAME_CAPTURE];
//#endif
//
//  App_Cxt_Ptr->nn_inference_time=UtilsContext_Ptr->ExecTimingContext.operation_exec_time[FRAME_INFERENCE];
//}

