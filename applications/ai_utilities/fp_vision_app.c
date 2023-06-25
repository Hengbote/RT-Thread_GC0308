/**
 ******************************************************************************
 * @file    fp_vision_app.c
 * @author  MCD Application Team
 * @brief   FP VISION Application configuration
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
#include <rtthread.h>
#include <rtdevice.h>
#include "fp_vision_app.h"
#include "ai_utilities.h"
#include "stm32ipl.h"
#include <stdio.h>
#include <string.h>

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
#define MAX(x, y) (x > y ? x : y)

#define DBG_TAG "FRAME"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

/* Private variables ---------------------------------------------------------*/
/* Global variables ----------------------------------------------------------*/
AppContext_TypeDef App_Context;
rt_thread_t frame_response_t;                  //摄像头任务结构体

/*****************************************/
/***AI_FP_GLOBAL_BUFFER_SIZE definition***/
/*****************************************/
#if MEMORY_SCHEME == FULL_INTERNAL_MEM_OPT   
 #ifdef AI_NETWORK_INPUTS_IN_ACTIVATIONS
  #define AI_FP_GLOBAL_BUFFER_SIZE  (MAX(AI_ACTIVATION_BUFFER_SIZE, CAM_FRAME_BUFFER_SIZE)) 
 #else
  #define AI_FP_GLOBAL_BUFFER_SIZE  (MAX(AI_ACTIVATION_BUFFER_SIZE, CAM_FRAME_BUFFER_SIZE) + AI_INPUT_BUFFER_SIZE) 
 #endif
#elif MEMORY_SCHEME != FULL_INTERNAL_MEM_OPT
 #if MEMORY_SCHEME == SPLIT_INT_EXT
  #ifdef AI_NETWORK_INPUTS_IN_ACTIVATIONS
   #define AI_FP_GLOBAL_BUFFER_SIZE (CAM_FRAME_BUFFER_SIZE + CAM_FRAME_BUFFER_SIZE) 
  #else
   #define AI_FP_GLOBAL_BUFFER_SIZE (CAM_FRAME_BUFFER_SIZE + MAX(AI_INPUT_BUFFER_SIZE, CAM_FRAME_BUFFER_SIZE)) 
  #endif
 #else /*MEMORY_SCHEME == FULL_EXTERNAL*/
  #ifdef AI_NETWORK_INPUTS_IN_ACTIVATIONS
   #define AI_FP_GLOBAL_BUFFER_SIZE (CAM_FRAME_BUFFER_SIZE + MAX(AI_ACTIVATION_BUFFER_SIZE, CAM_FRAME_BUFFER_SIZE))
  #else
   //      全局缓冲区的大小         (摄像头帧缓冲区的大小  + 比较((AI激活缓冲区大小                + AI输入缓冲区大小         ),  摄像头帧缓冲区的大小)谁大)
   #define AI_FP_GLOBAL_BUFFER_SIZE (CAM_FRAME_BUFFER_SIZE + MAX(AI_ACTIVATION_BUFFER_SIZE + AI_INPUT_BUFFER_SIZE, CAM_FRAME_BUFFER_SIZE))
  #endif
 #endif
#else
 #error Please check definition of MEMORY_SCHEME define
#endif

#if MEMORY_SCHEME == FULL_EXTERNAL
  #if defined ( __ICCARM__ )
    #pragma location="Vision_App_Complete"
    #pragma data_alignment=32
  #elif defined ( __CC_ARM )
    __attribute__((section(".Vision_App_Complete"), zero_init))
    __attribute__ ((aligned (32)))
  #elif defined ( __GNUC__ )
    __attribute__((section(".Vision_App_Complete")))
    __attribute__ ((aligned (32)))
  #else
    #error Unknown compiler
  #endif
  uint8_t ai_fp_global_memory[AI_FP_GLOBAL_BUFFER_SIZE];
