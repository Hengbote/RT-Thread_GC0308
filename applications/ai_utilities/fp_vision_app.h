/**
 ******************************************************************************
 * @file    fp_vision_app.h
 * @author  MCD Application Team
 * @brief   Header for fp_vision_app.c module
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
#ifndef __FP_VISION_APP_H
#define __FP_VISION_APP_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "fp_vision_global.h"
#include "fp_vision_ai.h"
//#include "fp_vision_utils.h"
//#include "fp_vision_test.h"
#include "fp_vision_camera.h"
//#include "fp_vision_display.h"
#include "fp_vision_preproc.h"
//#include "stm32h747i_discovery_sdram.h"
//#include "stm32h747i_discovery_sd.h"
//#include "stm32h747i_discovery_qspi.h"

#ifndef MEMORY_SCHEME       //使用了但是不知道在那里添加的,目前只能加在这里
 #define MEMORY_SCHEME       FULL_INTERNAL_MEM_OPT   /*列程为FULL_EXTERNAL*/
#endif

/* Exported types ------------------------------------------------------------*/
typedef struct
{
  /**General**/
  AppOperatingMode_TypeDef Operating_Mode;/*Application's operating mode*/  //应用程序的操作模式
  uint32_t run_loop;    //这个字段可能是一个标志，用于表示应用程序是否需要继续运行

  /**NN Output**/
  uint32_t nn_inference_time;               //神经网络推理的时间
  char const* nn_top1_output_class_name;    //神经网络推理后的最高概率类别的名称
  float nn_top1_output_class_proba;         //神经网络推理后的最高概率类别的概率
  int ranking[NN_OUTPUT_CLASS_NUMBER];      //神经网络推理的所有输出类别的排名数组

//  /**Camera context**/
//  CameraContext_TypeDef* Camera_ContextPtr;     //是一个相机上下文结构，用于存储与相机操作有关的状态和信息
//
  /**Pre-Processing context**/
  PreprocContext_TypeDef* Preproc_ContextPtr;   //存储与预处理图像的状态和信息

//  /**Display context**/
//  DisplayContext_TypeDef* Display_ContextPtr;   //有关LCD显示的状态和数据
//
//  /**Utility context**/
//  UtilsContext_TypeDef* Utils_ContextPtr;       //桢操作执行时间 遥感(操纵杆)信息 相机和lcd亮度设置
//
//  /**Test context**/
//  TestContext_TypeDef* Test_ContextPtr;         //测试上下文结构体  (内存转储,捕获的图像信息,模型验证相关信息,串口通信,图像测试运行,随机数,运行状态标记)

  /**AI NN context**/
  AiContext_TypeDef* Ai_ContextPtr;             //存储和管理神经网络的输入,输出,激活,查找表,标准化尺度因子,标准化零点
}AppContext_TypeDef;    //相机,图像,LCD,桢操作执行时间,测试上下文结构体,神经网络

/* Exported macros -----------------------------------------------------------*/
#if WEIGHT_QSPI == 1 && WEIGHT_EXEC_EXTRAM == 1
 #define QUADSPI_EXT_FLASH  0x90000000
#endif

/* Exported constants --------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
extern AppContext_TypeDef App_Context;
extern uint8_t ai_fp_global_memory[];
#if MEMORY_SCHEME == SPLIT_INT_EXT
 extern uint8_t ai_fp_activation_memory[];
#endif
//extern const char* output_labels[];

#if WEIGHT_QSPI == 1
 extern uint8_t weights_table_sdram[];
#endif

 /********************************/
 /***Onboard validation defines***/
 /********************************/
 /*number of files per class directory on SDCard (in the onboard validation context)*/
#define ONBOARD_VALID_NUM_FILE_PER_DIR 100

/*******************/
/***Misc defines****/
/*******************/
/* Check allowed combination of memory layout scheme and camera resolution*/
#if CAMERA_CAPTURE_RES == VGA_640_480_RES
#if MEMORY_SCHEME == FULL_INTERNAL_FPS_OPT || MEMORY_SCHEME == FULL_INTERNAL_MEM_OPT
#error Memory scheme not allowed in this application
#endif
#elif CAMERA_CAPTURE_RES == QVGA_320_240_RES
 #if MEMORY_SCHEME == FULL_EXTERNAL || MEMORY_SCHEME == SPLIT_INT_EXT
  #error Memory scheme not allowed in this application
 #endif
