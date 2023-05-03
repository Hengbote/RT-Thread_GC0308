#include <rtthread.h>
#include <rtdevice.h>
#include "GC0308.h"
#include "GC0308_config.h"
#include "i2c.h"

#define DBG_TAG "GC0308"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

rt_uint16_t JpegBuffer[INPUT_WIDTH][INPUT_HEIGHT];  //图片缓冲
extern DCMI_HandleTypeDef hdcmi;
extern DMA_HandleTypeDef handle_GPDMA1_Channel0;
extern Camera_Structure camera_device_t;        //摄像头设备
rt_thread_t camera_response_t;                  //摄像头任务结构体
struct rt_semaphore dcmi_sem;            //DCMI帧事件中断 回调函数信号量
//static struct rt_semaphore dcmi_sem;            //DCMI帧事件中断 回调函数信号量

const resolution_info_t resolution[FRAMESIZE_INVALID] = {
    {   96,   96, ASPECT_RATIO_1X1   }, /* 96x96 */
    {  160,  120, ASPECT_RATIO_4X3   }, /* QQVGA */
    {  176,  144, ASPECT_RATIO_5X4   }, /* QCIF  */
    {  240,  176, ASPECT_RATIO_4X3   }, /* HQVGA */
    {  240,  240, ASPECT_RATIO_1X1   }, /* 240x240 */
    {  320,  240, ASPECT_RATIO_4X3   }, /* QVGA  */
    {  400,  296, ASPECT_RATIO_4X3   }, /* CIF   */
    {  480,  320, ASPECT_RATIO_3X2   }, /* HVGA  */
    {  640,  480, ASPECT_RATIO_4X3   }, /* VGA   */
};

/*
 * *读取摄像头的PID.
*/
static rt_err_t GC0308_ReadPID(void)
{
    rt_uint8_t pid;

    do
    {
        /*
         * *选择使用第0组的寄存器
         * *清除所有寄存器并将其重置为默认值
         * */
        write_reg_1data(RESET_RELATED, 0xf0);

        /*读取寄存芯片ID*/
        read_reg_1data(0x00, &pid);
        LOG_I("PID = %02x", pid);
    }
    while(pid != GC0308_PID);

    return RT_EOK;
}

static void GC0308_Reset(void)// 传感器复位函数
{
    if(rt_mutex_take(camera_device_t.lock, RT_WAITING_FOREVER) != RT_EOK)       //上锁
        LOG_E("Failed to obtain the mutex\r\n");
    write_regs(gc0308_sensor_default_regs, sizeof(gc0308_sensor_default_regs)/(sizeof(rt_uint8_t) * 2));
    rt_mutex_release(camera_device_t.lock);                                     //解锁
    LOG_I("相机加载默认模式");
//    LOG_I("Camera defaults loaded");

    rt_thread_mdelay(20);
    write_reg_1data(RESET_RELATED, 0x00);   //寄存器选择第0页
#if 1
    set_reg_bits(0x28, 4, 0x07, 1);  //frequency division for esp32, ensure pclk <= 15MHz
#endif
}

static void Set_Pixformat(pixformat_t pixformat)   // 设置像素格式函数
{
    switch (pixformat) {
        case PIXFORMAT_RGB565:
            write_reg_1data(0xfe, 0x00);        //选择第0页寄存器
            set_reg_bits(0x24, 0, 0x0f, 6);     //RGB565
            break;

        case PIXFORMAT_YUV422:
            write_reg_1data(0xfe, 0x00);        //选择第0页寄存器
            set_reg_bits(0x24, 0, 0x0f, 2);     //yuv422 Y Cb Y Cr
            break;

        default:
            LOG_E("unsupport format");
            break;
    }
}