#elif MEMORY_SCHEME == SPLIT_INT_EXT
  #if defined ( __ICCARM__ )
    #pragma location="Vision_App_ImgPipeline"
    #pragma data_alignment=32
  #elif defined ( __CC_ARM )
    __attribute__((section(".Vision_App_ImgPipeline"), zero_init))
    __attribute__ ((aligned (32)))
  #elif defined ( __GNUC__ )
    __attribute__((section(".Vision_App_ImgPipeline")))
    __attribute__ ((aligned (32)))
  #else
    #error Unknown compiler
  #endif
  uint8_t ai_fp_global_memory[AI_FP_GLOBAL_BUFFER_SIZE];
  #if defined ( __ICCARM__ )
    #pragma location="Vision_App_Inference"
    #pragma data_alignment=32
  #elif defined ( __CC_ARM )
    __attribute__((section(".Vision_App_Inference"), zero_init))
    __attribute__ ((aligned (32)))
  #elif defined ( __GNUC__ )
    __attribute__((section(".Vision_App_Inference")))
    __attribute__ ((aligned (32)))
  #else
    #error Unknown compiler
  #endif
  uint8_t ai_fp_activation_memory[AI_ACTIVATION_BUFFER_SIZE];
#elif MEMORY_SCHEME == FULL_INTERNAL_FPS_OPT
  #if defined ( __ICCARM__ )
    #pragma location="Vision_App_Complete"
    #pragma data_alignment=32
  #elif defined ( __CC_ARM )
    __attribute__((section(".Vision_App_Complete"), zero_init))
    __attribute__ ((aligned (32)))
  #elif defined ( __GNUC__ )                            //对于GNU编译器
    __attribute__((section(".Vision_App_Complete")))    /*指定存储区段*/
    __attribute__ ((aligned (32)))                      /*指定32字节对齐*/
  #else
    #error Unknown compiler
  #endif
  uint8_t ai_fp_global_memory[AI_FP_GLOBAL_BUFFER_SIZE];
#elif MEMORY_SCHEME == FULL_INTERNAL_MEM_OPT
  #if defined ( __ICCARM__ )
    #pragma location="Vision_App_Complete"
    #pragma data_alignment=32
  #elif defined ( __CC_ARM )
    __attribute__((section(".Vision_App_Complete"), zero_init))
    __attribute__ ((aligned (32)))
  #elif defined ( __GNUC__ )                            //对于GNU编译器
    __attribute__((section(".Vision_App_Complete")))    /*指定存储区段*/
    __attribute__ ((aligned (32)))                      /*指定32字节对齐*/
  #else
    #error Unknown compiler
  #endif
  uint8_t ai_fp_global_memory[AI_FP_GLOBAL_BUFFER_SIZE];
#else
 #error Please check definition of MEMORY_SCHEME define
#endif

#if WEIGHT_QSPI == 1 && WEIGHT_EXEC_EXTRAM == 1
  #if MEMORY_SCHEME == FULL_EXTERNAL
    #if defined ( __ICCARM__ )
      #pragma location="Vision_App_Complete"
    #elif defined ( __CC_ARM )
      __attribute__((section(".Vision_App_Complete"), zero_init))
    #elif defined ( __GNUC__ )
      __attribute__((section(".Vision_App_Complete")))
    #else
      #error Unknown compiler
    #endif
  #elif MEMORY_SCHEME == SPLIT_INT_EXT
    #if defined ( __ICCARM__ )
      #pragma location="Vision_App_ImgPipeline"
    #elif defined ( __CC_ARM )
      __attribute__((section(".Vision_App_ImgPipeline"), zero_init))
    #elif defined ( __GNUC__ )
      __attribute__((section(".Vision_App_ImgPipeline")))
    #else
      #error Unknown compiler
    #endif
  #endif
  uint8_t weights_table_sdram[AI_WEIGHT_SIZE_BYTES];
#endif
  
uint8_t ai_fp_global_memory[AI_FP_GLOBAL_BUFFER_SIZE];	//AI全局内存数组	(摄像头帧缓冲区的大小+比较((AI激活缓冲区大小+AI输入缓冲区大小),摄像头帧缓冲区的大小)谁大)

float nn_output_buff[AI_NET_OUTPUT_SIZE] = {0};
 
