#include "AI.h"

#define DBG_TAG "AI"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

int result[6];

/* 模型句柄 */
static ai_handle network = AI_HANDLE_NULL;

/* 神经网络中间值 (激活缓冲区 ) */
AI_ALIGNED(32)
static ai_u8 activations[AI_NETWORK_DATA_ACTIVATIONS_SIZE];

/* 输入张量的缓冲区 */
AI_ALIGNED(32)
static ai_float in_data[AI_NETWORK_IN_1_SIZE];
//static ai_u8 in_data[AI_NETWORK_IN_1_SIZE_BYTES];

/* 输出张量的缓冲区 */
AI_ALIGNED(32)
static ai_float out_data[AI_NETWORK_OUT_1_SIZE];
//static ai_u8 out_data[AI_NETWORK_OUT_1_SIZE_BYTES];

/* 模型输入输出张量指针 */
static ai_buffer *ai_input;
static ai_buffer *ai_output;

rt_thread_t AI_response_t;  //AI任务结构体
rt_mutex_t lock;            //互斥量
//rt_sem_t ai_sem;            //AI信号量
//extern rt_sem_t shot_sem;   //拍照完成信号量

extern rt_uint8_t Target_Subimage_0[STANDARD_IMAGE_HEIGHT * STANDARD_IMAGE_WIDTH * STANDARD_IMAGE_COLOUR];
extern rt_uint8_t Target_Subimage_1[STANDARD_IMAGE_HEIGHT * STANDARD_IMAGE_WIDTH * STANDARD_IMAGE_COLOUR];
extern rt_uint8_t Target_Subimage_2[STANDARD_IMAGE_HEIGHT * STANDARD_IMAGE_WIDTH * STANDARD_IMAGE_COLOUR];
extern rt_uint8_t Target_Subimage_3[STANDARD_IMAGE_HEIGHT * STANDARD_IMAGE_WIDTH * STANDARD_IMAGE_COLOUR];
extern rt_uint8_t Target_Subimage_4[STANDARD_IMAGE_HEIGHT * STANDARD_IMAGE_WIDTH * STANDARD_IMAGE_COLOUR];
extern rt_uint8_t Target_Subimage_5[STANDARD_IMAGE_HEIGHT * STANDARD_IMAGE_WIDTH * STANDARD_IMAGE_COLOUR];

/*
 * 初始化推理
 */
int AI_Init(void) {

  ai_error err;
  /* 创建和初始化模型 */
  const ai_handle acts[] = { activations };
  err = ai_network_create_and_init(&network, acts, NULL);

  if (err.type != AI_ERROR_NONE) {
        LOG_E("Error: could not create NN instance or initialize\r\n");
        while(1);
  };

  /* 获取指向模型的输入/输出张量的指针 */
  ai_input = ai_network_inputs_get(network, NULL);
  ai_output = ai_network_outputs_get(network, NULL);

  return 0;
}

/*
 * 运行推理
 */
int AI_Run(const void *in_data, void *out_data) {

  ai_i32 n_batch;
  ai_error err;

  /* 1 - 更新输入和输出缓冲区的数据指针 */
  ai_input[0].data = AI_HANDLE_PTR(in_data);
  ai_output[0].data = AI_HANDLE_PTR(out_data);

  /* 2 - 使用神经网络进行推理 */
  if(rt_mutex_take(lock, RT_WAITING_FOREVER) != RT_EOK)       //上锁
          LOG_E("获取互斥量失败\r\n");
  n_batch = ai_network_run(network, &ai_input[0], &ai_output[0]);
  rt_mutex_release(lock);                                     //解锁

  if (n_batch != 1) {
      err = ai_network_get_error(network);
      LOG_I("Error: could not run inference because of %d\r\n",
              err.type);
  };

  return 0;
}