static void Set_Frame_Size(framesize_t framesize) // 设置帧尺寸函数
{
    rt_uint16_t w = resolution[framesize].width;    //指定帧尺寸的宽度
    rt_uint16_t h = resolution[framesize].height;   //指定帧尺寸的高度

    rt_uint16_t col_s = (resolution[FRAMESIZE_640x480_VGA].width - w) / 2;  //起始列
    rt_uint16_t row_s = (resolution[FRAMESIZE_640x480_VGA].height - h) / 2; //起始行

#if CONFIG_GC_SENSOR_SUBSAMPLE_MODE

    struct subsample_cfg {  //子采样配置
        uint16_t ratio_numerator;   //子采样比例的分子
        uint16_t ratio_denominator; //子采样比例的分母

        /*寄存器*/
        uint8_t reg0x54;
        uint8_t reg0x56;
        uint8_t reg0x57;
        uint8_t reg0x58;
        uint8_t reg0x59;
    };
    const struct subsample_cfg subsample_cfgs[] = { // 定义子样本比率
        {84,  420, 0x55, 0x00, 0x00, 0x00, 0x00},//1/5
        {105, 420, 0x44, 0x00, 0x00, 0x00, 0x00},//1/4
        {140, 420, 0x33, 0x00, 0x00, 0x00, 0x00},//1/3
        {210, 420, 0x22, 0x00, 0x00, 0x00, 0x00},//1/2
        {240, 420, 0x77, 0x02, 0x46, 0x02, 0x46},//4/7
        {252, 420, 0x55, 0x02, 0x04, 0x02, 0x04},//3/5
        {280, 420, 0x33, 0x02, 0x00, 0x02, 0x00},//2/3
        {420, 420, 0x11, 0x00, 0x00, 0x00, 0x00},//1/1
    };
    uint16_t win_w = 640;   //窗口宽度
    uint16_t win_h = 480;   //窗口高度
    float ratio;
    const struct subsample_cfg *cfg = NULL; //指向当前使用的子采样配置

    /*策略:尽量保持最大的视角*/
    for (size_t i = 0; i < sizeof(subsample_cfgs) / sizeof(struct subsample_cfg); i++) {    //遍历subsample_cfgs数组中的所有子采样配置
        cfg = &subsample_cfgs[i];

        if ((win_w * cfg->ratio_numerator / cfg->ratio_denominator >= w) &&     //如果 窗口宽度*样本比率>=指定宽度   并且
            (win_h * cfg->ratio_numerator / cfg->ratio_denominator >= h))       //窗口高度*样本比率>=指定高度
        {
            win_w = (w * cfg->ratio_denominator / cfg->ratio_numerator);          //窗口宽度=指定宽度*样本比率的倒数
            win_h = (h * cfg->ratio_denominator / cfg->ratio_numerator);          //窗口高度=指定高度*样本比率的倒数
            row_s = ((resolution[FRAMESIZE_640x480_VGA].height - win_h) / 2);     //起始列=(最大图像高度-窗口高度)/2
            col_s = ((resolution[FRAMESIZE_640x480_VGA].width - win_w) / 2);      //起始行=(最大图像宽度-窗口宽度)/2
//            LOG_I("subsample win:%d*%d", win_w, win_h);
            ratio = (float)cfg->ratio_numerator / (float)cfg->ratio_denominator;
            LOG_I("子窗口大小:%d*%d, 窗口比例:%.2f", win_w, win_h, ratio);
//            LOG_I("subsample window:%d*%d, ratio:%f", win_w, win_h, ratio);
            break;
        }
    }

    write_reg_1data(0xfe, 0x00);            //选择第0页

    write_reg_1data(0x05, H8(row_s));
    write_reg_1data(0x06, L8(row_s));
    write_reg_1data(0x07, H8(col_s));
    write_reg_1data(0x08, L8(col_s));
    write_reg_1data(0x09, H8(win_h + 8));
    write_reg_1data(0x0a, L8(win_h + 8));
    write_reg_1data(0x0b, H8(win_w + 8));
    write_reg_1data(0x0c, L8(win_w + 8));

    write_reg_1data(0xfe, 0x01);            //选择第1页

    set_reg_bits(0x53, 7, 0x01, 1);
    set_reg_bits(0x55, 0, 0x01, 1);
    write_reg_1data(0x54, cfg->reg0x54);
    write_reg_1data(0x56, cfg->reg0x56);
    write_reg_1data(0x57, cfg->reg0x57);
    write_reg_1data(0x58, cfg->reg0x58);
    write_reg_1data(0x59, cfg->reg0x59);    //

    write_reg_1data(0xfe, 0x00);

#elif CONFIG_GC_SENSOR_WINDOWING_MODE
    // 定义寄存器地址和数据数组
    rt_uint8_t reg_data[][2] = {
        {0xfe, 0x00},
        {0xf7, (col_s / 4)},
        {0xf8, (row_s / 4)},
        {0xf9, (col_s + h) / 4},
        {0xfa, (row_s + w) / 4},
        {0x05, H8(row_s)},
        {0x06, L8(row_s)},
        {0x07, H8(col_s)},
        {0x08, L8(col_s)},
        {0x09, H8(h + 8)},
        {0x0a, L8(h + 8)},
        {0x0b, H8(w + 8)},
        {0x0c, L8(w + 8)}
    };
    write_regs(reg_data, sizeof(reg_data)/(sizeof(rt_uint8_t) * 2));
#endif
    LOG_I("设置帧大小: %d*%d", w, h);
//    LOG_I("Set frame size to: %d*%d", w, h);
}

