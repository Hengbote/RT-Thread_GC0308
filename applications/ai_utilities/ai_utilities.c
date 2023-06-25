/* Includes ------------------------------------------------------------------*/
#include "ai_utilities.h"

/** @addtogroup Middlewares
  * @{
  */

/** @addtogroup STM32_AI_Utilities
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Global variables ----------------------------------------------------------*/
//extern rt_uint16_t JpegBuffer[INPUT_HEIGHT][INPUT_WIDTH];               //原始图片缓冲
/* Functions Definition ------------------------------------------------------*/
/**
* @brief Initializes the application data memory layout
* @param  Pointer to Application context
*/
void Init_DataMemoryLayout(AppContext_TypeDef *App_Context_Ptr) //初始化应用程序数据的内存布局
{
#ifdef OBJECTDETECT
  PeopleContext_TypeDef *AIContext = App_Context_Ptr->People_ContextPtr;
#else
  AiContext_TypeDef *AIContext = App_Context_Ptr->Ai_ContextPtr;
#endif /* OBJECTDETECT */

#if MEMORY_SCHEME == FULL_INTERNAL_MEM_OPT
  /* Bottom align camera_frame_buff buffer and resize_output_buff buffer */
  #define  RESIZE_OUTPUT_BUFFER_OFFSET (CAM_FRAME_BUFFER_SIZE - RESIZE_OUTPUT_BUFFER_SIZE)  //尺寸调整后输出缓冲区偏移(摄像头帧缓冲区的大小-尺寸调整后输出缓冲区大小)
//  App_Context_Ptr->Camera_ContextPtr->camera_capture_buffer = ai_fp_global_memory;          //相机捕获的图像数据地址,地址指向AI全局内存数组地址
//  App_Context_Ptr->Camera_ContextPtr->camera_frame_buffer = ai_fp_global_memory;            //相机帧处理或者转换后数据地址,地址指向AI全局内存数组地址
  App_Context_Ptr->Preproc_ContextPtr->Pfc_Dst_Img.data = ai_fp_global_memory;              //像素格式转换的目标图像(转换后)地址, 地址指向AI全局内存数组地址
  App_Context_Ptr->Preproc_ContextPtr->Resize_Dst_Img.data = ai_fp_global_memory + RESIZE_OUTPUT_BUFFER_OFFSET; //激活缓冲区地址,地址指向(AI全局内存数组地址+尺寸调整后输出缓冲区偏移)
  AIContext->activation_buffer = ai_fp_global_memory;                                       //激活缓冲区地址,地址指向AI全局内存数组地址
  #ifdef AI_NETWORK_INPUTS_IN_ACTIVATIONS
  AIContext->nn_input_buffer = NULL;
  #else
  AIContext->nn_input_buffer = ai_fp_global_memory + AI_ACTIVATION_BUFFER_SIZE;             //神经网络输入缓冲区地址, 地址指向(AI全局内存数组地址+AI激活缓冲区大小)
  #endif /* AI_NETWORK_INPUTS_IN_ACTIVATIONS */
#elif MEMORY_SCHEME != FULL_INTERNAL_MEM_OPT
  App_Context_Ptr->Camera_ContextPtr->camera_capture_buffer = ai_fp_global_memory;  //相机捕获的图像数据地址,地址指向AI全局内存数组地址
  App_Context_Ptr->Camera_ContextPtr->camera_frame_buffer = ai_fp_global_memory + CAM_FRAME_BUFFER_SIZE;    //相机帧处理或者转换后数据地址, 地址指向(AI全局内存数组地址+摄像头帧缓冲区的大小)
  #if MEMORY_SCHEME == SPLIT_INT_EXT
  /* Bottom align pfc_output_buff buffer and resize_output_buff buffer */
  #define  RESIZE_OUTPUT_BUFFER_OFFSET (PFC_OUTPUT_BUFFER_SIZE - RESIZE_OUTPUT_BUFFER_SIZE)
  App_Context_Ptr->Preproc_ContextPtr->Pfc_Dst_Img.data = ai_fp_activation_memory;
  App_Context_Ptr->Preproc_ContextPtr->Resize_Dst_Img.data = ai_fp_activation_memory + RESIZE_OUTPUT_BUFFER_OFFSET;
  AIContext->activation_buffer = ai_fp_activation_memory;
  #ifdef AI_NETWORK_INPUTS_IN_ACTIVATIONS
  AIContext->nn_input_buffer = NULL;
  #else
  AIContext->nn_input_buffer = ai_fp_global_memory + CAM_FRAME_BUFFER_SIZE;
  #endif /* AI_NETWORK_INPUTS_IN_ACTIVATIONS */
  #else /* MEMORY_SCHEME == FULL_EXTERNAL */
  /* Bottom camera_frame_buff buffer and resize_output_buff buffer */
  #define  RESIZE_OUTPUT_BUFFER_OFFSET (CAM_FRAME_BUFFER_SIZE - RESIZE_OUTPUT_BUFFER_SIZE)                  //尺寸调整后输出缓冲区偏移(摄像头帧缓冲区的大小-尺寸调整后输出缓冲区大小)
  App_Context_Ptr->Preproc_ContextPtr->Pfc_Dst_Img.data = ai_fp_global_memory + CAM_FRAME_BUFFER_SIZE;      //像素格式转换的目标图像(转换后)地址, 地址指向(AI全局内存数组地址+摄像头帧缓冲区的大小)
  App_Context_Ptr->Preproc_ContextPtr->Resize_Dst_Img.data = ai_fp_global_memory + CAM_FRAME_BUFFER_SIZE + RESIZE_OUTPUT_BUFFER_OFFSET; //尺寸调整的目标图像(转换后)地址, 地址指向(AI全局内存数组地址+摄像头帧缓冲区的大小+尺寸调整后输出缓冲区偏移)
  AIContext->activation_buffer = ai_fp_global_memory + CAM_FRAME_BUFFER_SIZE;                               //激活缓冲区地址,地址指向(AI全局内存数组地址+摄像头帧缓冲区的大小)
  #ifdef AI_NETWORK_INPUTS_IN_ACTIVATIONS
  AIContext->nn_input_buffer = NULL;
  #else
  AIContext->nn_input_buffer = ai_fp_global_memory + CAM_FRAME_BUFFER_SIZE + AI_ACTIVATION_BUFFER_SIZE;     //神经网络输入缓冲区地址, 地址指向(AI全局内存数组地址+摄像头帧缓冲区的大小+AI激活缓冲区大小)
  #endif /* AI_NETWORK_INPUTS_IN_ACTIVATIONS */
  #endif /* MEMORY_SCHEME == SPLIT_INT_EXT */
#else
  #error "Please check definition of MEMORY_SCHEME define"
#endif /* MEMORY_SCHEME == FULL_INTERNAL_MEM_OPT */
}