//const char* output_labels[AI_NET_OUTPUT_SIZE] = {
//  "Apple Pie",      "Beer",    "Caesar Salad", "Cappuccino", "Cheesecake",          "Chicken Wings",
//  "Chocolate Cake", "Coke",    "Cup Cakes",    "Donuts",     "French Fries",        "Hamburger",
//  "Hot Dog",        "Lasagna", "Pizza",        "Risotto",    "Spaghetti Bolognese", "Steak"};

/* Private function prototypes -----------------------------------------------*/
//static void CameraCaptureBuff2LcdBuff_Copy(AppContext_TypeDef *);
//static void App_Output_Display(AppContext_TypeDef *);
static void App_Context_Init(AppContext_TypeDef *);
//
///* Functions Definition ------------------------------------------------------*/
//
///**
// * @brief  Transfer from camera frame buffer to LCD frame buffer for display
// * @param App_Context_Ptr pointer to application context
// */
//static void CameraCaptureBuff2LcdBuff_Copy(AppContext_TypeDef *App_Context_Ptr)
//{
//  AppOperatingMode_TypeDef operating_mode = App_Context_Ptr->Operating_Mode;
//  MemDumpFrameSource_TypeDef frame_source = App_Context_Ptr->Test_ContextPtr->DumpContext.Dump_FrameSource;
//  uint8_t *camera_capture_buffer = App_Context_Ptr->Camera_ContextPtr->camera_capture_buffer;
//
//  UTIL_LCD_Clear(UTIL_LCD_COLOR_BLACK);
//
//  if (operating_mode == VALID || ((operating_mode == DUMP) && (frame_source == SDCARD_FILE)))
//  {
//    /* Coherency purpose: clean the source buffer area in L1 D-Cache before DMA2D reading */
//    int32_t buffer_size = CAM_RES_WIDTH * CAM_RES_HEIGHT * RGB_565_BPP; /* Expecting 16-bpp BMP files on SDCard */
//    UTILS_DCache_Coherency_Maintenance((void *)camera_capture_buffer, buffer_size, CLEAN);
//  }
//
//  if (operating_mode == VALID)
//  {
//    /* Display a 224x224 thumbnail on LCD in VALID mode */
//    image_t camera_out_img;
//    image_t thumb_img;
//
//    camera_out_img.data = camera_capture_buffer;
//    camera_out_img.w = CAM_RES_WIDTH;
//    camera_out_img.h = CAM_RES_HEIGHT;
//    camera_out_img.bpp = IMAGE_BPP_RGB565;
//
//    thumb_img.data = valid_image_buff;
//    thumb_img.w = 224;
//    thumb_img.h = 224;
//    thumb_img.bpp = IMAGE_BPP_RGB565;
//
//    STM32Ipl_Downscale(&camera_out_img, &thumb_img, false);
//
//    /* Coherency purpose: clean the source buffer area in L1 D-Cache before DMA2D reading */
//    UTILS_DCache_Coherency_Maintenance((void *)thumb_img.data, STM32Ipl_ImageDataSize(&thumb_img), CLEAN);
//
//    /* DMA2D transfer from temp buffer "valid_image_buff" in external memory to LCD write buffer */
//    DISPLAY_Copy2LCDWriteBuffer(App_Context_Ptr->Display_ContextPtr,
//                                (uint32_t *)(thumb_img.data),
//                                50, 130,
//                                thumb_img.w, thumb_img.h, DMA2D_INPUT_RGB565,
//                                0);
//  }
//  else
//  {
//    /* DMA2D transfer from Camera capture buffer to LCD write buffer */
//    DISPLAY_Copy2LCDWriteBuffer(App_Context_Ptr->Display_ContextPtr,
//                                (uint32_t *)(camera_capture_buffer),
//                                (LCD_RES_WIDTH - CAM_RES_WIDTH) >> 1,
//                                (LCD_RES_HEIGHT - CAM_RES_HEIGHT) >> 1,
//                                CAM_RES_WIDTH,
//                                CAM_RES_HEIGHT,
//                                DMA2D_INPUT_RGB565,
//                                0);
//  }
//}
//
///**
// * @brief Process the output from NN and display it on LCD and LED
// * @param App_Context_Ptr pointer to application context
// */
//static void App_Output_Display(AppContext_TypeDef *App_Context_Ptr)
//{
//  static uint32_t occurrence_number = NN_OUTPUT_DISPLAY_REFRESH_RATE;
//  static uint32_t display_mode=0;//by default, normal mode i.e. logo is displayed
//
//  occurrence_number--;
//
//  if (occurrence_number == 0)
//  {
//    char msg[70];
//
//    occurrence_number = NN_OUTPUT_DISPLAY_REFRESH_RATE;
//
//    /*Check if PB is pressed*/
//    if (BSP_PB_GetState(BUTTON_WAKEUP) != RESET)
//    {
//      display_mode = !display_mode;
//
//      UTIL_LCD_Clear(UTIL_LCD_COLOR_BLACK);
//
//      if (display_mode == 1)
//      {
//        sprintf(msg, "Entering CAMERA PREVIEW mode");
//      }
//      else  if (display_mode == 0)
//      {
//        sprintf(msg, "Exiting CAMERA PREVIEW mode");
//      }
//
//      UTIL_LCD_DisplayStringAt(0, LINE(9), (uint8_t*)msg, CENTER_MODE);
//
//      sprintf(msg, "Please release button");
//      UTIL_LCD_DisplayStringAt(0, LINE(11), (uint8_t*)msg, CENTER_MODE);
//      DISPLAY_Refresh(App_Context_Ptr->Display_ContextPtr);
//
//      /*Wait for PB release*/
//      while (BSP_PB_GetState(BUTTON_WAKEUP) != RESET);
//      HAL_Delay(200);
//
//      UTIL_LCD_Clear(UTIL_LCD_COLOR_BLACK);
//    }
//
//    if (display_mode == 0)
//    {
//      UTIL_LCD_Clear(UTIL_LCD_COLOR_BLACK);/*To clear the camera capture*/
//
//      DISPLAY_FoodLogo(App_Context_Ptr->Display_ContextPtr, LCD_RES_WIDTH / 2 - 64, LCD_RES_HEIGHT / 2 -100, App_Context_Ptr->ranking[0]);
//    }
//    else  if (display_mode == 1)
//    {
//      sprintf(msg, "CAMERA PREVIEW MODE");
//
//      UTIL_LCD_DisplayStringAt(0, LINE(DISPLAY_ACQU_MODE_LINE), (uint8_t*)msg, CENTER_MODE);
//    }
//
//    for (int i = 0; i < NN_TOP_N_DISPLAY; i++)
//    {
//      sprintf(msg, "%s %.0f%%", NN_OUTPUT_CLASS_LIST[App_Context_Ptr->ranking[i]], *((float*)(App_Context_Ptr->Ai_ContextPtr->nn_output_buffer)+i) * 100);
//      UTIL_LCD_DisplayStringAt(0, LINE(DISPLAY_TOP_N_LAST_LINE - NN_TOP_N_DISPLAY + i), (uint8_t *)msg, CENTER_MODE);
//    }
//
//    sprintf(msg, "Inference: %ldms", App_Context_Ptr->nn_inference_time);
//    UTIL_LCD_DisplayStringAt(0, LINE(DISPLAY_INFER_TIME_LINE), (uint8_t *)msg, CENTER_MODE);
//
//    sprintf(msg, "Fps: %.1f", 1000.0F / (float)(App_Context_Ptr->Utils_ContextPtr->ExecTimingContext.Tfps));
//    UTIL_LCD_DisplayStringAt(0, LINE(DISPLAY_FPS_LINE), (uint8_t *)msg, CENTER_MODE);
//
//    DISPLAY_Refresh(App_Context_Ptr->Display_ContextPtr);
//
//    /*Toggle LED based on result confidence*/
//    BSP_LED_Off(LED_GREEN);
//    BSP_LED_Off(LED_ORANGE);
//    BSP_LED_Off(LED_RED);
//
//    if ((*((float*)(App_Context_Ptr->Ai_ContextPtr->nn_output_buffer)+0) * 100) < NN_BAD_RES)
//    {
//      BSP_LED_On(LED_RED);
//    }
//    else if (((*((float*)(App_Context_Ptr->Ai_ContextPtr->nn_output_buffer)+0) * 100) >= NN_BAD_RES) && ((*((float*)(App_Context_Ptr->Ai_ContextPtr->nn_output_buffer)+0) * 100) < NN_GOOD_RES))
//    {
//      BSP_LED_On(LED_ORANGE);
//    }
//    else
//    {
//      BSP_LED_On(LED_GREEN);
//    }
//  }
//}