static rt_err_t GC0308_Register_Init(void)  //GC0308寄存器初始化
{
    if(GC0308_ReadPID() == RT_EOK)          //读取摄像头ID
    {
        GC0308_Reset();
        Set_Frame_Size(FRAME_SIZE);
        Set_Pixformat(PIXFORMAT_RGB565);
        return RT_EOK;
    }
    else
    {
        LOG_E("Failed read the camera ID\r\n");
        return -RT_ERROR;
    }
}

/* DCMI接收到一桢数据后产生帧事件中断，调用此回调函数 */
void HAL_DCMI_FrameEventCallback(DCMI_HandleTypeDef *hdcmi)     //帧事件中断 回调函数
{
    rt_sem_release(&dcmi_sem);      //发送信号量
}

/**
  * @brief 这个函数处理GPDMA1通道0全局中断
  */
void GPDMA1_Channel0_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&handle_GPDMA1_Channel0);
}

/**
  * @brief 这个函数处理DCMI/PSSI全局中断
  */
void DCMI_PSSI_IRQHandler(void)
{
  HAL_DCMI_IRQHandler(&hdcmi);
}

void GC0308_Reponse_Callback(void *parameter)
{
    rt_err_t ret = -RT_ERROR;

    MX_GPDMA1_Init();
    MX_DCMI_Init();
    camera_device_t.lock        = rt_mutex_create("mutex_camera", RT_IPC_FLAG_FIFO);                    //创建摄像头互斥锁
    if(camera_device_t.lock     == RT_NULL)
        LOG_E("Cannot create mutex for camera device on '%s'", CAMERA_I2C_BUS_NAME);
    camera_device_t.uart        = rt_device_find(CAMERA_UART_NAME);                                     //寻找摄像头串口输出设备
    if(camera_device_t.uart     == RT_NULL)
        LOG_E("Cannot find camera device on '%s'", CAMERA_UART_NAME);
    camera_device_t.i2c         = (struct rt_i2c_bus_device *)rt_device_find(CAMERA_I2C_BUS_NAME);      //寻找摄像头IIC总线设备
    if(camera_device_t.i2c      == RT_NULL)
        LOG_E("Cannot find camera device on '%s'", CAMERA_I2C_BUS_NAME);
    camera_device_t.dcmi        = &hdcmi;                                                               //获取DCMI设备结构体地址
    if(camera_device_t.dcmi     == RT_NULL)
        LOG_E("Cannot find camera device on DCMI");

    rt_device_open(camera_device_t.uart, RT_DEVICE_OFLAG_WRONLY);                                       //打开摄像头串口输出设备 只读
    rt_err_t result = rt_sem_init(&dcmi_sem, "dcmi_sem", 0, RT_IPC_FLAG_FIFO);                          //初始化帧事件信号量 先进先出模式
    if (result != RT_EOK)
        LOG_E("Failed to initialize the semaphore");

    ret = GC0308_Register_Init();    //GC0308寄存器初始化

    while(ret == RT_EOK)
    {
//        __HAL_DCMI_ENABLE_IT(camera_device_t.dcmi, DCMI_IT_FRAME);  //使用帧中断
//        memset((void *)JpegBuffer,0,pictureBufferLength * 4);       //把接收BUF清空
//        HAL_DCMI_Start_DMA(camera_device_t.dcmi, DCMI_MODE_SNAPSHOT,(rt_uint32_t)JpegBuffer, pictureBufferLength);//启动拍照
//        rt_thread_mdelay(50);
//
//        if(rt_sem_take(&dcmi_sem, RT_WAITING_FOREVER) == RT_EOK)
//        {
//            HAL_DCMI_Suspend(camera_device_t.dcmi);   //拍照完成，挂起DCMI
//            HAL_DCMI_Stop(camera_device_t.dcmi);      //拍照完成，停止DMA传输
//            int pictureLength =pictureBufferLength;
//            while(pictureLength > 0)        //循环计算出接收的JPEG的大小
//            {
//                if(JpegBuffer[pictureLength-1] != 0x00000000)
//                    break;
//                pictureLength--;
//            }
//            pictureLength *= 4;//buf是uint32_t，下面发送是uint8_t,所以长度要*4
//
////            if(rt_mutex_take(camera_device_t.lock, RT_WAITING_FOREVER) != RT_EOK)       //上锁
////                LOG_E("Failed to obtain the mutex\r\n");
//            rt_device_write(camera_device_t.uart, 0, (rt_uint8_t*)JpegBuffer, pictureLength);
////            rt_mutex_release(camera_device_t.lock);                                     //解锁
//        }

        rt_thread_mdelay(10000);
    }
}