/**
* @brief  Run preprocessing stages on captured frame
* @param  App context ptr
* @retval None
*/
//在捕获的帧上运行预处理阶段
void Run_Preprocessing(AppContext_TypeDef *App_Context_Ptr)
{
//  TestRunContext_TypeDef* TestRunCtxt_Ptr=&App_Context_Ptr->Test_ContextPtr->TestRunContext;    //初始化描述测试运行的详细信息结构体,包括源图像缓冲区的信息、数据格式等
  PreprocContext_TypeDef* PreprocCtxt_Ptr=App_Context_Ptr->Preproc_ContextPtr;                  //初始化预处理上下文结构体,用于存储与图像预处理操作有关的状态和信息


  /**********************/
  /****Image resizing****/
  /********图像缩放********/
  /**********************/
//  PreprocCtxt_Ptr->Resize_Src_Img.data=App_Context_Ptr->Camera_ContextPtr->camera_frame_buffer;     //指向需要进行尺寸调整的源图像(转换前)像素数据的指针 = 指向用于存储相机帧处理或者转换后数据的指针
//  PreprocCtxt_Ptr->Resize_Src_Img.data=(uint8_t *)JpegBuffer;     //指向需要进行尺寸调整的源图像(转换前)像素数据的指针 = 指向原始图片缓冲区
  PreprocCtxt_Ptr->Resize_Src_Img.w=CAM_RES_WIDTH;          //需要进行尺寸调整的源图像(转换前)的宽度 = 相机分辨率宽 640
  PreprocCtxt_Ptr->Resize_Src_Img.h=CAM_RES_HEIGHT;         //需要进行尺寸调整的源图像(转换前)的高度 = 相机分辨率高 480
  PreprocCtxt_Ptr->Resize_Src_Img.bpp=IMAGE_BPP_RGB565;     //需要进行尺寸调整的源图像(转换前)的格式为RGB565
  PreprocCtxt_Ptr->Resize_Dst_Img.data=App_Context_Ptr->Preproc_ContextPtr->Resize_Dst_Img.data;    //指向尺寸调整的目标图像(转换后)像素数据的指针 = 输入的结构体中指向尺寸调整的目标图像(转换后)像素数据的指针
  PreprocCtxt_Ptr->Resize_Dst_Img.w=AI_NETWORK_WIDTH;       //尺寸调整的目标图像(转换后)的宽度 = AI网络输入宽度
  PreprocCtxt_Ptr->Resize_Dst_Img.h=AI_NETWORK_HEIGHT;      //尺寸调整的目标图像(转换后)的高度 = AI网络输入高度
  PreprocCtxt_Ptr->Resize_Dst_Img.bpp=IMAGE_BPP_RGB565;     //尺寸调整的目标图像(转换后)的格式为RGB565
  PREPROC_ImageResize(App_Context_Ptr->Preproc_ContextPtr); //采用最近邻的方法 进行缩放 输入输入结构体中的存储与预处理图像结构体


  /*************************************/
  /****Image Pixel Format Conversion****/
  /*图像像素格式转换*/
  /*************************************/
  PreprocCtxt_Ptr->Pfc_Src_Img.data=App_Context_Ptr->Preproc_ContextPtr->Resize_Dst_Img.data;   //指向需要进行像素格式转换的源图像(转换前)像素数据的指针 = 指向尺寸调整的目标图像(转换后)像素数据的指针
  PreprocCtxt_Ptr->Pfc_Src_Img.w=AI_NETWORK_WIDTH;      //需要进行像素格式转换的源图像(转换前)的宽度 = AI网络宽度 224
  PreprocCtxt_Ptr->Pfc_Src_Img.h=AI_NETWORK_HEIGHT;     //需要进行像素格式转换的源图像(转换前)的高度 = AI网络高度 224
  PreprocCtxt_Ptr->Pfc_Src_Img.bpp=IMAGE_BPP_RGB565;    //需要进行像素格式转换的源图像(转换前)的格式为RGB565
  PreprocCtxt_Ptr->Pfc_Dst_Img.data=App_Context_Ptr->Preproc_ContextPtr->Pfc_Dst_Img.data;      //指向像素格式转换的目标图像(转换后)像素数据的指针 = 输入的结构体中指向像素格式转换的目标图像(转换后)像素数据的指针
  PreprocCtxt_Ptr->Pfc_Dst_Img.w=AI_NETWORK_WIDTH;      //像素格式转换的目标图像(转换后)的宽度 = AI网络宽度 224
  PreprocCtxt_Ptr->Pfc_Dst_Img.h=AI_NETWORK_HEIGHT;     //像素格式转换的目标图像(转换后)的高度 = AI网络高度 224
  PreprocCtxt_Ptr->Dma2dcfg.x=0;                        //
  PreprocCtxt_Ptr->Dma2dcfg.y=0;                        //
  PreprocCtxt_Ptr->Dma2dcfg.rowStride=AI_NETWORK_WIDTH; //
  PreprocCtxt_Ptr->red_blue_swap=1;                     //
  PREPROC_PixelFormatConversion(App_Context_Ptr->Preproc_ContextPtr);



  /***********************************************************/
  /*********Pixel value convertion and normalisation**********/
  /*像素值转换和标准化*/
  /***********************************************************/
  AI_PixelValueConversion(App_Context_Ptr->Ai_ContextPtr, (void*)(App_Context_Ptr->Preproc_ContextPtr->Pfc_Dst_Img.data));
}