/**
 * @brief Initializes the application context structure
 * @param App_Context_Ptr pointer to application context
 */
static void App_Context_Init(AppContext_TypeDef *App_Context_Ptr)	//初始化应用程序上下文结构
{  
  App_Context_Ptr->Operating_Mode=NOMINAL;	//正常操作模式
  App_Context_Ptr->run_loop=1;
  
  /*Initializes app pointers to contextx*/
//  App_Context_Ptr->Camera_ContextPtr=&CameraContext;	//指向一个存储相机状态信息的结构
//  App_Context_Ptr->Display_ContextPtr=&Display_Context;	//指向一个存储显示屏状态信息的结构
//  App_Context_Ptr->Utils_ContextPtr=&UtilsContext;		//桢操作执行时间 遥感(操纵杆)信息 相机和lcd亮度设置
//  App_Context_Ptr->Test_ContextPtr=&TestContext;		//测试上下文结构体	(内存转储,捕获的图像信息,模型验证相关信息,串口通信,图像测试运行,随机数,运行状态标记)
  App_Context_Ptr->Ai_ContextPtr=&Ai_Context;			//存储和管理神经网络的输入,输出,激活,查找表,标准化尺度因子,标准化零点
  App_Context_Ptr->Preproc_ContextPtr=&Preproc_Context;	//是一个预处理上下文结构，用于存储与图像预处理操作有关的状态和信息
  
//  /*Initializes app specific context's parameters */
//  /**Camera**/
//  App_Context_Ptr->Camera_ContextPtr->mirror_flip=CAMERA_MIRRORFLIP_NONE;	//相机输出无镜像或翻转
//  App_Context_Ptr->Camera_ContextPtr->AppCtxPtr =App_Context_Ptr;	//指向应用程序上下文结构	以便于在后续的程序运行中，能够通过这些指针访问到相应的上下文或结构体
//
//  /**Utilities**/
//  App_Context_Ptr->Utils_ContextPtr->AppCtxPtr =App_Context_Ptr;	//指向应用程序上下文结构	以便于在后续的程序运行中，能够通过这些指针访问到相应的上下文或结构体
//
//  /**Test**/
//  App_Context_Ptr->Test_ContextPtr->AppCtxPtr =App_Context_Ptr;		//指向应用程序上下文结构	以便于在后续的程序运行中，能够通过这些指针访问到相应的上下文或结构体
//#if CAMERA_CAPTURE_RES == VGA_640_480_RES
//  strcpy(App_Context_Ptr->Test_ContextPtr->ValidationContext.class_path,"/onboard_valid_dataset_vga");		//设置类别路径
//#elif CAMERA_CAPTURE_RES == QVGA_320_240_RES
//  strcpy(App_Context_Ptr->Test_ContextPtr->ValidationContext.class_path,"/onboard_valid_dataset_qvga");
//#endif
//
//  /**Display**/
//  App_Context_Ptr->Display_ContextPtr->AppCtxPtr =App_Context_Ptr;	//指向应用程序上下文结构	以便于在后续的程序运行中，能够通过这些指针访问到相应的上下文或结构体
  
  /**AI**/
  App_Context_Ptr->Ai_ContextPtr->AppCtxPtr =App_Context_Ptr;		//指向应用程序上下文结构	以便于在后续的程序运行中，能够通过这些指针访问到相应的上下文或结构体
  App_Context_Ptr->Ai_ContextPtr->nn_output_buffer=nn_output_buff;	//神经网络输出缓冲区
  /*{scale,zero-point} set to {255,0} since Food Reco NN model was trained using input data normalized in the range [0, 1]*/
  /*神经网络模型使用范围在[0, 1]的输入数据进行训练的*/
  App_Context_Ptr->Ai_ContextPtr->nn_input_norm_scale=255.0f;		//代表神经网络输入的比例因子
  App_Context_Ptr->Ai_ContextPtr->nn_input_norm_zp=0;				//代表神经网络输入的零点

  /**Preproc**/
  App_Context_Ptr->Preproc_ContextPtr->AppCtxPtr = App_Context_Ptr;	//指向应用程序上下文结构	以便于在后续的程序运行中，能够通过这些指针访问到相应的上下文或结构体
  App_Context_Ptr->Preproc_ContextPtr->red_blue_swap = 1; /* See UM2611 section 3.2.6 Pixel data order */	//交换红蓝颜色通道	参见UM2611 3.2.6像素数据顺序
  App_Context_Ptr->Preproc_ContextPtr->Resize_Dst_Img.data = NULL; /* Set by Init_DataMemoryLayout */	//在"Init_DataMemoryLayout"函数中分配和设置 素格式转换目标图像
  App_Context_Ptr->Preproc_ContextPtr->Resize_Dst_Img.w = AI_NETWORK_WIDTH;		//素格式转换目标图像宽
  App_Context_Ptr->Preproc_ContextPtr->Resize_Dst_Img.h = AI_NETWORK_HEIGHT;	//素格式转换目标图像高
  App_Context_Ptr->Preproc_ContextPtr->Resize_Dst_Img.bpp = IMAGE_BPP_RGB565;	//图像数据格式
  App_Context_Ptr->Preproc_ContextPtr->Pfc_Dst_Img.data = NULL; /* Set by Init_DataMemoryLayout */		//在"Init_DataMemoryLayout"函数中分配和设置 尺寸调整的目标图像
  App_Context_Ptr->Preproc_ContextPtr->Pfc_Dst_Img.w = AI_NETWORK_WIDTH;		//尺寸调整的目标图像宽
  App_Context_Ptr->Preproc_ContextPtr->Pfc_Dst_Img.h = AI_NETWORK_HEIGHT;		//尺寸调整的目标图像高
  App_Context_Ptr->Preproc_ContextPtr->Pfc_Dst_Img.bpp = IMAGE_BPP_RGB888;		//图像数据格式
}