#endif

#define WELCOME_MSG_0            "Food image classification"
#define WELCOME_MSG_1            "Float model"
#define WELCOME_MSG_2            "Standard version"

#if CAMERA_CAPTURE_RES == VGA_640_480_RES
 #define WELCOME_MSG_3 "VGA"
#elif CAMERA_CAPTURE_RES == QVGA_320_240_RES
 #define WELCOME_MSG_3 "QVGA"
#endif

#if MEMORY_SCHEME == FULL_EXTERNAL
 #define WELCOME_MSG_4 "Full external"           /*完全外部(如SD卡)*/
#elif MEMORY_SCHEME == SPLIT_INT_EXT
 #define WELCOME_MSG_4 "Split ext/int"           /*内部与外部存储的混合使用*/
#elif MEMORY_SCHEME == FULL_INTERNAL_MEM_OPT
 #define WELCOME_MSG_4 "Full int Memory optim"   /*完全内部的，并且优化了内存使用*/
#elif MEMORY_SCHEME == FULL_INTERNAL_FPS_OPT
 #define WELCOME_MSG_4 "Full int FPS optim"      /*完全内部的，并且优化了帧率*/
#else
 #error Please check definition of MEMORY_SCHEME define
#endif


#if WEIGHT_QSPI == 1 /*&& WEIGHT_QSPI_PROGED == 1*/ && WEIGHT_EXEC_EXTRAM == 0
 #define WELCOME_MSG_5            "Weight/Bias in external QSPI flash"
#elif WEIGHT_QSPI == 1 /*&& WEIGHT_QSPI_PROGED == 1*/ && WEIGHT_EXEC_EXTRAM == 1
 #define WELCOME_MSG_5            "Weight/Bias in external SDRAM"
#elif WEIGHT_QSPI == 0
 #define WELCOME_MSG_5            "Weight/Bias in internal flash"
#endif

#define NN_GOOD_RES 70
#define NN_BAD_RES 55

#define RGB_565_BPP 2
#define RGB_888_BPP 3

#define xstr(s) str(s)
#define str(s) #s

/**************************/
/***LCD related defines****/
/**************************/
#define DISPLAY_INFER_TIME_LINE 16
#define DISPLAY_IMG_PREPROCESS_LINE (DISPLAY_INFER_TIME_LINE + 1)
#define DISPLAY_FPS_LINE (DISPLAY_INFER_TIME_LINE + 2)
#define DISPLAY_ACQU_MODE_LINE 19
#define DISPLAY_TOP_N_LAST_LINE (DISPLAY_INFER_TIME_LINE - 4)

///*****************************/
///***Buffers size definition***/
///*****************************/
#define CAM_FRAME_BUFFER_SIZE (CAM_RES_WIDTH * CAM_RES_HEIGHT * RGB_565_BPP)    //摄像头帧缓冲区的大小(宽*高*像素格式)

#define PFC_OUTPUT_BUFFER_SIZE AI_NET_INPUT_SIZE
#define RESIZE_OUTPUT_BUFFER_SIZE (AI_NETWORK_WIDTH *AI_NETWORK_HEIGHT * RGB_565_BPP)   //尺寸调整后输出缓冲区大小(AI网络标准宽*AI网络标准高*像素格式)
  
#define AI_INPUT_BUFFER_SIZE AI_NET_INPUT_SIZE_BYTES        //AI输入缓冲区大小
#define AI_ACTIVATION_BUFFER_SIZE AI_ACTIVATION_SIZE_BYTES  //AI激活缓冲区大小 激活缓冲区是神经网络中用于存储中间计算结果的内存区域

/* Exported functions ------------------------------------------------------- */
void APP_Init(AppContext_TypeDef *);
void APP_Deinit(AppContext_TypeDef *);
void APP_GetNextReadyFrame(AppContext_TypeDef *);
void APP_StartNewFrameAcquisition(AppContext_TypeDef *);
void APP_FramePreprocess(AppContext_TypeDef *);
void APP_NetworkInference(AppContext_TypeDef *);
void APP_Postprocess(AppContext_TypeDef *);
void APP_Context_Init(AppContext_TypeDef *);

void Frame_Reponse(void);

#ifdef __cplusplus
}
#endif

#endif /*__FP_VISION_APP_H */