bool Image_CheckResizeMemoryLayout(image_t *src_img, image_t *dst_img)  //图像处理_判断内存中的布局是否需要进行反向处理
{
  uint32_t src_size = STM32Ipl_ImageDataSize(src_img);  //计算需要进行尺寸调整的源图像(转换前)数据缓冲区大小（以字节为单位）
  uint32_t dst_size = STM32Ipl_ImageDataSize(dst_img);  //计算尺寸调整的目标图像(转换后)数据缓冲区大小（以字节为单位）
  uint32_t src_start = (uint32_t)src_img->data;         //获取指向需要进行尺寸调整的源图像(转换前)像素数据指针地址的起始位
  uint32_t dst_start = (uint32_t)dst_img->data;         //获取指向尺寸调整的目标图像(转换后)像素数据指针地址的起始位
  uint32_t src_end = src_start + src_size - 1;          //计算指向需要进行尺寸调整的源图像(转换前)像素数据指针地址的结束位
  uint32_t dst_end = dst_start + dst_size - 1;          //计算指向尺寸调整的目标图像(转换后)像素数据指针地址的结束位
  bool reverse;     //标识是否需要进行反向处理

  if ((src_end > dst_start) && (dst_end >= src_end))    //判断源图像和目标图像的内存区域是否重叠
  {
    reverse = true;
  }
  else
  {
    reverse = false; /* ((src_start >= dst_start) || (src_end <= dst_start)) */
  }

  return reverse;
}

bool Image_CheckPfcMemoryLayout(image_t *src_img, image_t *dst_img)
{
  uint32_t src_size = STM32Ipl_ImageDataSize(src_img);
  uint32_t dst_size = STM32Ipl_ImageDataSize(dst_img);
  uint32_t src_start = (uint32_t)src_img->data;
  uint32_t dst_start = (uint32_t)dst_img->data;
  uint32_t src_end = src_start + src_size - 1;
  uint32_t dst_end = dst_start + dst_size - 1;

  if (((dst_size - src_size) <  dst_size / 3))
  {
    while (1);
  }
  else
  {
    if (dst_end <= src_end)
    {
      return true;
    }
    else if (dst_start >= src_start)
    {
      return false;
    }
    else
    {
      while (1);
    }
  }
}