/**
 * @brief Initializes the application context structure
 * @param App_Context_Ptr pointer to application context
 */
void APP_Init(AppContext_TypeDef *App_Context_Ptr)	//初始化应用程序上下文结构
{  
  App_Context_Init(App_Context_Ptr);		//初始化应用程序上下文结构
  Init_DataMemoryLayout(App_Context_Ptr);	//初始化应用程序数据的内存布局
}

///**
// * @brief Get the subsequent frame available and:
// *        1. send it to LCD for display
// *        2. make it available for subsequent processing
// * @param App_Context_Ptr pointer to application context
// */
//void APP_GetNextReadyFrame(AppContext_TypeDef *App_Context_Ptr)
//{
//  ValidationContext_TypeDef* Validation_Ctx_Ptr=&App_Context_Ptr->Test_ContextPtr->ValidationContext;
//  uint8_t* cam_capture_buff = App_Context_Ptr->Camera_ContextPtr->camera_capture_buffer;
//#if MEMORY_SCHEME != FULL_INTERNAL_MEM_OPT
//  uint8_t* cam_frame_buff = App_Context_Ptr->Camera_ContextPtr->camera_frame_buffer;
//#endif
//
//  if((App_Context_Ptr->Operating_Mode == VALID) && (Validation_Ctx_Ptr->validation_completed ==0))
//  {
//    TEST_GetNextValidationInput(App_Context_Ptr->Test_ContextPtr,
//                                cam_capture_buff);
//  }
//  else if((App_Context_Ptr->Operating_Mode == DUMP) &&
//          (App_Context_Ptr->Test_ContextPtr->DumpContext.Dump_FrameSource == SDCARD_FILE))
//  {
//    TEST_GetNextDumpInput(App_Context_Ptr->Test_ContextPtr,
//                          cam_capture_buff);
//  }
//  else
//  {
//    /* Wait for current camera acquisition to complete*/
//    while(App_Context_Ptr->Camera_ContextPtr->new_frame_ready == 0);
//  }
//
//  /* DMA2D transfer from camera frame buffer to LCD write buffer */
//  CameraCaptureBuff2LcdBuff_Copy(App_Context_Ptr);
//
//#if MEMORY_SCHEME != FULL_INTERNAL_MEM_OPT
//  if(App_Context_Ptr->Operating_Mode != VALID)
//  {
//    /****Coherency purpose: invalidate the camera_capture_buffer area in L1 D-Cache before CPU reading****/
//    UTILS_DCache_Coherency_Maintenance((void*)cam_capture_buff,
//                                       CAM_FRAME_BUFFER_SIZE, INVALIDATE);
//  }
//
//  /****Copy the camera_capture_buff buffer content onto the camera_frame_buff buffer so to release the camera capture buffer before triggering the subsequent camera frame capture****/
//  for(uint32_t i=0;i<CAM_RES_WIDTH*CAM_RES_HEIGHT*RGB_565_BPP; i++)
//  {
//    *((uint8_t *)(cam_frame_buff + i)) = *((uint8_t *)(cam_capture_buff + i));
//  }
//#endif
//}
//
///**
// * @brief Start the camera acquisition of the subsequent frame
// * @param App_Context_Ptr pointer to application context
// */
//void APP_StartNewFrameAcquisition(AppContext_TypeDef *App_Context_Ptr)
//{
//  if(App_Context_Ptr->Operating_Mode == NOMINAL ||
//     App_Context_Ptr->Operating_Mode == CAPTURE ||
//       ((App_Context_Ptr->Operating_Mode == DUMP)&& (App_Context_Ptr->Test_ContextPtr->DumpContext.Dump_FrameSource != SDCARD_FILE)))
//  {
// //   __disable_irq();
//    App_Context_Ptr->Camera_ContextPtr->vsync_it=0;
//
//    App_Context_Ptr->Utils_ContextPtr->ExecTimingContext.tcapturestart1=HAL_GetTick();
//
//    App_Context_Ptr->Camera_ContextPtr->new_frame_ready = 0;
//
//    /***Resume the camera capture in NOMINAL mode****/
//    BSP_CAMERA_Resume(0);
//  //  __enable_irq();
//  }
//}

