#include <rtthread.h>
#include <rtdevice.h>
#include "image_recognition.h"
#include "GC0308.h"
#include "i2c.h"

#define BIBYTE_PICTURE_BUFFER_LENGTH 19200  //2字节图像缓冲长度
#define SUBIMAGEWIDTH 14    //子图像宽
#define SUBIMAGEHEIGHT 20   //子图像高
#define INPUTWIDTH 160      //输入图像宽
#define INPUTHEIGHT 120     //输入图像高
/*宏定义_子图像*/
//#define INPUTIMAGE_IN_ROW_SUBIMAGE_NUMBER     (INPUTWIDTH - SUBIMAGEWIDTH + 1)    //子图像列数
//#define INPUTIMAGE_IN_COLUMN_SUBIMAGE_NUMBER  (INPUTHEIGHT - SUBIMAGEHEIGHT + 1)  //子图像行数
/*宏定义_子图像*/

#define DBG_TAG "image_recognition"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

extern rt_uint32_t JpegBuffer[PICTURE_BUFFER_LENGTH];
static rt_uint16_t Binary_JpegBuffer[BIBYTE_PICTURE_BUFFER_LENGTH];

/*宏定义_子图像*/
//rt_uint16_t subImages[SUB_IMAGES_COLS_NUMBER * SUB_IMAGES_ROWS_NUMBER][SUBIMAGEWIDTH * SUBIMAGEHEIGHT];    //子图像
/*宏定义_子图像*/

/*动态分配内存_子图像*/
rt_uint16_t inputImage_in_row_subimage_number = INPUTWIDTH - SUBIMAGEWIDTH + 1;         //输入图像每列中子图像数
rt_uint16_t inputImage_in_column_subimage_number = INPUTHEIGHT - SUBIMAGEHEIGHT + 1;    //输入图像每行中子图像数
rt_uint16_t (**subImages)[SUBIMAGEWIDTH * SUBIMAGEHEIGHT];              //子图像首地址
//subImages = (rt_uint16_t (*)[SUBIMAGEWIDTH * SUBIMAGEHEIGHT])malloc(sizeof(rt_uint16_t) * SUBIMAGEWIDTH * SUBIMAGEHEIGHT * subImages_cols_number * subImages_rows_number);子图像数组
/*动态分配内存_子图像*/

extern Camera_Structure camera_device_t;    //摄像头设备
rt_thread_t rmage_recognition_response_t;   //图像识别任务结构体

static void Binary_Threshold_Rgb565(rt_uint16_t *imageBuffer, framesize_t framesize)    //二值化
{
//    rt_uint16_t w = resolution[framesize].width;    //指定帧尺寸的宽度
//    rt_uint16_t h = resolution[framesize].height;   //指定帧尺寸的高度
    rt_uint32_t size = resolution[framesize].width * resolution[framesize].height;

    for (int i = 0; i < size; i++) {
        rt_uint16_t pixel = imageBuffer[i];

        // 提取RGB分量
        rt_uint8_t red = (pixel & 0xF800) >> 11;
        rt_uint8_t green = (pixel & 0x07E0) >> 5;
        rt_uint8_t blue = (pixel & 0x001F);

        // 二值化处理 96
        if (red + green/2 + blue >= 50)
            Binary_JpegBuffer[i] = 0xFFFF; // 设置为白色
        else
            Binary_JpegBuffer[i] = 0x0000; // 设置为黑色
    }
}

//void Split_Image_Into_Subimages(uint8_t *inputImage, uint8_t subImages[][subImageWidth * subImageHeight])
//{
//    for (int r = 0; r < rows; r++) {    //遍历输入图像的行
//        for (int c = 0; c < cols; c++) {
//            for (int y = 0; y < subImageHeight; y++) {
//                for (int x = 0; x < subImageWidth; x++) {
//                    int inputIdx = (r + y) * inputWidth + (c + x);
//                    int subImageIdx = y * subImageWidth + x;
//                    subImages[r * cols + c][subImageIdx] = inputImage[inputIdx];
//                }
//            }
//        }
//    }
//}

void Image_Recognition_Reponse_Callback(void *parameter)    //图像识别任务回调函数
{
    while(1)
    {
        rt_thread_mdelay(10000);
    }
}

void Image_Recognition_Reponse(void)    //图像识别任务调用函数
{
    rmage_recognition_response_t = rt_thread_create("rmage_recognition_response_t", Image_Recognition_Reponse_Callback, RT_NULL, 2048, 10, 10);
    if(rmage_recognition_response_t!=RT_NULL)rt_thread_startup(rmage_recognition_response_t);
    LOG_I("Image_Recognition_Reponse Init Success\r\n");
}

void Binary_Image(int argc, rt_uint8_t *argv[])     //二值化命令
{
    Binary_Threshold_Rgb565((rt_uint16_t*)JpegBuffer, FRAME_SIZE);
//    Binary_Threshold_Rgb565((rt_uint16_t*)JpegBuffer, 160, 120);
    rt_device_write(camera_device_t.uart, 0, (rt_uint8_t*)Binary_JpegBuffer, PICTURE_BUFFER_LENGTH*4);
}

/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(Binary_Image, Binary image);