void GC0308_Reponse(void)
{
    camera_response_t = rt_thread_create("camera_response_t", GC0308_Reponse_Callback, RT_NULL, 2048, 10, 10);
    if(camera_response_t!=RT_NULL)rt_thread_startup(camera_response_t);
    LOG_I("I2C_Reponse Init Success\r\n");
}

void Take_Picture(int argc, rt_uint8_t *argv[])
{
    __HAL_DCMI_ENABLE_IT(camera_device_t.dcmi, DCMI_IT_FRAME);  //使用帧中断
    rt_memset((void *)JpegBuffer,0,PICTURE_BUFFER_LENGTH * sizeof(rt_uint32_t));  //把接收BUF清空
    //启动拍照    DCMI结构体指针 DCMI捕获模式 目标内存缓冲区地址 要传输的捕获长度
    HAL_DCMI_Start_DMA(camera_device_t.dcmi, DCMI_MODE_SNAPSHOT,(rt_uint32_t)JpegBuffer, PICTURE_BUFFER_LENGTH);    //启动拍照

    if(rt_sem_take(&dcmi_sem, RT_WAITING_FOREVER) == RT_EOK)
    {
        HAL_DCMI_Suspend(camera_device_t.dcmi);   //拍照完成，挂起DCMI
        HAL_DCMI_Stop(camera_device_t.dcmi);      //拍照完成，停止DMA传输
        rt_device_write(camera_device_t.uart, 0, JpegBuffer, PICTURE_BUFFER_LENGTH * sizeof(rt_uint32_t));
//        rt_device_write(camera_device_t.uart, 0, (rt_uint8_t*)JpegBuffer, pictureLength);

    }
}

/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(Take_Picture, Take a picture);