/**
 * @brief Run preprocessing stages on captured frame
 * @param App_Context_Ptr pointer to application context
 */
void APP_FramePreprocess(AppContext_TypeDef *App_Context_Ptr)	//对捕获的帧进行预处理
{
  /*Call a fct in charge of executing the sequence of preprocessing steps*/
	//调用一个负责执行预处理步骤序列的函数
  Run_Preprocessing(App_Context_Ptr);
}

///**
// * @brief Run neural network inference on preprocessed captured frame
// * @param App_Context_Ptr pointer to application context
// */
//void APP_NetworkInference(AppContext_TypeDef *App_Context_Ptr)
//{
//  uint32_t tinf_start;
//  uint32_t tinf_stop;
//  TestRunContext_TypeDef* TestRunCtxt_Ptr=&App_Context_Ptr->Test_ContextPtr->TestRunContext;
//
//  TestRunCtxt_Ptr->src_buff_addr=(void *)(App_Context_Ptr->Ai_ContextPtr->nn_input_buffer);
//  TestRunCtxt_Ptr->src_buff_name=Test_buffer_names[3];
//  TestRunCtxt_Ptr->src_width_size=AI_NETWORK_WIDTH;
//  TestRunCtxt_Ptr->src_height_size=AI_NETWORK_HEIGHT;
//  TestRunCtxt_Ptr->src_bpp=IMAGE_BPP_RGB888; /* Not used. src_size used for RAW format */
//  TestRunCtxt_Ptr->src_size=AI_NET_INPUT_SIZE_BYTES;
//  TestRunCtxt_Ptr->PerformCapture=1;
//  TestRunCtxt_Ptr->DumpFormat=DATA_FORMAT_RAW;
//  TestRunCtxt_Ptr->rb_swap=1;
//  TEST_Run(App_Context_Ptr->Test_ContextPtr, App_Context_Ptr->Operating_Mode);
//
//  tinf_start=UTILS_GetTimeStamp(App_Context_Ptr->Utils_ContextPtr);
//
//  /***********************************/
//  /*********Run NN inference**********/
//  /***********************************/
//  AI_Run(App_Context_Ptr->Ai_ContextPtr);
//
//  tinf_stop=UTILS_GetTimeStamp(App_Context_Ptr->Utils_ContextPtr);
//
//  App_Context_Ptr->Utils_ContextPtr->ExecTimingContext.operation_exec_time[FRAME_INFERENCE]=tinf_stop-tinf_start;
//}
//
///**
// * @brief Run post-processing operation
// * @param App_Context_Ptr pointer to application context
// */
//void APP_Postprocess(AppContext_TypeDef *App_Context_Ptr)
//{
//  TestRunContext_TypeDef* TestRunCtxt_Ptr=&App_Context_Ptr->Test_ContextPtr->TestRunContext;
//
//  /*** At that point, it is recommended to wait until current camera acquisition is completed before proceeding
//  *** before proceeding in order to avoid bottleneck at FMC slave (btw LTDC/DMA2D and DMA).
//  ***/
//  while(App_Context_Ptr->Camera_ContextPtr->new_frame_ready == 0);
//
//  UTILS_Compute_ExecutionTiming(App_Context_Ptr->Utils_ContextPtr);
//
//  /**NN ouput dequantization if required**/
//  AI_Output_Dequantize(App_Context_Ptr->Ai_ContextPtr);
//
//  TestRunCtxt_Ptr->src_buff_addr=(void *)(App_Context_Ptr->Ai_ContextPtr->nn_output_buffer);
//  TestRunCtxt_Ptr->src_buff_name=Test_buffer_names[4];
//  TestRunCtxt_Ptr->src_width_size=0;
//  TestRunCtxt_Ptr->src_height_size=0;
//  TestRunCtxt_Ptr->src_size=/*AI_NETWORK_OUT_1_SIZE*/AI_NETWORK_OUT_1_SIZE_BYTES;//so to make it compatible w/ case where dump is in SDRAM
//  TestRunCtxt_Ptr->PerformCapture=0;
//  TestRunCtxt_Ptr->DumpFormat=DATA_FORMAT_TXT;
//  TestRunCtxt_Ptr->rb_swap=0;
//  TEST_Run(App_Context_Ptr->Test_ContextPtr, App_Context_Ptr->Operating_Mode);
//
//  /**Perform ranking**/
//  for (int i = 0; i < NN_OUTPUT_CLASS_NUMBER; i++)
//  {
//    App_Context_Ptr->ranking[i] = i;
//  }
//
//  UTILS_Bubblesort((float*)(App_Context_Ptr->Ai_ContextPtr->nn_output_buffer), App_Context_Ptr->ranking, NN_OUTPUT_CLASS_NUMBER);
//
//  App_Context_Ptr->nn_top1_output_class_name=NN_OUTPUT_CLASS_LIST[App_Context_Ptr->ranking[0]];
//  App_Context_Ptr->nn_top1_output_class_proba=*((float*)(App_Context_Ptr->Ai_ContextPtr->nn_output_buffer)+0);
//
//  if(App_Context_Ptr->Operating_Mode == NOMINAL)
//  {
//    /*Display Neural Network output classification results as well as other performances informations*/
//    App_Output_Display(App_Context_Ptr);
//
//    BSP_LED_Toggle(LED_BLUE);
//  }
//  else
//  {
//    TEST_PostProcess(App_Context_Ptr->Test_ContextPtr);
//  }
//}




static void Frame_Reponse_Callback(void *parameter)
{
    APP_Init(&App_Context);   //初始化桢处理程序上下文结构

    while(1)
    {
        APP_FramePreprocess(&App_Context);  //运行帧预处理
    }
}

void Frame_Reponse(void)
{
    frame_response_t = rt_thread_create("frame_response_t", Frame_Reponse_Callback, RT_NULL, 2048, 10, 10);
    if(frame_response_t!=RT_NULL){
        rt_thread_startup(frame_response_t);
        LOG_I("桢处理响应函数初始化成功");
//        LOG_I("Frame_Reponse Init Success");
    }
}