void AI_Reponse_Callback(void *parameter)
{

    lock        = rt_mutex_create("mutex_AI", RT_IPC_FLAG_FIFO);        //创建互斥锁
    if(lock     == RT_NULL)
        LOG_E("无法创建互斥锁");

//    ai_sem = rt_sem_create("ai_sem", RT_NULL, RT_IPC_FLAG_FIFO);       //创建AI信号量 先进先出模式
//    if (ai_sem == RT_NULL)
//        LOG_E("Failed to create a AI semaphore");

    MX_CRC_Init();

    /* 启用CRC IP时钟以使用神经网络运行时库 */
    __HAL_RCC_CRC_CLK_ENABLE();

    /* 初始化推理 */
    AI_Init();

//    while(ret == RT_EOK)
    while(1)
    {
//        rt_sem_take(shot_sem, RT_WAITING_FOREVER);                      //接收拍照完成信号量 永久等待
//        // 填充输入缓冲区 */
//        for(uint32_t i = 0; i < AI_NETWORK_IN_1_SIZE; i++)
//            ((ai_float *)in_data)[i] = Target_Subimage_0[i];
//
//        /* 运行推理 */
//        AI_Run(in_data, out_data);
//
//        /* 读取神经网络的输出 */
//        for(int i = 0; i < AI_NETWORK_OUT_1_SIZE; i++){
//            if(((float *)out_data)[i] > 0.8)
//                result[0] = i;
////                LOG_I("number: %d", i);
//        }

        // 填充输入缓冲区 */
        for(uint32_t i = 0; i < AI_NETWORK_IN_1_SIZE; i++)
            ((ai_float *)in_data)[i] = Target_Subimage_1[i];

        /* 运行推理 */
        AI_Run(in_data, out_data);

        /* 读取神经网络的输出 */
        for(int i = 0; i < AI_NETWORK_OUT_1_SIZE; i++){
            if(((float *)out_data)[i] > 0.8)
                result[1] = i;
//                LOG_I("number: %d", i);
        }

        // 填充输入缓冲区 */
        for(uint32_t i = 0; i < AI_NETWORK_IN_1_SIZE; i++)
            ((ai_float *)in_data)[i] = Target_Subimage_2[i];

        /* 运行推理 */
        AI_Run(in_data, out_data);

        /* 读取神经网络的输出 */
        for(int i = 0; i < AI_NETWORK_OUT_1_SIZE; i++){
            if(((float *)out_data)[i] > 0.8)
                result[2] = i;
//                LOG_I("number: %d", i);
        }

        // 填充输入缓冲区 */
        for(uint32_t i = 0; i < AI_NETWORK_IN_1_SIZE; i++)
            ((ai_float *)in_data)[i] = Target_Subimage_3[i];

        /* 运行推理 */
        AI_Run(in_data, out_data);

        /* 读取神经网络的输出 */
        for(int i = 0; i < AI_NETWORK_OUT_1_SIZE; i++){
            if(((float *)out_data)[i] > 0.8)
                result[3] = i;
//                LOG_I("number: %d", i);
        }

        // 填充输入缓冲区 */
        for(uint32_t i = 0; i < AI_NETWORK_IN_1_SIZE; i++)
            ((ai_float *)in_data)[i] = Target_Subimage_4[i];

        /* 运行推理 */
        AI_Run(in_data, out_data);

        /* 读取神经网络的输出 */
        for(int i = 0; i < AI_NETWORK_OUT_1_SIZE; i++){
            if(((float *)out_data)[i] > 0.8)
                result[4] = i;
//                LOG_I("number: %d", i);
        }

        // 填充输入缓冲区 */
        for(uint32_t i = 0; i < AI_NETWORK_IN_1_SIZE; i++)
            ((ai_float *)in_data)[i] = Target_Subimage_5[i];

        /* 运行推理 */
        AI_Run(in_data, out_data);

        /* 读取神经网络的输出 */
        for(int i = 0; i < AI_NETWORK_OUT_1_SIZE; i++){
            if(((float *)out_data)[i] > 0.8)
                result[5] = i;
//                LOG_I("number: %d", i);
        }


//        LOG_I("number: %d, %d, %d, %d, %d, %d", result[0], result[1], result[2], result[3], result[4], result[5]);
        LOG_I("number: %d, %d, %d, %d, %d, xx", result[5], result[4], result[3], result[2], result[1]);


        rt_thread_mdelay(100);
//        rt_sem_release(ai_sem);   //释放AI信号量
    }
}

void AI_Reponse(void)
{
    AI_response_t = rt_thread_create("AI_response_t", AI_Reponse_Callback, RT_NULL, 4096, 10, 10);
//    AI_response_t = rt_thread_create("AI_response_t", AI_Reponse_Callback, RT_NULL, 2048, 10, 10);
    if(AI_response_t!=RT_NULL){
        rt_thread_startup(AI_response_t);
        LOG_I("AI函数初始化成功");
    }
}
